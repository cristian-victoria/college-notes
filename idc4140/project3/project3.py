from tqdm import tqdm
import numpy as np
import torch
import torchvision.transforms as transforms

import medmnist
from medmnist import DermaMNIST

import torch.utils.data as data

from sklearn.linear_model import LogisticRegression
from sklearn.metrics import classification_report, accuracy_score, confusion_matrix
from sklearn.model_selection import GridSearchCV

import torch.nn as nn
import torch.optim as optim
import time
import matplotlib.pyplot as plt

# =====================================
# PART 1: Data Exploration and Preprocessing
# =====================================

def preprocess() -> tuple[data.Dataset, data.Dataset, data.Dataset]:
    # Preprocessing - Taken from MedMNIST GitHub Repo
    data_transform = transforms.Compose([
        transforms.ToTensor(),  # Convert images to tensors
        transforms.Normalize(mean=[.5], std=[.5]) # Normalize pixel values
    ])

    # Standard 28x28 version (MNIST-like) - Taken from MedMNIST GitHub Repo
    train_dataset = DermaMNIST(split="train", transform=data_transform, download=True)
    val_dataset   = DermaMNIST(split="val", transform=data_transform, download=True)
    test_dataset  = DermaMNIST(split="test", transform=data_transform, download=True)  

    return train_dataset, val_dataset, test_dataset

def save_montage(train_dataset: data.Dataset, val_dataset: data.Dataset, test_dataset: data.Dataset) -> None:
    # Save montage images
    train_img = train_dataset.montage(length=20)
    train_img.save("train-dataset-montage-20.png")
    val_img = val_dataset.montage(length=20)
    val_img.save("val-dataset-montage-20.png")
    test_img = test_dataset.montage(length=20)
    test_img.save("test-dataset-montage-20.png")  

def get_label_counts(dataset: data.Dataset) -> dict:
    # Counts the occurence of each unique class label
    counts = {}
    for i in range(len(dataset)):
        label = dataset[i][1].item()
        if label in counts:
            counts[label] += 1
        else:
            counts[label] = 1
    return counts

def analyze_class_distribution(class_names: list, dataset_counts: dict) -> None:
    for i, name in enumerate(class_names):
        print(f"{name}, {dataset_counts[i]}")

# =====================================
# PART 2: Logistic Regression
# =====================================

def loader_to_numpy(loader) -> tuple[np.ndarray, np.ndarray]:
    all_images, all_labels = [], []
    for images, labels in loader:
        all_images.append(images.view(images.size(0), -1).numpy()) # convert all image batches to NumPy array
        all_labels.append(labels.numpy()) # convert all labels to NumPy array
    return np.concatenate(all_images), np.concatenate(all_labels).flatten() # flatten image into 2D format and concatenates everything into two arrays: input and target labels

def log_reg(train_dataset: data.Dataset, val_dataset: data.Dataset, test_dataset: data.Dataset) -> tuple[data.DataLoader, data.DataLoader, data.DataLoader]:
    # 1. Flatten each image into a vector
    
    BATCH_SIZE = 128

    # encapsulate data into dataloader form - Taken from MedMNIST GitHub repo
    train_loader = data.DataLoader(dataset=train_dataset, batch_size=BATCH_SIZE, shuffle=True)
    val_loader = data.DataLoader(dataset=val_dataset, batch_size=2*BATCH_SIZE, shuffle=False) # not shuffled to get consistent evaluation
    test_loader = data.DataLoader(dataset=test_dataset, batch_size=2*BATCH_SIZE, shuffle=False) # not shuffled to get consistent evaluation


    X_train, y_train = loader_to_numpy(train_loader)
    X_val,   y_val   = loader_to_numpy(val_loader)
    X_test,  y_test  = loader_to_numpy(test_loader)

    print("Model Dimensions: ")
    print(f"X_train shape : {X_train.shape}")   # (7007, 2352)
    print(f"X_val shape   : {X_val.shape}")
    print(f"X_test shape  : {X_test.shape}")

    # 2. Train a multi-class Logistic Regression model

    # Initialize model
    model = LogisticRegression(solver='lbfgs', max_iter=2000)
    print("Initialized model.")

    # Train the model
    model.fit(X_train, y_train)
    print("Trained model.")

    # Define the search space (powers of 10 are a standard starting point)
    param_grid = {'C': [0.01, 0.1, 1, 10]}

    # Adjust one hyper parameter
    # Initialize the grid search
    # 'cv=5' means 5-fold cross-validation
    grid_search = GridSearchCV(model, param_grid, cv=5, scoring='accuracy', verbose=2)

    # Run the search
    print("Running grid search... ")
    grid_search.fit(X_train, y_train)
    print("Finished grid search.")

    # Get the results
    print(f"Best C value: {grid_search.best_params_['C']}")
    print(f"Best Cross-Validation Accuracy: {grid_search.best_score_:.4f}")

    # Use the best model to predict on test data
    best_model = grid_search.best_estimator_
    test_acc = best_model.score(X_test, y_test)

    y_pred = best_model.predict(X_test)
    print("Final Test Accuracy:", test_acc)
    print("\nClassification Report:\n", classification_report(y_test, y_pred))

    return train_loader, val_loader, test_loader

