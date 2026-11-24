from ucimlrepo import fetch_ucirepo # taken from the repo: https://github.com/uci-ml-repo/ucimlrepo

import pandas as pd
import numpy as np

from sklearn.preprocessing import LabelEncoder, StandardScaler

from sklearn.model_selection import train_test_split

from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import (accuracy_score, confusion_matrix, precision_score, recall_score, f1_score)

import seaborn as sns

import matplotlib.pyplot as plt

# =====================================
# PART 1: Load and Inspect the Dataset
# =====================================

def load_census_dataset() -> pd.DataFrame:
    # fetch dataset 
    census_income = fetch_ucirepo(id=20) # taken directly from https://archive.ics.uci.edu/dataset/20/census+income
    
    # data (as pandas dataframes) 
    X = census_income.data.features
    y = census_income.data.targets 

    # Combine into a single data frame
    df = pd.concat([X, y], axis=1)

    # print first few rows
    print("\nFirst few rows of Census dataset:")
    print(df.head())

    rows, cols = df.shape
    print(f"Number of rows : {rows:,}")
    print(f"Number of columns : {cols}")

    # identify attributes and their values
    print("\nAttributes and their values:")
    print(census_income.variables[['name','description']])

    # Check for missing or invalid values 
    print("\nMissing values:")
    print(df.isnull().sum())

    # Replace "?" with NaN, then drop those rows
    # the only invalid entries are the ? placeholder strings used for unknown values
    df.replace("?", pd.NA, inplace=True)
    df.dropna(inplace=True)
    print("Finished cleaning dataset")

    return df

# =====================================
# PART 2: Data Preprocessing
# =====================================

def preprocess_census_data(df: pd.DataFrame):
    # Separate features and target
    X = df.drop(columns=["income"])
    y_series = df["income"].str.strip().str.replace(".", "", regex=False)    

    # Encode the target
    y = (y_series == ">50K").astype(int)
    y.name = "income"

    # Encode categorical features
    categorical_cols = X.select_dtypes(include="object").columns.tolist()
    numeric_cols = X.select_dtypes(include="number").columns.tolist()

    # Do NOT want to overwrite raw data
    X_encoded = X.copy()
    le = LabelEncoder() # since target is binary, Label Encoder is used
    for col in categorical_cols:
        X_encoded[col] = le.fit_transform(X_encoded[col].astype(str))

    # Scale numeric features
    scaler = StandardScaler()
    X_scaled_arr = scaler.fit_transform(X_encoded)
    
    # Create final numeric feature matrix
    X_scaled = pd.DataFrame(X_scaled_arr, columns=X_encoded.columns, index=X_encoded.index)
    print(f"X_scaled shape : {X_scaled.shape}")
    print(f"({X_scaled.shape[0]:,} rows, {X_scaled.shape[1]} features)")

    return X_scaled, y

# =====================================
# PART 3: Train/Test Split
# =====================================

def split_data(X_scaled, y) -> tuple[pd.DataFrame, pd.DataFrame, pd.Series, pd.Series]:
    # Splitting data
    X_train, X_test, y_train, y_test = train_test_split(
        X_scaled, y,
        test_size=0.2, # 20% testing, 80% training
        random_state=42 # Set random state
    )

    print("Train/Test Split:")
    print(f"Total samples: {len(X_scaled):,}")
    print(f"Training size: {len(X_train):,}")
    print(f"Testing size: {len(X_test):,}")

    return X_train, X_test, y_train, y_test

# =====================================
# PART 4: K-Nearest Neighbors
# ====================================

def train_and_evaluate_knn(X_train, X_test, y_train, y_test, k_values) -> pd.DataFrame:
    records = []

    for k in k_values:
        # Initialize the model with the current k
        knn = KNeighborsClassifier(n_neighbors=k)
        
        # Train the model
        knn.fit(X_train, y_train)

        # Make predictions on the test data
        y_pred = knn.predict(X_test)

        # Calculate metrics
        acc = accuracy_score(y_test, y_pred)
        prec = precision_score(y_test, y_pred, zero_division=0)
        rec = recall_score(y_test, y_pred, zero_division=0)
        f1 = f1_score(y_test, y_pred, zero_division=0)

        cm = confusion_matrix(y_test, y_pred)

        # Unpack the 2x2 matrix into 4 variables
        tn, fp, fn, tp = cm.ravel()

        # Keep a record of all the calculations
        records.append({
            "k": k, 
            "accuracy": acc, 
            "precision": prec,
            "recall": rec, 
            "f1_score": f1,
            "TP": tp, 
            "TN": tn, 
            "FP": fp, 
            "FN": fn
        })

        # Output results for every k
        print(f"For k = {k}")
        print(f"Accuracy  : {acc:.4f}")
        print(f"Precision : {prec:.4f}")
        print(f"Recall    : {rec:.4f}")
        print(f"F1 Score  : {f1:.4f}")
        print(f"Confusion Matrix:")
        print(f"True Negatives (correct rejections): {tn}")
        print(f"False Positives (false approvals): {fp}")
        print(f"False Negatives (missed qualifier): {fn}")
        print(f"True Positives (correct approval): {tp}")

    # Summary Table
    results = pd.DataFrame(records).set_index("k")

    return results

