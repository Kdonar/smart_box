
// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include <OneWire.h>

// This #include statement was automatically added by the Particle IDE.
#include <RelayShield.h>


// -----------------
// Read temperature
// -----------------

// // Data wire is plugged into port 0 on the Photon
// // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
// OneWire oneWire(D0);

// // Pass our oneWire reference to Dallas Temperature.
// DallasTemperature dallas(&oneWire);

//Create an instance of the RelayShield library, so we have something to talk to
RelayShield myRelays;


// Create a variable that will store the temperature value
double temperature = 0.0;
double temperatureF = 0.0;
unsigned long SmartBox::ts_cool_start = 0;
unsigned long SmartBox::ts_cool_end = 0;

class SmartBox;

void setup() {

  // Register a Particle variable here
  Particle.variable("temperature", &temperature, DOUBLE);
  Particle.variable("temperatureF", &temperatureF, DOUBLE);

  // setup the temperature sensor library
  dallas.begin();
  
  //.begin() sets up the relay library
  myRelays.begin();

  // Set relay to off position by default
  myRelays.off(1); 
  
}

void loop() {
    

}


class SmartBox{
	
	double temperatureF; 
	enum temp_setpoint_t {FRESH, FROZEN, SHELF}; // Behavior of compartment
	
	public:
		SmartBox(int, temp_setpoint_t);
		double getTemp(); // Get current temperature
		void setState(state_t state); // Change setpoint of compartment
		void switchLockState(); // Change position of lock
		void update(); // Update relays
		
	protected:
		struct compartment_t {
			bool islocked;
			temp_setpoint_t compartment_setpoint;
			double measured_temp;
			double set_plus_tol = 3.0;
			double set_minus_tol = 3.0;
			const double error_plus = 130.0;
			const double error_minus = -30.0;
		}compartment_1;
			
		// Compressor operating mode
		enum compressor_operation_t {S_OFF, S_STARTUP, S_START_COOLING, S_COOLING, S_END_COOLING}compressor_operation;
		
		// min off time of 5 minutes to allow pressure equalization
		const unsigned long  _min_off = 300000;
		
		// min on time of 1 minutes for cooling stabilization
		const unsigned long _min_on = 60000;
		
		// Default start with the unit in the off state //
		int state = S_STARTUP;
		// Store the current compressor on and off times 
		static unsigned long ts_cool_start;
		static unsigned long ts_cool_end;
};

SmartBox::SmartBox(int temperature_pin, temp_setpoint_t temp_setpoint)
{
	this.setState(temp_setpoint); // set target temps and tolerances
	
	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire oneWire(temperature_pin);

	// Pass our oneWire reference to Dallas Temperature.
	DallasTemperature dallas(&oneWire);
}

double SmartBox::getTemp() {

    // Request temperature conversion
    dallas.requestTemperatures();
            
    // get the temperature in Celcius
    float tempC = dallas.getTempCByIndex(0);
            
    // convert to double
    temperature = (double)tempC;
            
    // convert to Fahrenheit
    float tempF = DallasTemperature::toFahrenheit( tempC );
            
    // convert to double
    temperatureF = (double)tempF; 
}

// Set the temperature and tolerances of the compartment by telling it which state to hold
void SmartBox::setState(state_t state)
{
	compartment_state = state;
	
	switch (compartment_state)
	{
		case FRESH:
			set_point = 37;
			set_plus_tol = 3;
			set_minus_tol = 3;
		break;
		
		case FROZEN:
			set_point = 0;
			set_plus_tol = 3;
			set_minus_tol = 3;
		break;
		
		case SHELF:
			set_point = 70;
			set_plus_tol = 20;
			set_minus_tol = 25;
		break;
	}
}

void SmartBox::switchLockState (){
	// Insert code to lock compartment
}

void SmartBox::Update(){
	// Code to turn compressor on/off
    switch (state)
    {
        case S_STARTUP:
        
            temperatureF = gettemp();
            
            if (temperatureF < error_minus || temperatureF > error_plus)
            
            {
                // Do confirmation set on relay to make sure cooling is off
                myRelays.off(1);
                
                // Since there was an error measuring temperature, wait 5 seconds and try startup again
                delay (5000);
              
            }
            
            else if (temperatureF > (set_point + set_plus_tol))
                
            {
                state = S_START_COOLING;
            }
                
            else
                
            {
                // Do confirmation set on relay to make sure cooling is off
                myRelays.off(1);
                
                // Since the actual cooling ending time is unknown, set it at this point
                ts_cool_end = millis();
                state = S_OFF;
                delay (5000);                   
            }
                
            break;

        case S_OFF:
        
            temperatureF = gettemp();
            
            if (temperatureF < error_minus || temperatureF > error_plus)
            
            {
                
                // Do confirmation set on relay to make sure cooling is off
                myRelays.off(1);
                delay (5000);

            }
        
            else if ( ((millis() - ts_cool_end) > min_off) && (temperatureF > (set_point + set_plus_tol)))
            
            {
                state = S_START_COOLING;
            }
            
            else
            
            {
                // Do confirmation set on relay to make sure cooling is off
                myRelays.off(1); 
                delay (5000);
            }
            
            break;

        case S_START_COOLING:
        
            // Record the time cooling has started
            ts_cool_start = millis();
            
            // Set the relay to turn the cooling on
            myRelays.on(1);
            
            state = S_COOLING;
            
            break;
        
        case S_COOLING:
        
            temperatureF = gettemp();
            
            if (temperatureF < error_minus || temperatureF > error_plus)
            
            {
                
                // Do confirmation set on relay to make sure cooling is on
                myRelays.on(1);
                delay (5000);

            }
        
            else if ( ((millis() - ts_cool_start) > min_on) && (temperatureF < (set_point - set_minus_tol)))
            
            {
                state = S_END_COOLING;
            }
            
            else
            
            {
                // Do confirmation set on relay to make sure cooling is on
                myRelays.on(1);
                delay (5000);
            }
            
            break;        
            
        case S_END_COOLING:
        
            // Record the time cooling has stopped
            ts_cool_end = millis();
            
            // Set the relay to turn the cooling off
            myRelays.off(1);
            
            state = S_OFF;
            
            break;
    }
}