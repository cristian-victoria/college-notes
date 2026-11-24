//Cristian Victoria
//COP 3330 Section 15
//Homework 2
//The class definition for Temperature

#include <iostream>
#include <iomanip>
#include <string>
#include "temperature.h"

using namespace std;

Temperature::Temperature()
{
	degrees = 0;
	scale = 'C';
	format = 'D';
}

Temperature::Temperature(double deg, char s)
{
	degrees = deg;
	scale = s;
	format = 'D';
	if((scale == 'C')||(scale == 'c')){
		if(degrees<-273.15){
			scale = 'C';
			degrees = 0;
		}
	}else if((scale == 'F')||(scale == 'f')){
                if(degrees<-459.67){
			scale = 'C';
                        degrees = 0;
                }
        }else if((scale == 'K')||(scale == 'k')){
                if(degrees<0){
			scale = 'C';
                        degrees = 0;
                }
        }else{
		scale = 'C';
                degrees = 0;
	}
}

void Temperature::Input()
{
	bool pass = false;
	do{
		cout<<"Please enter a temperature: "; 
        	cin>>degrees>>scale;
		if((scale == 'C')||(scale == 'c')){
                	if(degrees<-273.15){
                        	cout<<"Invalid temperaure. Please try again."<<endl;
			}else{
				pass = true;
			}	
		}else if((scale == 'F')||(scale == 'f')){
                	if(degrees<-459.67){
                        	cout<<"Invalid temperaure. Please try again."<<endl;
			}else{
                                pass = true;
                        }
		}else if((scale == 'K')||(scale == 'k')){
                	if(degrees<0){
                        	cout<<"Invalid temperaure. Please try again."<<endl;
			}else{
                                pass = true;
                        }
		}
	}while(pass==false);
}

void Temperature::Show() const
{
	string displayFullWordScale;
        if(format=='D'){
                cout<<degrees<<" "<<scale<<endl;
        }else if(format=='P'){
                cout<<fixed<<setprecision(1)<<degrees<<" "<<scale<<endl;
        }else if(format=='L'){
                if(scale=='C'||scale=='c'){
                        displayFullWordScale = "Celcius";
                }else if(scale=='F'||scale=='f'){
                        displayFullWordScale = "Farenheit";
                }else if(scale=='K'||scale=='k'){
                        displayFullWordScale = "Kelvin";
                }
                cout<<degrees<<" "<<displayFullWordScale;
        }
}

bool Temperature::Set(double deg, char s)
{
	bool valid = true;
	double tempDegrees = deg;
	char tempScale = s;
	if((tempScale == 'C')||(tempScale == 'c')){
        	if(tempDegrees<-273.15){
			valid = false;
                }else{
                        valid = true;
			degrees = tempDegrees;
                        scale = tempScale;
                }
	}else if((tempScale == 'F')||(tempScale == 'f')){
		if(tempDegrees<-459.67){
                        valid = false;
                }else{
                        valid = true;
			degrees = tempDegrees;
                        scale = tempScale;
                }	
	}else if((tempScale == 'K')||(tempScale == 'k')){
		if(tempDegrees<0){
                        valid = false;
                }else{
                        valid = true;
			degrees = tempDegrees;
			scale = tempScale;
                }
	}

	return valid;
}

double Temperature::GetDegrees() const
{
	return degrees;
}

char Temperature::GetScale() const
{
	return scale;
}

bool Temperature::SetFormat(char f)
{
	bool valid = false;
	char tempFormat = f;
	if(tempFormat=='D'){
		format = tempFormat;
		valid = true;
	}else if(tempFormat=='P'){
                format = tempFormat;
                valid = true;
	}else if(tempFormat=='L'){
                format = tempFormat;
                valid = true;
	}else{
		format = 'D';
	}

	return valid;
}

bool Temperature::Convert(char sc)
{
	char tempScale = sc;
	bool valid = false;
	if((tempScale == 'C')||(tempScale == 'c')){
		valid = true;
                if((scale == 'F')||(scale == 'f')){
                        scale = tempScale;
			degrees = (degrees-32)*(5/9);
                }else if((scale == 'K')||(scale == 'k')){
                        scale = tempScale;
                        degrees = degrees-273.15;
		}
	}else if((tempScale == 'F')||(tempScale == 'f')){
                valid = true;
                if((scale == 'C')||(scale == 'C')){
                        scale = tempScale;
                        degrees = (degrees/(5/9))+32;
                }else if((scale == 'K')||(scale == 'k')){
                        scale = tempScale;
                        degrees = ((degrees-273.15)/(5/9))+32;    
                }
	}else if((tempScale == 'K')||(tempScale == 'k')){
                valid = true;
                if((scale == 'C')||(scale == 'C')){
                        scale = tempScale;
                        degrees = degrees+273.15;
                }else if((scale == 'F')||(scale == 'f')){
                        scale = tempScale;
                        degrees = ((degrees-32)*(5/9))+273.15;    
                }
	}
	
	return valid;
}

int Temperature::Compare(const Temperature& d) const
{
	double parameterDegrees = d.degrees;
	char parameterScale = d.scale;
	if((scale == 'C')||(scale == 'c')){
                if((parameterScale == 'F')||(parameterScale == 'f')){
                        parameterDegrees = (parameterDegrees-32)*(5/9);
                }else if((scale == 'K')||(scale == 'k')){
                        parameterDegrees = parameterDegrees-273.15;
                }
	}else if((scale == 'F')||(scale == 'f')){
                if((parameterScale == 'C')||(parameterScale == 'C')){
                        parameterDegrees = (parameterDegrees/(5/9))+32;
                }else if((scale == 'K')||(scale == 'k')){
                        parameterDegrees = ((parameterDegrees-273.15)/(5/9))+32;
                }
	}else if((scale == 'K')||(scale == 'k')){
                if((parameterScale == 'C')||(parameterScale == 'C')){
                        parameterDegrees = parameterDegrees+273.15;
                }else if((scale == 'F')||(scale == 'f')){
                       	parameterDegrees = ((parameterDegrees-32)*(5/9))+273.15;
                }
	}
	if(degrees<parameterDegrees){
		return	-1;
	}else if(degrees==parameterDegrees){
		return 0;
	}else if(degrees>parameterDegrees){
		return 1;
	}
}
