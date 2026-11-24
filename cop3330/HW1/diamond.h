//Cristian Victoria
//COP 3330 Section 15
//Homework 1 
//Header file for diamond.cpp

class Diamond
{
public:
	Diamond();	//Initializes diamond
	Diamond(int s, char b='#', char f='*');		//Constructor with parameters

	//Accessors
	int GetSize() const;
	int Perimeter() const;
	double Area() const;

	//Mutators
	void Grow();
	void Shrink();
	void SetBorder(char b);
	void SetFill(char f);
		
	//Display output
	void Draw() const;
	void Summary() const;

private:
	int size;	//Side length of diamond
	char border;	//Border character
	char fill;	//Fill character
};
