import numpy as np
from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
import warnings
warnings.filterwarnings('ignore')

# ------------------------------ 
# Function Definitions
# ------------------------------

def load_data():
    """Load the Scene dataset from text files"""

    print("Loading Scene dataset...")
    
    # Load feature data (tab-delimited)
    X_train = np.loadtxt('DataProblem1/X_train.txt', delimiter='\t')
    X_test = np.loadtxt('DataProblem1/X_test.txt', delimiter='\t')
    
    # X_test loaded as (294, 907) instead of (907, 294) so we need to transpose it to match training data format.
    if X_test.shape[0] != X_train.shape[0] and X_test.shape[1] != X_train.shape[1]:
        X_test = X_test.T
    
    # Load label data (tab-delimited binary matrix)
    y_train = np.loadtxt('DataProblem1/y_train.txt', delimiter='\t', dtype=int)
    y_test = np.loadtxt('DataProblem1/y_test.txt', delimiter='\t', dtype=int)
    
    print(f"\nTraining set: {X_train.shape[0]} samples × {X_train.shape[1]} features")
    print(f"Test set: {X_test.shape[0]} samples × {X_test.shape[1]} features")
    print(f"Number of classes: {y_train.shape[1]}")
    print(f"Training labels: {y_train.shape}")
    print(f"Test labels: {y_test.shape}")
    
    return X_train, X_test, y_train, y_test

def compute_multilabel_accuracy(y_true, y_pred):
    """Compute multi-label accuracy"""
    n_samples = y_true.shape[0]
    accuracies = []
    
    for i in range(n_samples):
        true_labels = y_true[i, :]
        pred_labels = y_pred[i, :]
        
        # Intersection: both are 1
        intersection = np.sum(np.logical_and(true_labels == 1, pred_labels == 1))
        
        # Union: at least one is 1
        union = np.sum(np.logical_or(true_labels == 1, pred_labels == 1))
        
        # Handle edge case where both vectors are all zeros
        if union == 0:
            accuracy = 1.0
        else:
            accuracy = intersection / union
        
        accuracies.append(accuracy)
    
    return np.mean(accuracies)

def train_multilabel_svm(X_train, y_train, kernel='poly', param=2):
    """ Train one SVM per class """
    
    # Standardize features
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    
    n_classes = y_train.shape[1]
    models = []
    
    print(f"\nTraining SVMs with {kernel} kernel (parameter={param})...")
    
    for i in range(n_classes):        
        if kernel == 'poly':
            svm = SVC(
                kernel='poly',
                degree=int(param),
                coef0=1,
                gamma='scale',
                C=1.0,
                class_weight='balanced',
                random_state=42
            )
        elif kernel == 'rbf':
            svm = SVC(
                kernel='rbf',
                gamma=param,
                C=1.0,
                class_weight='balanced',
                random_state=42
            )
        else:
            raise ValueError(f"Unknown kernel: {kernel}")
        
        # Train binary classifier for this class
        svm.fit(X_train_scaled, y_train[:, i])
        models.append(svm)
        # print("Done.")
    
    return models, scaler

def predict_multilabel(models, scaler, X_test):
    """Make predictions on test set"""
    X_test_scaled = scaler.transform(X_test)
    n_samples = X_test.shape[0]
    n_classes = len(models)
    
    predictions = np.zeros((n_samples, n_classes), dtype=int)
    
    for i, model in enumerate(models):
        predictions[:, i] = model.predict(X_test_scaled)
    
    return predictions

# ------------------------------ 
# Main Execution 
# ------------------------------

# Load data
X_train, X_test, y_train, y_test = load_data()
    
results = []

# Execute (i): Polynomial Kernel
print("\n------------------------------")
print("\n(i) SVM with Polynomial Kernel")

models_poly, scaler_poly = train_multilabel_svm(X_train, y_train, kernel='poly', param=2)

print("\nMaking predictions on test set...")
y_pred_poly = predict_multilabel(models_poly, scaler_poly, X_test)

accuracy_poly = compute_multilabel_accuracy(y_test, y_pred_poly)

print(f"\nTest Accuracy with Polynomial Kernel: {accuracy_poly * 100:.2f}%")

# Execute (ii): Gaussian Kernel
print("\n------------------------------")
print("\n(ii) SVM WITH Gaussian KERNEL")

models_rbf, scaler_rbf = train_multilabel_svm(X_train, y_train, kernel='rbf', param=2)

print("\nMaking predictions on test set...")
y_pred_rbf = predict_multilabel(models_rbf, scaler_rbf, X_test)

accuracy_rbf = compute_multilabel_accuracy(y_test, y_pred_rbf)

print(f"\nTest Accuracy with Gaussian Kernel: {accuracy_rbf * 100:.2f}%\n")