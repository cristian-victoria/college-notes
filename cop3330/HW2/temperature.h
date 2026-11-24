//Cristian Victoria
//COP 3330 Section 15
//Homework 2
//Header file for temperature.h

class Temperature
{
public:
        Temperature();      
        Temperature(double deg, char s);

        void Input();
        void Show() const;
        double GetDegrees() const;  
        char GetScale() const;
	bool Set(double deg, char s);
        bool SetFormat(char f);
        bool Convert(char sc);
        int Compare(const Temperature& d) const;	

private:
        double degrees;
	char scale;
	char format;
};
