//Translate the 9-axis data into String
void serializeState(System::State* state, System::Globals* globals)
{
  String ser_str = "";
  if (globals->use_AIRSpeed && state->status_AIRSPEED == 1)
  {
    ser_str += state-> airspeed;
  }

  if (globals->use_MPU && state->status_MPU == 1) {
    if (ser_str.length() > 0)
      ser_str += ", ";
    ser_str += state->ax ;
    ser_str += ", ";
    ser_str += state->ay;
    ser_str += ", ";
    ser_str += state->az;
    ser_str += ", ";
    ser_str += state->gx;
    ser_str += ", ";
    ser_str += state->gy;
    ser_str += ", ";
    ser_str += state->gz;
    ser_str += ", ";
    ser_str += state->mx;
    ser_str += ", ";
    ser_str += state->my;
    ser_str += ", ";
    ser_str += state->mz;
  }

  char altitude[16], pressure[16], p0[16];
  if (globals->use_BMP180 && state->status_BMP180 == 1) {
    if (ser_str.length() > 0)
      ser_str += ", ";

    floatToString(pressure, state->pressure_BMP180, 4, 6);
    floatToString(p0, state->p0_BMP180, 4, 6);
    floatToString(altitude, state->barometric_altitude_BMP180, 4, 6);
    String temperature = floatToStr(state->temperature_BMP180, -1, 4, 2);

    ser_str += temperature; // the measurred ambient temperature
    ser_str += ", ";
    ser_str += pressure;    // the meassured pressure
    ser_str += ", ";
    ser_str += altitude;    // the altitude (meters) above baseline, given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
    //  state_str += ", ";
    //  state_str += p0_BMP180;          //baseline pressure
  }

  if (globals->use_BMP280 && state->status_BMP280 == 1) {
    if (ser_str.length() > 0)
      ser_str += ", ";
    floatToString(pressure, state->pressure_BMP280, 4, 6);
    floatToString(p0, state->p0_BMP280, 4, 6);
    floatToString(altitude, state->barometric_altitude_BMP280, 4, 6);
    String temperature = floatToStr(state->temperature_BMP280, -1, 4, 2);

    ser_str += temperature; // the measurred ambient temperature
    ser_str += ", ";
    ser_str += pressure;    // the meassured pressure
    ser_str += ", ";
    ser_str += altitude;    // the altitude (meters) above baseline, given a pressure measurement P (mb) and the pressure at a baseline P0 (mb),
    //  state_str += ", ";
    //  state_str += p0_BMP280;          //baseline pressure
  }
  if (globals->use_GPS && state->status_GPS < 99) {
    if (ser_str.length() > 0)
      ser_str += ", ";
    char lat[20], lon[20];
    floatToString(lat, state->latitude, 6, 8);
    floatToString(lon, state->longitude, 6, 8);
    ser_str += state->satellites;   ser_str += ", ";
    ser_str += lat; ser_str += ", ";//state->latitude;
    ser_str += lon; ser_str += ", "; //state->longitude;
    ser_str += state->gps_altitude; ser_str += ", ";
    ser_str += state->course;       ser_str += ", ";
    ser_str += state->ground_speed; ser_str += ", ";
    ser_str += state->gps_fix_age;  ser_str += ", ";
    ser_str += state->gps_hdop;     ser_str += ", ";
    ser_str += state->date;
  }

  strcpy (globals->state_str, ser_str.c_str());
}

String getDataHeader(System::State* state, System::Globals* globals)
{
  String header_str = "";
  if (globals->use_AIRSpeed && state->status_AIRSPEED == 1)
  {
    header_str += "airspeed";
  }
  if (globals->use_MPU && state->status_MPU == 1) {
    if (header_str.length() > 0)
      header_str += ", ";
    header_str += "ax, ay, az, gx, gy, gz, mx, my, mz";
  }

  if (globals->use_BMP180 && state->status_BMP180 == 1) {
    if (header_str.length() > 0)
      header_str += ", ";
    header_str += "temperature_BMP180, pressure_BMP180, barometric_altitude_BMP180";
  }

  if (globals->use_BMP280 && state->status_BMP280 == 1) {
    if (header_str.length() > 0)
      header_str += ", ";
    header_str += "temperature_BMP280, pressure_BMP280, barometric_altitude_BMP280";
  }

  if (globals->use_GPS && state->status_GPS < 99) {
    if (header_str.length() > 0)
      header_str += ", ";
    header_str += "satellites, latitude, longitude, gps_altitude, course, ground_speed, gps_fix_age, gps_hdop, date";
  }

  return header_str;
}

