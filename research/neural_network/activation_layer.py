from codebases.uni.research.neural_network.base_layer import Layer
import numpy as np

class Activation(Layer):
    """
    Docstring for Activation

    Takes input neurones (i) and simply passes them into an activation function
    The output has the same shape as the input
    """
    def __init__(self, activation, activation_prime):
        """
        Docstring for __init__
        
        :param activation: activation function
        :param activation_prime: derivative of activation function
        """
        self.activation = activation
        self.activation_prime = activation_prime

    def forward(self, input):
        """
        Apply the activation function to every element of x
        """
        self.input = input
        return self.activation(self.input)
    
    def backward(self, output_gradient, learning_rate):
        """
        Docstring for backward
        
        :param output_gradient: Description
        :param learning_rate: Description

        Update parameters and return derivative of the error with respect to the input
        """
        return np.multiply(output_gradient, self.activation_prime(self.input))