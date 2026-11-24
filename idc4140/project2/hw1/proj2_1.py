"""
IDC 4140 - Programming Assignment 2
Clustering and Classification
Due 04-07-2026

All the work is done by Cristian Victoria
"""

from ucimlrepo import fetch_ucirepo # taken from the repo: https://github.com/uci-ml-repo/ucimlrepo

import pandas as pd

from sklearn.preprocessing import OrdinalEncoder
from sklearn.decomposition import PCA
from sklearn.preprocessing import MinMaxScaler

import numpy as np
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt

# =====================================
# PART 1: Load and Inspect the Dataset
# =====================================

def load_car_dataset() -> pd.DataFrame:
    # ----- 1. fetch dataset 
    car_evaluation = fetch_ucirepo(id=19)   # taken directly from https://archive.ics.uci.edu/dataset/19/car+evaluation
    
    # data (as pandas dataframes) 
    X = car_evaluation.data.features 
    y = car_evaluation.data.targets 

    # Combine into a single data frame
    df = pd.concat([X, y], axis=1)

    # ----- 2. print first few rows
    print("\nFirst few rows of Car Evaluation dataset:")
    print(df.head())

     # ----- 3. identify attributes and their values
    print("\nAttributes and their values:")
    print(car_evaluation.variables[['name','description']])

    #  ----- 4. Check for missing or invalide values 
    print("\nCheck for missing values:")
    print(df.isnull().values.any())
    
    return df

# =====================================
# PART 2: Preprocess the Data
# =====================================

def preprocess_data(df: pd.DataFrame) -> pd.DataFrame:
    # separate features from target, 'class'
    feature_cols = [col for col in df.columns if col != 'class']
    X = df[feature_cols]
   
    # explicitly define the orderings for each feature
    # this is done in order to preserve the orderings
    ordinal_categories = [
        ['low', 'med', 'high', 'vhigh'],   # buying
        ['low', 'med', 'high', 'vhigh'],   # maint
        ['2', '3', '4', '5more'],          # doors
        ['2', '4', 'more'],                # persons
        ['small', 'med', 'big'],           # lug_boot
        ['low', 'med', 'high'],            # safety
    ] 
    
    # Ordinal Encoder
    encoder = OrdinalEncoder(categories=ordinal_categories)
    X_encoded = encoder.fit_transform(X)
    X_encoded_df = pd.DataFrame(X_encoded, columns=feature_cols)

    print("\nPreview of transformed data:")
    print(X_encoded_df.head())

    print(f"\nTransformed feature matrix shape: {X_encoded_df.shape} = {X_encoded_df.shape[0]} samples and {X_encoded_df.shape[1]} original features")

    return X_encoded_df

# =====================================
# PART 3: Apply PCA
# =====================================

def apply_pca(X_encoded_df: pd.DataFrame, n_components: int) -> pd.DataFrame:
    # PCA is sensitive to the scale of features
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X_encoded_df)

    pca_all = PCA()
    pca_all.fit(X_scaled)
    
    ev_ratio = pca_all.explained_variance_ratio_
    cumulative_ev_ratio = ev_ratio.cumsum()
    
    print(f"{'Component':<12} {'Explained Var':>14} {'Cumulative':>12}")
    print("-" * 55)
    for i, (var, cum) in enumerate(zip(ev_ratio, cumulative_ev_ratio), start=1):
        print(f"PC{i:<10} {var:>13.4f}  {cum:>11.4f}")

    pca_fit = PCA(n_components=n_components)
    X_pca_fit = pca_fit.fit_transform(X_scaled)

    pca_cols = [f"PC{i+1}" for i in range(n_components)]
    X_pca_df = pd.DataFrame(X_pca_fit, columns=pca_cols)

    print(f"\nTransformed feature matrix shape: {X_pca_df.shape} = {X_pca_df.shape[0]} samples and {X_pca_df.shape[1]} principal components\n")

    print("Preview of transformed data (first 5 rows):")
    print(X_pca_df.head())

    print(f"\nVariance captured by the 3 selected components:")
    for i, var in enumerate(pca_fit.explained_variance_ratio_, start=1):
        print(f"  PC{i}: {var:.4f}  ({var*100:.2f}%)")
    print(f"  Cumulative: {pca_fit.explained_variance_ratio_.sum():.4f}  "
          f"({pca_fit.explained_variance_ratio_.sum()*100:.2f}%)")

    return X_pca_df

# =====================================
# PART 4: K-Means Clustering
# =====================================

