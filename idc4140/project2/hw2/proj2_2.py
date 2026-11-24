import requests
import pandas as pd

import numpy as np
from sklearn.preprocessing import StandardScaler

import matplotlib.pyplot as plt

from scipy.spatial.distance import pdist, squareform
from scipy.cluster.hierarchy import linkage, dendrogram, fcluster
# =====================================
# PART 1: Obtain the Data
# =====================================

def fetch_dinosaur_data() -> pd.DataFrame:
    # taken from https://paleobiodb.org/data1.2/
    url = "https://paleobiodb.org/data1.2/occs/list.csv"

    params = {
        "min_ma": 65,
        "max_ma": 200,
        "base_name": "Dinosauria", # adding a base name in order to include all subgroups and not entire data set
        "show": "coords,classext", # coords = geographic coordinates (long and lat), classext = full biological hierarchy for every fossil (phylum, class, order, family, and genus)
        "limit": "all"
    }  

    response = requests.get(url, params=params) 

    if response.status_code == 200:
        # Use io.StringIO to turn the text response into a file-like object for pandas
        from io import StringIO
        df = pd.read_csv(StringIO(response.text), comment='#')
    else:
        print(f"Error {response.status_code}: {response.text}")

    print("Display first few rows")
    print(df[['occurrence_no', 'max_ma', 'min_ma']].head()) # only these 3 attributes for now
    print(f"Total records: {len(df)}")

    # Unique taxa — use 'accepted_name' (the accepted taxonomic name per PBDB)
    taxon_col = 'accepted_name' if 'accepted_name' in df.columns else 'taxon_name'
    n_taxa = df[taxon_col].nunique()
    print(f"Unique taxa: {n_taxa}")

    print(f"Available attributes ({len(df.columns)} columns):")
    for col in df.columns:
        print(f"    - {col}")

    return df

# =====================================
# PART 2: Clean and Filter the Dataset
# =====================================

def clean_dinosaur_data(df: pd.DataFrame) -> pd.DataFrame:

    # Keep original size
    original_size = len(df)
    print(f"Original dataset size: {original_size} records")

    genus_ranks = ['genus', 'species'] # genus and species work as labels
    # Genus identifies which taxon a row represents (like a student ID)
    # Species is another rank filter that tells us which genus the fossil belongs to

    # Filter dataset to retain genus/species-level records
    if 'identified_rank' in df.columns: # First check whether identified_rank column exists in df
        df = df[df['identified_rank'].isin(genus_ranks)].copy() # keep rows where rank is either genus or species
        df = df.dropna(subset=['accepted_name']) # drops any remaning rows where accepted_name is null
    else:
        print("\n 'identified_rank' not found; skipping rank filter")

    # Retain usable fields
    # These are numeric and continuous variables that describe the physical context of the fossil (space and time)
    desired_cols = [
        # These are discarded from matrix but kept as labels
        'accepted_name', # taxon identity (genus label)
        'identified_rank', # taxonomic rank of the identification
        'identified_name', # original field identification name

        # Goegraphic attributes
        'max_ma', # oldest age bound (Ma) = temporal range start
        'min_ma', # youngest age bound (Ma) = temporal range end

        # Temporal attributes
        'lat', # latitude of occurrence = geographic distribution
        'lng', # longitude of occurrence = geographic distribution
    ]

    # Keep only desired columns
    df = df[desired_cols].copy()
    print(f"Selected columns: {desired_cols}")
    
    # Filter out incomplete data
    taxon_col = 'accepted_name'
    if taxon_col in df.columns:
        n_taxa = df[taxon_col].dropna().nunique()
    else:
        n_taxa = 'N/A'

    print("\nFirst few rows of cleaned dataset:")
    print(df.head().to_string())
        
    removed = original_size - len(df)
    print(f"\nRemoved {removed} records")
    print(f"Remaining: {len(df)} records")

    return df

# =====================================
# PART 3: Build a Taxon-Level Feature Table
# =====================================

