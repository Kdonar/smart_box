// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.
#include <RelayShield.h>



class SmartBox;
// SmartBox::SmartBox(temp_setpoint_t setpoint);

void setup() {


  
}

void loop() {
    

}

class SmartBox: public OneWire, public DallasTemperature, public RelayShield
{	
	public:
		enum temp_mode_t {FRESH, FROZEN, SHELF}; // Behavior of compartment
		double temperatureF; // Last measured value of compartment
		
		SmartBox(temp_mode_t mode)
		: OneWire(D0), DallasTemperature (this) // Constructor
		{
			this->SetMode(mode);
		}
		void SetMode(temp_mode_t);
		double GetTemp();
		
	protected:
		struct compartment_t {
			bool islocked;
            double target_temp;
			double tempF;
			double tempC;
			double set_plus_tol = 3.0;
			double set_minus_tol = 3.0;
			const double error_plus = 130.0;
			const double error_minus = -30.0;
		}compartment_1;
};

// Set the operating mode of the compartment e.g. fresh, frozen, or shelf stable foods
void SmartBox::SetMode(temp_mode_t mode)
{
	switch (mode)
	{
		case FRESH:
			compartment_1.target_temp = 37;
			compartment_1.set_plus_tol = 3;
			compartment_1.set_minus_tol = 3;
		break;
		
		case FROZEN:
			compartment_1.target_temp = 0;
			compartment_1.set_plus_tol = 3;
			compartment_1.set_minus_tol = 3;
		break;
		
		case SHELF:
			compartment_1.target_temp = 70;
			compartment_1.set_plus_tol = 20;
			compartment_1.set_minus_tol = 25;
		break;
	}
}

double SmartBox::GetTemp() {

    // Request temperature conversion
    this->requestTemperatures();
            
    // get the temperature in Celcius
    float tempC = this->getTempCByIndex(0);
            
    // convert to double
    double temperatureC = (double)tempC;
            
    // convert to Fahrenheit
    float tempF = DallasTemperature::toFahrenheit( tempC );
            
    // convert to double
    temperatureF = (double)tempF; 
}