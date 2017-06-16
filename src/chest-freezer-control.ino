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
	
	
	public:
	enum temp_mode_t {FRESH, FROZEN, SHELF}; // Behavior of compartment
	double temperatureF; 
		SmartBox(temp_mode_t mode)
		: OneWire(D0), DallasTemperature (this)
		{
			this->SetMode(mode);
		}
		void SetMode(temp_mode_t);
		
	protected:
		struct compartment_t {
			bool islocked;
// 			enum temp_mode_t {FRESH, FROZEN, SHELF} target_temp;
            temp_mode_t target_temp;
			double tempF;
			double tempC;
			double set_plus_tol = 3.0;
			double set_minus_tol = 3.0;
			const double error_plus = 130.0;
			const double error_minus = -30.0;
		}compartment_1;

};

void SmartBox::SetMode(temp_mode_t mode)
{
	
}