# =====================================
# PART 3: Neural Network
# =====================================

#Define Architecture
class DermaMNIST_NN(nn.Module):
    def __init__(self, input_size, hidden_size, num_classes) -> None:
        super(DermaMNIST_NN, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)   # First linear transformation
        self.relu = nn.ReLU()                           # Activation function
        self.fc2 = nn.Linear(hidden_size, num_classes)  # Second linear transformation

    def forward(self, x):
        # The first linear transformation, where the flattened image vector x (2352 values) is multiplied by the weight matrix W and added with a bias b, producing a 128 output valie (one per hidden neuron)
        z1 = self.fc1(x)           # z^(1) = W^(1)x + b^(1)

        # This applied the ReLU activation function to each of the 128 values
        # ReLU is defined as f(z) = max(0,z), meaning that positive values remain the same while negative values are set to 0. This is what makes the NN non linear. 
        a1 = self.relu(z1)         # a^(1) = f(z^(1))

        # This is the second linear transformation, taking the 128 activated hidden values and mapping them to 7 output valies (one score per class). 
        # The higher the score for a class, the more confident the model is that the input belongs to that class.
        z2 = self.fc2(a1)          # z^(2) = W^(2)a^(1) + b^(2)
        
        # This is returned without explicitly applying softmax because nn.CrossEntropyLoss applies it internally. 
        return z2 # ŷ = softmax(z^(2))

