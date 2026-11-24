import numpy as np
import keras
from keras.datasets import mnist
import tensorflow as tf

from codebases.uni.research.neural_network.dense_layer import Dense
from codebases.uni.research.neural_network.convolutional_nn import Convolutional
from codebases.uni.research.neural_network.reshape import Reshape
from codebases.uni.research.neural_network.activation_functions import Sigmoid
from codebases.uni.research.neural_network.loss_functions import binary_cross_entropy, binary_cross_entropy_prime
from codebases.uni.research.neural_network.network import train, predict

def preprocess_data(x, y, limit):
    zero_index = np.where(y == 0)[0][:limit]
    one_index = np.where(y == 1)[0][:limit]
    all_indices = np.hstack((zero_index, one_index))
    all_indices = np.random.permutation(all_indices)
    x, y = x[all_indices], y[all_indices]
    x = x.reshape(len(x), 1, 28, 28)
    x = x.astype("float32") / 255
    y = tf.keras.utils.to_categorical(y)
    y = y.reshape(len(y), 2, 1)
    return x, y

# load MNIST from server, limit to 100 images per class since we're not training on GPU
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, y_train = preprocess_data(x_train, y_train, 100)
x_test, y_test = preprocess_data(x_test, y_test, 100)

# neural network
network = [
    Convolutional((1, 28, 28), 3, 5),
    Sigmoid(),
    Reshape((5, 26, 26), (5 * 26 * 26, 1)),
    Dense(5 * 26 * 26, 100),
    Sigmoid(),
    Dense(100, 2),
    Sigmoid()
]

# train
train(
    network,
    binary_cross_entropy,
    binary_cross_entropy_prime,
    x_train,
    y_train,
    epochs=20,
    learning_rate=0.1
)

# test
for x, y in zip(x_test, y_test):
    output = predict(network, x)
    print(f"pred: {np.argmax(output)}, true: {np.argmax(y)}")

"""
FC NN: ~0.056 error after 100 epochs

CNN: ~0.0018 error after 20 epochs

CNN is binary, not 10-class MNIST

MNIST 0 vs 1 classification, not digits 0 to 9.

That explains:
- Extremely low error
- Almost perfect predictions
- Why convergence is so fast
"""