//Translate the GPS data into String
String toString(Sensors::GPS_Data data)
{
  String outStinrg = "GPS: ";

  char lat[20], lon[20];
  floatToString(lat, data.latitude, 6, 8);
  floatToString(lon, data.longitude, 6, 8);

  outStinrg += data.satellites; outStinrg += ", ";     // number of satellites
  outStinrg += lat; outStinrg += ", ";     // latitude
  outStinrg += lon; outStinrg += ", ";    // longtitude
  outStinrg += data.altitude; outStinrg += ", ";     // Altitude (m)
  outStinrg += data.course; outStinrg += ", ";      // Course
  outStinrg += data.speed; outStinrg += ", ";        // Speed
  outStinrg += data.fix_age; outStinrg += ", ";       // age of the GPS fix
  outStinrg += data.hdop; outStinrg += ", "; // Accuracy level
  //  outStinrg += data.time; outStinrg += ", ";
  outStinrg += data.date; // Time and date
  return outStinrg;
}

char * floatToString(char * outstr, double val, byte precision, byte widthp) {
  char temp[16];
  byte i;

  // compute the rounding factor and fractional multiplier
  double roundingFactor = 0.5;
  unsigned long mult = 1;
  for (i = 0; i < precision; i++)
  {
    roundingFactor /= 10.0;
    mult *= 10;
  }

  temp[0] = '\0';
  outstr[0] = '\0';

  if (val < 0.0) {
    strcpy(outstr, "-\0");
    val = -val;
  }

  val += roundingFactor;

  strcat(outstr, itoa(int(val), temp, 10)); //prints the int part
  if ( precision > 0) {
    strcat(outstr, ".\0"); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision - 1;
    while (precision--)
      mult *= 10;

    if (val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val) - val ) * mult;
    unsigned long frac1 = frac;

    while (frac1 /= 10)
      padding--;

    while (padding--)
      strcat(outstr, "0\0");

    strcat(outstr, itoa(frac, temp, 10));
  }

  // generate space padding
  if ((widthp != 0) && (widthp >= strlen(outstr))) {
    byte J = 0;
    J = widthp - strlen(outstr);

    for (i = 0; i < J; i++) {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp, outstr);
    strcpy(outstr, temp);
  }

  return outstr;
}

//double to string
char* dToStr (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}


//double to string
char* fToStr (float val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}

String intToStr(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  String retVal = String(sz);

  smartdelay(0);
  return retVal;
}

String floatToStr(float val, float invalid, int len, int prec)
{
  String retVal = "";
  if (val == invalid)
  {
    while (len-- > 1)
      retVal.concat('*');
    retVal.concat(' ');
  }
  else
  {
    char str[20];
    floatToString(str, val, prec, 7);
    retVal.concat(str);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      retVal.concat(' ');
  }
  smartdelay(0);
  return retVal;
}



/*
  state.ax = 0, state.ay = 0, state.az = 0; // Accelerometer vals                       |
  state.gx = 0, state.gy = 0, state.gz = 0; // Gyroscope vals                           | <---- MPU9255
  state.mx = 0, state.my = 0, state.mz = 0; // Magnetometer vals                        |

  state.temperature_BMP180 = 0.0; // the measurred ambient temperature  |
  state.pressure_BMP180 = 0.0;    // the meassured pressure             |
  // the barometric altitude (meters)above baseline, given a      | <---- BMP180
  // pressure measurement P (mb) and the pressure at a baseline P0|
  state.barometric_altitude_BMP180 = 0.0; //                            |
  state.p0_BMP180 = 0.0;  // baseline pressure                         |

  state.temperature_BMP280 = 0.0; // the measurred ambient temperature  |
  state.pressure_BMP280 = 0.0;    // the meassured pressure             |
  // the barometric altitude (meters)above baseline, given a      | <---- BMP280
  // pressure measurement P (mb) and the pressure at a baseline P0|
  state.barometric_altitude_BMP280 = 0.0;  //                           |
  state.p0_BMP280 = 0.0;  // baseline pressure                         |

  state.satellites = 0;    // number of satellites         |
  state.latitude = 0.0,               // latitude                     |
  state.longitude = 0.0,              // longtitude                   |
  state.gps_altitude = 0.0,           // Altitude (m)                 |
  state.course = 0.0,                 // Course                       | <---- GPS
  state.ground_speed = 0.0;           // Ground Speed (GPS based)     |
  state.gps_fix_age = 0;    // age of the GPS fix           |
  state.gps_hdop = 0;       // Accuracy level               |
  state.date = "";                  // Time and date                |

  //Sensors status
  state.status_SD = 0;    // (SD card)                  0 = not connected, 1= connected, 99= general error
  state.status_GPS = 0;   // (GPS)                      0 = not connected or no fix, 1= connected and have fix, 99=error
  state.status_BMP180 = 0; // (Air Pressure/temperature) 0 = not connected, 1= connected and functioning, 99=error
  state.status_BMP280 = 0; // (Air Pressure/temperature) 0 = not connected, 1= connected and functioning, 99=error
  state.status_MPU = 0;   // (Accelometer, Gyro, Mag)   0 = not connected  1= connected and functioning, 99=error
  state.status_CUR = 0;   // (Current Meter)            0 = not connected, 1= connected and functioning, 99=error
  state.status_DLE = 0;   // (Tachometer)               0 = not connected, 1= connected and functioning, 99=error

  return state;

  }
*/