def neural_net(class_names: list, train_loader: data.DataLoader, val_loader: data.DataLoader, test_loader: data.DataLoader) -> None:
    INPUT_SIZE   = 2352   # 3 * 28 * 28 (flattened image)
    HIDDEN_SIZE  = 128    # neurons in hidden layer
    num_classes = len(class_names)
    LEARNING_RATE = 0.001
    NUM_EPOCHS   = 20

    # Initialize model
    model = DermaMNIST_NN(INPUT_SIZE, HIDDEN_SIZE, num_classes)
    print(model)
    print(f"\nTrainable parameters: {sum(p.numel() for p in model.parameters()):,}")

    # This is the loss function, it measures how wrong the model's predictions are
    # For each sample, it compares the model's predicted probabilities, ŷ, against the true label, y, across all C (=7) classes
    # Includes softmax internally, used to convert the 7 raw scores into probabilitie that sum to 1, and the class with the highest probability is the model's prediction
    criterion = nn.CrossEntropyLoss()    # L = -Σ (C, i=1) y_i log(ŷ_i) 
    
    # This is the gradient update rule, which adjusts the model's weights after each batch to reduce the loss
    # Adam is an improved version of basic Stochastic Gradient Descent (SGD)
    optimizer = optim.Adam(model.parameters(), lr=LEARNING_RATE)    # w ← w - η * ∂L/∂w 

    # Training
    train_losses, val_losses     = [], []
    train_accuracies, val_accuracies = [], []

    start_time = time.time()
    for epoch in range(NUM_EPOCHS):
        # Training process
        epoch_start = time.time()
        model.train()
        correct, total, running_loss = 0, 0, 0.0

        for images, labels in train_loader:
            images = images.view(images.size(0), -1)   # flatten: (B, 2352)
            labels = labels.squeeze().long()

            optimizer.zero_grad()

            # Forward pass
            outputs = model(images)     
            
            # Loss function
            loss = criterion(outputs, labels)
            
            # Backpropagation
            loss.backward()
            optimizer.step()

            running_loss += loss.item()
            _, predicted = torch.max(outputs, 1)
            correct += (predicted == labels).sum().item()
            total   += labels.size(0)

        train_loss = running_loss / len(train_loader)
        train_acc  = correct / total
        train_losses.append(train_loss)
        train_accuracies.append(train_acc)

        # Validation
        model.eval()
        correct, total, running_loss = 0, 0, 0.0

        with torch.no_grad():
            for images, labels in val_loader:
                images = images.view(images.size(0), -1)
                labels = labels.squeeze().long()
                outputs = model(images)
                loss = criterion(outputs, labels)
                running_loss += loss.item()
                _, predicted = torch.max(outputs, 1)
                correct += (predicted == labels).sum().item()
                total   += labels.size(0)

        val_loss = running_loss / len(val_loader)
        val_acc  = correct / total
        val_losses.append(val_loss)
        val_accuracies.append(val_acc)

        epoch_time = time.time() - epoch_start
        print(f"Epoch [{epoch+1:02d}/{NUM_EPOCHS}]  "
            f"Train Loss: {train_loss:.4f}  Train Acc: {train_acc:.4f}  "
            f"Val Loss: {val_loss:.4f}  Val Acc: {val_acc:.4f}  "
            f"Time: {epoch_time:.2f}s")

    total_time = time.time() - start_time
    print(f"\nTotal training time: {total_time:.2f}s")

    # Evaluate metrics
    model.eval()
    all_preds, all_labels = [], []

    with torch.no_grad():
        for images, labels in test_loader:
            images = images.view(images.size(0), -1)
            labels = labels.squeeze().long()
            outputs = model(images)
            _, predicted = torch.max(outputs, 1)
            all_preds.extend(predicted.numpy())
            all_labels.extend(labels.numpy())

    test_acc = accuracy_score(all_labels, all_preds)
    print(f"\nTest Accuracy: {test_acc:.4f}")
    print(classification_report(all_labels, all_preds, target_names=class_names))

# =====================================
# MAIN
# =====================================

def main():
    # 1. Data Exploration and Preprocessing
    train_dataset, val_dataset, test_dataset = preprocess()

    print("Training Dateset dimensions:\n")
    print(train_dataset)
    print("\n===================")
    print("Val Dateset dimensions:\n")
    print(val_dataset)
    print("\n===================")
    print("Test Dateset dimensions:\n")
    print(test_dataset)

    print("\n===================")
    save_montage(train_dataset, val_dataset, test_dataset)
    print("Saved montage images.\n")

    class_names = list(train_dataset.info['label'].values())
    num_channels = train_dataset.info['n_channels']

    train_counts = get_label_counts(train_dataset)
    val_counts = get_label_counts(val_dataset)
    test_counts = get_label_counts(test_dataset)

    print("\n===================")
    print("1. Train Dataset\n") 
    analyze_class_distribution(class_names, train_counts)

    print("\n===================")    
    print("2. Val Dataset\n")
    analyze_class_distribution(class_names, val_counts)

    print("\n===================")
    print("3. Test Dataset\n")
    analyze_class_distribution(class_names, test_counts)

    # 2. Logistic Regression
    print("\n===================")
    print("Performing Logistic Regression:\n")
    train_loader, val_loader, test_loader = log_reg(train_dataset, val_dataset, test_dataset)

    # 3. Neural Network
    print("\n===================")
    print("Building Neural Network:\n")
    neural_net(class_names, train_loader, val_loader, test_loader)

    return 0



if __name__ == "__main__":
    main()