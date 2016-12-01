#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

//#define P0 1013.25
Adafruit_BMP280 bmp; // I2C
//Adafruit_BMP280 bme(BMP_CS); // hardware SPI
//Adafruit_BMP280 bme(BMP_CS, BMP_MOSI, BMP_MISO,  BMP_SCK);

double P0 = 0.0;
double T0;

void setupBMP280(System::State* state)
{
  //is this sensor connected and working?
  state->status_BMP280 = 0;

  if (!bmp.begin()) {
    //Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  }
  else
  {
    state->status_BMP280 = 1; // this sensor is working properly
  }

  delay(100);

  float T, P;
  T0 = bmp.readTemperature();
  P0 = bmp.readPressure()/100;
  state->p0_BMP280 = P0;
  //DEBUG -------------------------------------|
  //  Serial.print("BMP280 P0 = \t"); Serial.println(P0);
  //  Serial.print("BMP280 T0 = \t"); Serial.println(T0);
  //DEBUG -------------------------------------|
}

void get_BMP280_data(System::State* state)
{
  state->temperature_BMP280          = bmp.readTemperature();
  state->pressure_BMP280             = bmp.readPressure()/100;
  state->barometric_altitude_BMP280  = bmp.readAltitude(P0);
}

