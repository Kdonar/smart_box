// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.
#include <RelayShield.h>


class SmartBox: public OneWire, public DallasTemperature, public RelayShield
{	
	// min off time of 5 minutes to allow pressure equalizatoin
	const unsigned long min_off = 300000;
	// min on time of 1 minutes for cooling stabilization
	const unsigned long min_on = 60000;
	
	public:
		// ***Class variables***
		// compressure_state is shared between all instances of class, 
		// must initialize outside of class instance
		typedef enum compressor_state_t {STARTUP, START_COOLING, COOLING, END_COOLING, OFF}compressor_state_t;
		static compressor_state_t compressor_state;
		static unsigned long ts_cool_start;
		static unsigned long ts_cool_end;

		const double  error_plus = 130; // Maximum temp output, anything higher is considered an erroneous reading
		const double  error_minus = -30; // Minimum temp output, anything lower is considered an erroneous reading
		
		// ***Instance variables***
		bool islocked; // Locked state of compartment
		double target_temp; // Target temperature of compartment
		double temperatureF; // Current temperature of compartment
		double set_plus_tol = 3.0; // Default tolerance of compartment
		double set_minus_tol = 3.0; // Default tolerance of compartment
		typedef enum temp_mode_t {FRESH, FROZEN, SHELF}temp_mode_t; // Behavior of compartment
		
		// ***Member Functions***
		SmartBox(temp_mode_t mode): OneWire(D0), DallasTemperature (this) 
		{
			SetMode(mode); // Set the target temp and min/max temperature for compartment
		}
		
		void SetMode(temp_mode_t); // Change set temperature of the compartment
		void GetTemp(); // Update temperatureF
		void Update();
		
		protected:
		bool ErrorCheck();
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

// Get temperature for this instance (Compartment)
void SmartBox::GetTemp()
{
    // Request temperature conversion
    this->requestTemperatures();
            
    // get the temperature in Celcius
    float tempC = this->getTempCByIndex(0);
            
    // convert to Fahrenheit
    float tempF = DallasTemperature::toFahrenheit( tempC );
            
    // convert to double
    temperatureF = (double)tempF;	
}

// Check for erroneous temperature reading
bool SmartBox::ErrorCheck(){
	return (temperatureF < error_minus || temperatureF > error_plus);
}

void SmartBox::Update()
{
	GetTemp();
	
	switch(compressor_state)
	{
		case STARTUP:
			
			if(ErrorCheck())
			{
				// Do confirmation set on relay to make sure cooling is off
				this->off(1);
				// Since there was an error measuring temperature, wait 5 seconds and try startup again
				delay(500);
			}
			else if ( temperatureF > (target_temp + set_plus_tol)) compressor_state = START_COOLING;
			else 
			{
				// Do confirmation set on relay to make sure cooling is off
				this->off(1);
				
				// Since the actual cooling ending time is unknown, set it at this point
				ts_cool_end = millis();
				compressor_state = OFF;
				delay(500);
			}
			break;
		
		case OFF:
			if (ErrorCheck())
			{
				this->off(1);
				delay(5000);
			}
			
			else if ( ((millis() - ts_cool_end) > min_off) && (temperatureF > (target_temp + set_plus_tol)))
			{
				compressor_state = START_COOLING;
			}
			
			else
			{
				this->off(1);
				delay(500);
			}
			break;
			
		case START_COOLING:
			
			// Record the time cooling has started
			ts_cool_start = millis();
			
			// Set the relay to turn on cooling
			this->on(1);
			compressor_state = COOLING;
			
			break;
		
		case COOLING:
		
			if(ErrorCheck())
			{
				this->on(1);
				delay(5000);
			}
			
			else if( ((millis() - ts_cool_start) > min_on) && (temperatureF < (target_temp - set_minus_tol)))
			{
				compressor_state = END_COOLING;
			}
			
			else
			{
				// Do a confirmation set to make sure cooling is on
				this->on(1);
				delay(5000);
			}
			break;
			
		case END_COOLING:
		
			// Record the time cooling has stopped
			ts_cool_end = millis();
			
			// Set the relay to turn the cooling off
			this->off(1);
			compressor_state = OFF;
			
			break;	
	}
}
//***object setup***
SmartBox compartment_1(SmartBox::FRESH);
SmartBox::compressor_state_t SmartBox::compressor_state = STARTUP; // Initialize compressor state
unsigned long SmartBox::ts_cool_start = millis();
unsigned long SmartBox::ts_cool_end = millis();

void setup() {

}

//***Main program***
void loop() {
    compartment_1.Update();
}