#include <Arduino.h> //It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h" 

class Sensors
{

  public:

    struct MPU9250_Data {

      int16_t ax, ay, az; // Accelerometer vals
      int16_t gx, gy, gz; // Gyroscope vals
      int16_t mx, my, mz; // Magnetometer vals
      uint8_t status; // 0= not connected, 1= connected and functioning, 99=error
    };

    struct BMP_Data {
      float temperature; // the measurred ambient temperature
      float pressure;      // the meassured pressure
      double altitude;     // the altitude (meters) above baseline, given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
      double p0;           // baseline pressure
      uint8_t status; // 0= not connected, 1= connected and functioning, 99=error
    };

    struct GPS_Data {
      unsigned short satellites;     // number of satellites
      float latitude,     // latitude
            longitude,    // longtitude
            altitude,     // Altitude (m)
            course,       // Course
            speed;        // Speed
      unsigned long fix_age;       // age of the GPS fix
      unsigned long hdop; // Accuracy level 
      String date;  // Time and date
      uint8_t status; // 0= not connected or no fix, 1= connected and have fix, 99=error

    };

};
