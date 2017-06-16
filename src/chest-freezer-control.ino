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
		
		enum temp_mode_t {FRESH, FROZEN, SHELF}; // Behavior of compartment
		enum compressor_state_t {STARTUP, START_COOLING, COOLING, END_COOLING, OFF} compressor_state;
		double temperatureF; // Last measured value of compartment
		
		SmartBox(temp_mode_t mode) // Constructor
		: OneWire(D0), DallasTemperature (this) 
		{
			this->SetMode(mode, compartment_1); // Set the target temp and min/max temperature for compartment
			compressor_state = STARTUP; // Start compressor in the off state
		}
		void SetMode(temp_mode_t, compartment_t);
		void GetTemp(compartment_t);
		// void Update();
};

// Set the operating mode of the compartment e.g. fresh, frozen, or shelf stable foods
void SmartBox::SetMode(temp_mode_t mode, compartment_t compartment)
{
	switch (mode)
	{
		case FRESH:
			compartment.target_temp = 37;
			compartment.set_plus_tol = 3;
			compartment.set_minus_tol = 3;
		break;
		
		case FROZEN:
			compartment.target_temp = 0;
			compartment.set_plus_tol = 3;
			compartment.set_minus_tol = 3;
		break;
		
		case SHELF:
			compartment.target_temp = 70;
			compartment.set_plus_tol = 20;
			compartment.set_minus_tol = 25;
		break;
	}
}

void SmartBox::GetTemp(compartment_t compartment) {

    // Request temperature conversion
    this->requestTemperatures();
            
    // get the temperature in Celcius
    compartment.tempC = this->getTempCByIndex(0);
            
    // convert to double
    // double temperatureC = (double)tempC;
            
    // convert to Fahrenheit
    compartment.tempF = DallasTemperature::toFahrenheit( compartment.tempC );
            
 
}