def apply_kmeans(k_values: list[int], X_pca_df: pd.DataFrame) -> list[int]:

    inertias = []   # how cumpact are the clusters, where lower = better

    print("Evaluation results:")
    print(f"{'k':<6} {'Inertia':>14}")
    for k in k_values:
        # random_state=42 ensures reproducibility across runs
        kmeans = KMeans(n_clusters=k, random_state=42, n_init=10)
        labels = kmeans.fit_predict(X_pca_df)

        inertia = kmeans.inertia_

        inertias.append(inertia)

        print(f"{k:<6} {inertia:>14.4f}")

    return inertias

# =====================================
# PART 4: Determine the Best Clustering
# =====================================

def determine_best(k_values: list[int], inertias: list[int]) -> int:
    # identify best k from elbow method
    # Compute the second derivative of inertia
    # the elbow is where the rate of decrease itself decreases the most sharply.
    deltas = np.diff(inertias) # first differences
    delta2 = np.diff(deltas) # second differences
    elbow_idx = np.argmax(delta2) + 1 # +1 because diff shortens array
    best_k = k_values[elbow_idx]
    print(f"Best K: {best_k}")

    # Evidence
    # Elbow Method Plot
    fig, ax = plt.subplots(figsize=(10, 6))

    # Elbow curve (inertia)
    ax.plot(k_values, inertias, marker='o', color='steelblue', linewidth=2)
    ax.set_title("Elbow Method — Inertia vs. k", fontsize=13)
    ax.set_xlabel("Number of Clusters (k)")
    ax.set_ylabel("Inertia (Within-Cluster Sum of Squares)")
    ax.set_xticks(k_values)
    ax.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout()
    plt.savefig("kmeans_elbow.png", dpi=150)
    print("\nPlot saved to kmeans_elbow.png")

    return best_k

def visualize_clusters(X_pca_df: pd.DataFrame, best_k: int) -> None:
    # Fit final K-Means with the selected k
    kmeans_final = KMeans(n_clusters=best_k, random_state=42, n_init=10)
    labels = kmeans_final.fit_predict(X_pca_df)
    centroids = kmeans_final.cluster_centers_

    # Add cluster labels to a DataFrame
    X_clustered = X_pca_df.copy()
    X_clustered['Cluster'] = labels

    # Report cluster sizes

    print("Cluster Sizes:")
    cluster_sizes = X_clustered['Cluster'].value_counts().sort_index()
    print(f"{'Cluster':<10} {'Size':>8} {'% of Data':>12}")
    for cluster_id, size in cluster_sizes.items():
        pct = size / len(X_clustered) * 100
        print(f"  {cluster_id:<8} {size:>8}   {pct:>9.2f}%")
    print(f"  {'Total':<8} {len(X_clustered):>8}   {'100.00%':>10}")

    # 2D Scatter of PC1 vs PC2
    colors = plt.cm.tab10.colors  # distinct colors for up to 10 clusters

    fig, ax = plt.subplots(figsize=(10, 6))

    for cid in range(best_k):
        mask = labels == cid
        ax.scatter(
            X_pca_df.loc[mask, 'PC1'],
            X_pca_df.loc[mask, 'PC2'],
            c=[colors[cid]],
            label=f'Cluster {cid}',
            alpha=0.5,
            s=20
        )
    # Plot centroids
    ax.scatter(
        centroids[:, 0], centroids[:, 1],
        c='black', marker='X', s=150, zorder=5, label='Centroids'
    )
    ax.set_title(f"K-Means (k={best_k}) — PC1 vs PC2", fontsize=12)
    ax.set_xlabel("PC1")
    ax.set_ylabel("PC2")
    ax.legend(fontsize=8)
    ax.grid(True, linestyle='--', alpha=0.4)

    plt.tight_layout()
    plt.savefig("clusters_2d.png", dpi=150)

    print("\nPlots saved to clusters_2d.png")


# =====================================
# MAIN
# =====================================

def main():
    # ------ PART 1 --------
    print("------ PART 1 --------")
    df = load_car_dataset()

    # ------ PART 2 --------
    print("\n------ PART 2 --------")
    X_encoded_df = preprocess_data(df)

    # ------ PART 3 --------
    print("\n------ PART 3 --------")
    X_pca_df = apply_pca(X_encoded_df, 3)

    # ------ PART 4 --------
    print("\n------ PART 4 --------")
    # range of k = 2,3,4,5,6,7,8,9,10
    k_range: range = range(2, 11)
    k_values = list(k_range)

    inertias = apply_kmeans(k_values, X_pca_df)

    # ------ PART 5 --------
    print("\n------ PART 5 --------")
    best_k = determine_best(k_values, inertias)
    visualize_clusters(X_pca_df, best_k)

    return 0

if __name__ == "__main__":
    main()