# =====================================
# PART 5: Choose the Best K
# ====================================
def choose_best_k(results, k_values) -> int:    
    # Make a list of all accuracies
    accuracy = results["accuracy"].tolist()

    # Best k actually comes from f1 score because dataset is inbalanced
    best_k = results["f1_score"].idxmax()
    best_acc = results.loc[best_k, "accuracy"]

    print(f"Best k: {best_k}")

    # Plot
    fig, ax = plt.subplots(figsize=(9, 5))

    ax.plot(k_values, accuracy,  marker="o", label="Accuracy",  color="steelblue",  linewidth=2)
    
    # Mark best k
    ax.axvline(x=best_k, color="crimson", linestyle="--", linewidth=1.5, label=f"Best k = {best_k}")
    ax.scatter([best_k], [best_acc], color="steelblue",  zorder=5, s=100)

    # Make graph look pretty
    ax.set_xlabel("k  (number of neighbors)", fontsize=12)
    ax.set_ylabel("Score", fontsize=12)
    ax.set_xticks(k_values)
    ax.set_ylim(0.75, 0.90)
    ax.legend(fontsize=10)
    ax.grid(True, linestyle="--", alpha=0.5)

    plt.tight_layout()
    plt.savefig("knn_accuracy_vs_k.png", dpi=150)
    print("\n  Plot saved to knn_accuracy_vs_k.png")

    return best_k

# =====================================
# PART 6: Final Model Evaluation
# ====================================

def final_model_eval(X_train, X_test, y_train, y_test, best_k) -> None:
    # Initialize model with the best k found
    model = KNeighborsClassifier(n_neighbors=best_k)
    
    # Train the model on the full training set
    model.fit(X_train, y_train)

    # Predict the test ste
    y_pred = model.predict(X_test)

    # Evaluate it
    acc  = accuracy_score(y_test, y_pred)
    prec = precision_score(y_test, y_pred, zero_division=0)
    rec  = recall_score(y_test, y_pred, zero_division=0)
    f1   = f1_score(y_test, y_pred, zero_division=0)
    cm   = confusion_matrix(y_test, y_pred)
    tn, fp, fn, tp = cm.ravel()

    print("Final Model Evaluation:")
    print(f"Accuracy: {acc:.4f}")
    print(f"Precision: {prec:.4f}")
    print(f"Recall: {rec:.4f}")
    print(f"F1 Score: {f1:.4f}")

    # Display confusion matrix
    fig, ax = plt.subplots(figsize=(6, 5))

    # Label each quadrant
    labels = np.array([
        [f"True Negative\n{tn}", f"False Positive\n{fp}"],
        [f"False Negative\n{fn}", f"True Positive\n{tp}"]
    ])

    sns.heatmap(cm, annot=labels, fmt="", cmap="Blues", ax=ax,
                xticklabels=["Pred <=50K", "Pred >50K"],
                yticklabels=["Act <=50K",  "Act >50K"])

    ax.set_title(f"Confusion Matrix — KNN (k={best_k})", fontsize=13, pad=12)
    ax.set_ylabel("Actual Label", fontsize=11)
    ax.set_xlabel("Predicted Label", fontsize=11)

    plt.tight_layout()
    plt.savefig("confusion_matrix_final.png", dpi=150)
    plt.show()

    # Interpret errors
    fp_rate = fp / (fp + tn) * 100   # rate of false positives
    fn_rate = fn / (fn + tp) * 100   # rate of false negatives 

    print("Error Interpretation:")
    print(f"Of the actual <=50K, {fp_rate:.1f}% (false positive rate) were wrongly approved")
    print(f"Of the actual >50K {fn_rate:.1f}% (false negative rate) were wrongly rejected")

def main():
    # ------ PART 1 --------
    print("------ PART 1 --------")
    df = load_census_dataset()

    # ------ PART 2 --------
    print("------ PART 2 --------")
    X_scaled, y = preprocess_census_data(df)

    # ------ PART 3 --------
    print("------ PART 3 --------")
    X_train, X_test, y_train, y_test = split_data(X_scaled, y)

    # ------ PART 4 --------
    print("------ PART 4 --------")
    k_values=[1, 3, 5, 7, 9, 11, 15, 20]
    results = train_and_evaluate_knn(X_train, X_test, y_train, y_test, k_values)

    # ------ PART 5 & 7.1 --------
    print("------ PART 5 --------")
    best_k = choose_best_k(results, k_values)

    # ------ PART 6 & 7.2 --------
    print("------ PART 6 --------")
    final_model_eval(X_train, X_test, y_train, y_test, best_k)

    return 0

if __name__ == "__main__":
    main()