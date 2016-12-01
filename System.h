#include <Arduino.h> //It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h" 

class System
{

  public:

    struct Globals {
      bool write_state_to_serial  = false;
      bool write_state_to_SD_card = false;

      bool use_receiver           = false;
      bool use_SD_card            = false; // ********
      bool use_AIRSpeed           = false;
      bool use_GPS                = false;
      bool use_MPU                = true;
      bool use_BMP180             = false;
      bool use_BMP280             = true;
      bool use_TM1637Display      = true; // *********

      bool use_stabilizer         = true; // Autopilot mode0
      
      char state_str[256] = ""; // serialization of the state object

      bool debug_led_mode;
      int displayContent = 0; // [0=GPS, 1=temp_BMP280, 2= MPU, 3=temp_BMP180, 4=pressure_BMP280]
      int numContentsToDisplay = 5; // number of different items to display
    };

    struct Consts {
      //Pins
      const int SD_reader_chipSelect 	= 42; //   SD card chip select wire
      const int debug_led_pin 		    = 13; //   Debug led
      const int current_sensor_pin    = A0; //   analogue pin
      const int airspeed_sensor_pin   = A1; //   analogue pin
      const int TM1637_CLK            = 40; // |
      const int TM1637_DIO            = 38; // | <--- Quadruple 7-Seg display's data wires

      const int throttle_pin   = 2;
      const int ailerons_pin   = 3;
      const int elevator_pin   = 4;
      const int rudder_pin     = 5;

      // Repeated tasks rate [i.e., read sensor every X loops]
      const int minPeriod         = 15;
      const int readAirspeedEvery = minPeriod;
      const int readPressureEvery = minPeriod;
      const int readGPSEvery      = 50;
      const int readMPUEvery      = minPeriod;
      const int readCurrentEvery  = 100;
      const int writeEvery        = minPeriod;
      const int stabilizeProcEvery = minPeriod;
      const int toggleDisplayEvery = 1000; //100; // flip what's being displayed every x loops

      // Persistency
      String filename = "tele_";

    };

    // holds the system state
    struct State {
      //Sensor data ----------------------------------------------------------------------------------------------------------------------------------------------
      int16_t ax, ay, az; // Accelerometer vals                       |
      int16_t gx, gy, gz; // Gyroscope vals                           | <---- MPU9255
      int16_t mx, my, mz; // Magnetometer vals                        |

      float temperature_BMP180; // the measurred ambient temperature  |
      float pressure_BMP180;    // the meassured pressure             |
      // the barometric altitude (meters)above baseline, given a      | <---- BMP180
      // pressure measurement P (mb) and the pressure at a baseline P0|
      float barometric_altitude_BMP180; //                            |
      double p0_BMP180;  // baseline pressure                         |

      float temperature_BMP280; // the measurred ambient temperature  |
      float pressure_BMP280;    // the meassured pressure             |
      // the barometric altitude (meters)above baseline, given a      | <---- BMP280
      // pressure measurement P (mb) and the pressure at a baseline P0|
      float barometric_altitude_BMP280;  //                           |
      double p0_BMP280;  // baseline pressure                         |

      unsigned short satellites;      // number of satellites         |
      float latitude,                 // latitude                     |
            longitude,                // longtitude                   |
            gps_altitude,             // Altitude (m)                 |
            course,                   // Course                       | <---- GPS
            ground_speed;             // Ground Speed (GPS based)     |
      unsigned long gps_fix_age;      // age of the GPS fix           |
      unsigned long gps_hdop;         // Accuracy level               |
      String date;                    // Time and date                |

      unsigned int airspeed_raw_reading;  // the raw analog value     | <---- AirSpeed
      float        airspeed;              //the calculated airspeed   |
      //Sensors status
      uint8_t status_SD;      // (SD card)                  0 = not connected, 1= connected, 99= general error
      uint8_t status_GPS;     // (GPS)                      0 = not connected or no fix, 1= connected and have fix, 99=error
      uint8_t status_BMP180;  // (Air Pressure/temperature) 0 = not connected, 1= connected and functioning, 99=error
      uint8_t status_BMP280;  // (Air Pressure/temperature) 0 = not connected, 1= connected and functioning, 99=error
      uint8_t status_MPU;     // (Accelometer, Gyro, Mag)   0 = not connected  1= connected and functioning, 99=error
      uint8_t status_CUR;     // (Current Meter)            0 = not connected, 1= connected and functioning, 99=error
      uint8_t status_AIRSPEED;// (Current Meter)            0 = not connected, 1= connected and functioning, 99=error
      uint8_t status_DLE;     // (Tachometer)               0 = not connected, 1= connected and functioning, 99=error
    };

};
