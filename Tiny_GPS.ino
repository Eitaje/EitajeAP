#include <TinyGPS.h>

/* This sample code demonstrates the normal use of a TinyGPS object.
   It assumes that you have a 9600-baud serial GPS device hooked up on pins 4(rx) and 3(tx).

  DOP Value | Rating    | Description
  ----------|-----------|-----------------------------------------------------------------------------------------------------------------------------------------------------
  < 1       | Ideal     | Highest possible confidence level to be used for applications demanding the highest possible precision at all times.
  1-2       | Excellent | At this confidence level, positional measurements are considered accurate enough to meet all but the most sensitive applications.
  2-5       | Good      | Represents a level that marks the minimum appropriate for making business decisions. Positional measurements could be used to make reliable in-route navigation suggestions to the user.
  5-10      | Moderate  | Positional measurements could be used for calculations, but the fix quality could still be improved. A more open view of the sky is recommended.
  10-20     | Fair      | Represents a low confidence level. Positional measurements should be discarded or used only to indicate a very rough estimate of the current location.
  >20       | Poor      | At this level, measurements are inaccurate by as much as 300 meters with a 6-meter accurate device (50 DOP × 6 meters) and should be discarded.
  ----------|-----------|-----------------------------------------------------------------------------------------------------------------------------------------------------
*/

TinyGPS gps;

//static void smartdelay(unsigned long ms);
static String print_float(float val, float invalid, int len, int prec);
static String print_int(unsigned long val, unsigned long invalid, int len);
static String print_date(TinyGPS &gps);
static String print_str(const char *str, int len);
static void smartdelay(unsigned long ms);

String setupGPS()
{
  String retVal;
  retVal.concat("Sats HDOP Latitude  Longitude  Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum\n");
  retVal.concat("          (deg)     (deg)      Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail\n");
  retVal.concat("-------------------------------------------------------------------------------------------------------------------------------------\n");

  return retVal;
}

void get_GPS_data(System::State* state)
{
  //  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  state->satellites = gps.satellites();
  state->gps_hdop = gps.hdop();

  gps.f_get_position(&state->latitude, &state->longitude, &state->gps_fix_age);

  state-> date          = print_date(gps);
  state-> gps_altitude  = gps.f_altitude();
  state-> course        = gps.f_course();
  state-> ground_speed  = gps.f_speed_kmph();

  //update GPS status:
  if (state->satellites == 255)
    state->status_GPS = 0;
  else if (state->satellites > 0 && state->satellites < 255)
    state->status_GPS = 1;

  //  retVal.concat(print_str(gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(gps.f_course()), 6));
  //  retVal.concat(print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000, 0xFFFFFFFF, 9));
  //  retVal.concat(print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2));
  //  retVal.concat(print_str(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON)), 6));

}
//get_GPS_data as string
String get_GPS_data_str()
{
  String retVal;

  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  retVal.concat(print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5));
  retVal.concat(print_int(gps.hdop(), TinyGPS::GPS_INVALID_HDOP, 5));
  gps.f_get_position(&flat, &flon, &age);
  retVal.concat(print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6));
  retVal.concat(print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6));
  retVal.concat(print_int(age, TinyGPS::GPS_INVALID_AGE, 5));
  retVal.concat(print_date(gps));
  retVal.concat(print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2));
  retVal.concat(print_float(gps.f_course(), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2));
  retVal.concat(print_float(gps.f_speed_kmph(), TinyGPS::GPS_INVALID_F_SPEED, 6, 2));
  retVal.concat(print_str(gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(gps.f_course()), 6));
  retVal.concat(print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000, 0xFFFFFFFF, 9));
  retVal.concat(print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2));
  retVal.concat(print_str(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, LONDON_LAT, LONDON_LON)), 6));

  gps.stats(&chars, &sentences, &failed);
  retVal.concat(print_int(chars, 0xFFFFFFFF, 6));
  retVal.concat(print_int(sentences, 0xFFFFFFFF, 10));
  retVal.concat(print_int(failed, 0xFFFFFFFF, 9));
  //  retVal.concat("\n");

  return retVal;
}

static String print_float(float val, float invalid, int len, int prec)
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

static String print_int(unsigned long val, unsigned long invalid, int len)
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

static String print_date(TinyGPS &gps)
{
  String retVal = "";
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    retVal.concat("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
            day, month, year, hour, minute, second);
    retVal.concat(sz);
  }
  retVal.concat(print_int(age, TinyGPS::GPS_INVALID_AGE, 5));
  smartdelay(0);
  return retVal;
}

static String print_str(const char *str, int len)
{
  String retVal = "";
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    retVal.concat(i < slen ? str[i] : ' ');
  smartdelay(0);
  return retVal;
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial2.available())
      gps.encode(Serial2.read());
  } while (millis() - start < ms);
}

