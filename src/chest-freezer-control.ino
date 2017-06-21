// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.
#include <RelayShield.h>


// typedef enum temp_mode_t {FRESH, FROZEN, SHELF}temp_mode_t; // Behavior of compartment
// typedef static enum compressor_state_t {STARTUP, START_COOLING, COOLING, END_COOLING, OFF} compressor_state_t;

// temp_mode_t test = FRESH;



class SmartBox: public OneWire, public DallasTemperature, public RelayShield
{	
	public:
		// ***Class variables***
		// compressure_state is shared between all instances of class, 
		// must initialize outside of class instance
		typedef enum compressor_state_t {STARTUP, START_COOLING, COOLING, END_COOLING, OFF}compressor_state_t;
		static compressor_state_t compressor_state;

		const double  error_plus = 130; // Maximum temp output, anything higher is considered an erroneous reading
		const double  error_minus = -30; // Minimum temp output, anything lower is considered an erroneous reading
		
		// ***Instance variables***
		bool islocked; // Locked state of compartment
		double target_temp; // Target temperature of compartment
		double tempF; // Current temperature of compartment
		double set_plus_tol = 3.0; // Default tolerance of compartment
		double set_minus_tol = 3.0; // Default tolerance of compartment
		typedef enum temp_mode_t {FRESH, FROZEN, SHELF}temp_mode_t; // Behavior of compartment
		
		// ***Member Functions***
		SmartBox(temp_mode_t mode) // Constructor
		: OneWire(D0), DallasTemperature (this) 
		{
			SetMode(mode); // Set the target temp and min/max temperature for compartment
// 			compressor_state = STARTUP; // Start compressor in the off state
		}
		void SetMode(temp_mode_t);
		// void UpdateTemp();

		// void Update();
		
	protected:
		// min off time of 5 minutes to allow pressure equalizatoin
		const unsigned long min_off = 300000;
		// min on time of 1 minutes for cooling stabilization
		const unsigned long min_on = 60000;
};

// Set the operating mode of the compartment e.g. fresh, frozen, or shelf stable foods
void SmartBox::SetMode(temp_mode_t mode)
{
	switch (mode)
	{
		case FRESH:
			target_temp = 37;
			set_plus_tol = 3;
			set_minus_tol = 3;
		break;
		
		case FROZEN:
			target_temp = 0;
			set_plus_tol = 3;
			set_minus_tol = 3;
		break;
		
		case SHELF:
			target_temp = 70;
			set_plus_tol = 20;
			set_minus_tol = 25;
		break;
	}
}


// LAST ADDED CODE 6/20
SmartBox compartment_1(SmartBox::FRESH);
SmartBox::compressor_state_t SmartBox::compressor_state = STARTUP; // Initialize compressor state


void setup() {

}

void loop() {
    

}