def build_feature_matrix(df: pd.DataFrame) -> pd.DataFrame:
    taxon_col = 'accepted_name'

    # Define your time bins (from 200 down to 60)
    bins = np.arange(200, 50, -10) 
    bin_names = [f"{bins[i]}-{bins[i+1]}Ma" for i in range(len(bins)-1)]

    # Convert occurrence data into numeric features
    records = [] # keep records of all these features in a list
    for genus, grp in df.groupby(taxon_col):

        # first appearance age
        # refers to the oldest geological moment a specific taxon shows up in fossil record
        # = Maximum value of max_ma across all occurrences
        first_appearance = grp['max_ma'].max()

        # last appearance age
        # represents the most recent known age bound
        # = Minimum value of min_ma across all occurrences
        last_appearance = grp['min_ma'].min()

        # temporal range length
        # indicate genera that persisted through time
        # = Difference between first and last appearance in Ma
        temporal_range = first_appearance - last_appearance

        # nunmber of occurence
        # reflects how well-sampled the genus is in the record
        # = Total number of fossil occurrence records for this genus
        n_occurrences = len(grp)

        # number of collections
        # a collection is the specific site where one or more fossiles were found
        # = number of unique collection IDs
        n_collections = (
            grp['collections'].nunique()
            if 'collections' in grp.columns
            else 0
        )

        # number of regions
        # Captures geographic spread without requiring formal country labels
        # = number of distinct geographic regions
        # first approximate by rounding lat/lng to the nearest 10 degrees
        lat_bins = (grp['lat'] / 10).round() * 10 
        lng_bins = (grp['lng'] / 10).round() * 10
        # then count unique (lat_bin, lng_bin) cells
        n_regions = pd.Series(list(zip(lat_bins, lng_bins))).nunique()

        # presence in time bins
        # A dinosaur is present if its MAX age is > lower AND its MIN age is < upper
        # # = 1 if present, 0 otherwise
        for i in range(len(bins)-1):
            upper = bins[i]
            lower = bins[i+1]

            df[bin_names[i]] = ((df['max_ma'] > lower) & (df['min_ma'] < upper)).astype(int)

        time_features = df[bin_names]

        row = {
            taxon_col : genus,
            'first_appearance': round(first_appearance, 2),
            'last_appearance' : round(last_appearance,  2),
            'temporal_range' : round(temporal_range,   2),
            'n_occurrences' : n_occurrences,
            'n_collections' : n_collections,
            'n_regions' : n_regions,
            **time_features,
        }
        records.append(row)

    # Build numeric feature matrix
    feature_df = pd.DataFrame(records).set_index(taxon_col)

    # Filter out any non-numeric columns
    numeric_df = feature_df.select_dtypes(include=[np.number])
    
    # Handle missing values
    numeric_df = numeric_df.fillna(0)

    print(f"\nRaw feature matrix shape: {numeric_df.shape}")

    # scale
    scaler = StandardScaler()
    X_scaled = scaler.fit_transform(numeric_df)
    
    # rebuild the scaled data frame
    feature_scaled_df = pd.DataFrame(
        X_scaled,
        index=numeric_df.index,
        columns=numeric_df.columns
    )

    print("\n  Preview of scaled feature matrix (first 5 taxa):")
    print(feature_scaled_df.head().to_string())

    feature_scaled_df.to_csv('dinosaur_features_scaled_clean.csv')
    print("Scaled feature table saved successfully.")

    return feature_scaled_df

# =====================================
# PART 4: Exploratory Data Analysis
# =====================================

def eda(feature_scaled_df: pd. DataFrame) -> None:
    # Summary Statistics
    print("Summary Statistics: \n")
    print(feature_scaled_df.describe().round(2).to_string())
    
    # Histogram of occurences
    fig, ax = plt.subplots(figsize=(13, 9))
    fig.suptitle("EDA — Dinosaur Genus Feature Matrix", fontsize=14, fontweight='bold')

    # Most genera are rare in the fossil record; a few iconic ones dominate
    # We use a log-scaled x-axis to make the right-skewed distribution legible
    ax.hist(feature_scaled_df['n_occurrences'], bins=40, color='steelblue',
             edgecolor='white', linewidth=0.5)
    ax.set_title("Histogram of Occurrence Counts per Genus")
    ax.set_xlabel("Number of Occurrences")
    ax.set_ylabel("Number of Genera")
    ax.set_yscale('log')          # log y-axis: many genera with few occurrences
    ax.grid(True, linestyle='--', alpha=0.5)

    plt.tight_layout()
    plt.savefig("histogram_n_occurences.png", dpi=150)
    print("\n  Plot saved to histogram_n_occurences.png")

# =====================================
# PART 5: Hierarchical Clustering
# =====================================

def complete_linkage(feature_scaled_df: pd.DataFrame) -> None:
    # Compute distance metric    
    dist_condensed = pdist(feature_scaled_df.values, metric='euclidean')
    dist_matrix = squareform(dist_condensed)
    
    # Convert to a DataFrame so we keep the taxon names as labels
    dist_df = pd.DataFrame(
        dist_matrix,
        index=feature_scaled_df.index,
        columns=feature_scaled_df.index
    )

    # Perform hierarchical clustering using complete linkage
    Z = linkage(dist_condensed, method='complete', metric='euclidean')

    plt.figure(figsize=(15, 7))
    
    # Generate a dendogram
    dendrogram(
        Z,
        labels=feature_scaled_df.index,
        leaf_rotation=90,
        leaf_font_size=8,
        truncate_mode='lastp',
        p=40
    )

    plt.ylabel("Maximum Euclidean Distance")
    plt.xlabel("Taxa Groups")
    plt.savefig("dendogram.png", dpi=150)
    print("\n  Dendogram saved to dendogram.png")

def main():
    # ------ PART 1 --------
    print("------ PART 1 --------")
    df = fetch_dinosaur_data()

    # ------ PART 2 --------
    print("------ PART 2 --------")
    clean_df = clean_dinosaur_data(df)

    # ------ PART 3 --------
    print("------ PART 3 --------")
    feature_scaled_df = build_feature_matrix(clean_df)

    # ------ PART 4 --------
    print("------ PART 4 --------")
    eda(feature_scaled_df)

    # ------ PART 5 --------
    print("------ PART 5 --------")
    complete_linkage(feature_scaled_df)
    
    return 0

if __name__ == "__main__":
    main()
