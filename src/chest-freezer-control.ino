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

class SmartBox: public OneWire, public DallasTemperature, public RelayShield{
	
	double temperatureF; 
	enum temp_setpoint_t {FRESH, FROZEN, SHELF}; // Behavior of compartment
	
	public:
		SmartBox(temp_setpoint_t)
		: OneWire(D0), DallasTemperature (this)
		{
		    
		}
		
		
	protected:
		struct compartment_t {
			bool islocked;
			temp_setpoint_t temp_setpoint;
			double tempF;
			double tempC;
			double set_plus_tol = 3.0;
			double set_minus_tol = 3.0;
			const double error_plus = 130.0;
			const double error_minus = -30.0;
		}compartment_1;

};


