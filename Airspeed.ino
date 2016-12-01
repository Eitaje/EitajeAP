
#include "Pressure.h"

#define PRESSURE_PIN A1
# define historyLen 45 // speed is averaged over the lasr [historyLen] readings

/*
 * Attach the analog pin to the MPXV7002DP chip
 */
Pressure MPXV7002DP(PRESSURE_PIN);


float speedHistory[historyLen];

/*
 * Setup
 */
void setupAirSpeedSensor(System::State* state) {
  int val = MPXV7002DP.Init();
  //is this sensor connected and working?
  state->status_AIRSPEED = 1;
}

/*
 * This method compute the airspeed in KMH
 */
void get_airspeed_data(System::State* state, System::Globals* globals) {
  float real_temp = 0;
  int air_pressure =0;
  int measures = 0;

  //Get the current temperature
  if (globals->use_BMP180 && state->status_BMP180 == 1)
  {
    real_temp += state->temperature_BMP180;
    air_pressure += state->pressure_BMP180;
    measures++;
  }
  if (globals->use_BMP280 && state->status_BMP280 == 1)
  {
    real_temp += state->temperature_BMP280;
    air_pressure += state->pressure_BMP280;
    measures++;
  }
  real_temp /= measures;
  air_pressure /= measures;

  //Get the airspeed reading, and update readings history
  int val = MPXV7002DP.GetAirSpeed(real_temp);
  for (int i = historyLen - 1 ; i > 0 ; i--)
    speedHistory[i] = speedHistory[i - 1];

  speedHistory[0] = val;

  //avarage airspeed over recent history
  float airspeed = 0.0f;
  for (int i = 0 ; i < historyLen; i++)
    airspeed += speedHistory[i];

  // update the state
  state -> airspeed_raw_reading  = val;
  state -> airspeed = (airspeed / historyLen);

  //  Serial.print(val2);Serial.print(",");
  //    SerialUSB.println(airspeed/historyLen);
}
