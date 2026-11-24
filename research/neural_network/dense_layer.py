from codebases.uni.research.neural_network.base_layer import Layer     # inherits from base layer (layer.py)
import numpy as np          # Uses numpy to compute dot product (seen in forward propagation)

class Dense(Layer):
    """
    Connects a set of input neurons (i) to a set of input neurons (j)
    Input = x, a matrix with shape i
    Output = y, a matrix with shape j

    Each input neuron is connected to every output neuron
        - Each connection represents a weight, w(ji)
        - Every output is the sum of all inputs * weights connecting them to that specific output:
            y(j) = x(1)w(j1) + x(2)w(j2) + ... + x(i)w(ji)+b(j)

    Biases = trainable parameters
    """
    def __init__(self, input_size, output_size):
        """
        Docstring for __init__
        
        :param input_size: number of neurons in input
        :param output_size: number of neurons in output

        weights = a matrix with shape j*i
        bias = a column vector with shape j
        """
        self.weights = np.random.randn(output_size, input_size) # Initialize weights randomly
        self.bias = np.random.randn(output_size, 1)   # Initialize biases randomly

    def forward(self, input):
        """
        Docstring for forward

        :param input: set of input neurons

        Returns Y = W * X + B
        """
        self.input = input # initialize input
        return np.dot(self.weights, self.input) + self.bias # Computes W * X + B using numpy's dot product function
    
    def backward(self, output_gradient, learning_rate):
        """
        Docstring for backward
        
        :param output_gradient: Derrivative of error with respect to the biases
        :param learning_rate: Optimizer

        Given the derivative of error with respect to the output of the dense layer:
            - Calculate the derivative of error with respect to the weights
            - Calculate the derivative of error with respect to the biases
            - Calculate the derivative of error with respect to the input of the dense layer

        Returns the derivative of error with respect to the input
        """
        weights_gradient = np.dot(output_gradient, self.input.T)
        input_gradient = np.dot(self.weights.T, output_gradient)
        self.weights -= learning_rate * weights_gradient
        self.bias -= learning_rate * output_gradient
        
        return input_gradient