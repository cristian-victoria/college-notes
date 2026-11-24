class Layer:
    def __init__(self):
        # Has two attributes, input and output
        self.input = None
        self.output = None
    
    def forward(self, input):
        # Takes in input and returns output
        pass

    def backward(self, output_fradient, learning_rate):
        # Takes in derivative of error with respect to output (output_gradient) and an additional parameter (learning_rate) which you can use to pass an optimizer
        # Responsible for: updating trainable parameters (if any) and returning the derivative of error with respect to the layer

        pass