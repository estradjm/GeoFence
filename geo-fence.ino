#include <TinyGPS++.h>
#include <SoftwareSerial.h>
/*
   This program uses the TinyGPS library to parse Latitude and Longitude coordinates from a Sparkfun GPS Logger Shield connected to an Arduino UNO.
   The 9600-baud serial GPS device is hooked up on pins 9(rx) and 8(tx). LED in pin 13 will turn on when the device leaves the geofenced area, otherwise, 
   the LED will be off. Modified version of Mikal Hart's DeviceExample.ino from TinyGPSPlus-0.94b.
*/
static const int RXPin = 9, TXPin = 8;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(TXPin, RXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Serial.println(F(" Latitude   Longitude   Fence"));
  Serial.println(F("  (deg)      (deg)            "));
  Serial.println(F("-----------------------------"));
  pinMode(13, OUTPUT);
}

void loop()
{

/*
 * Snippet to be used in later expansion to bring in coordinates via bluetooth connection using Google Maps interface. 
 * Right now, points are hard coded in.
 * 
  String content = "";
  char character;

  while(Serial.available()) {
      character = Serial.read();
      content.concat(character);
  }

  if (content != "") {
    Serial.println(content);
  }

  */

  // Make a square first for testing purposes 
  static const double Point1_LAT = 35.89156, Point1_LON = -106.32358;
  static const double Point2_LAT = 35.89483, Point2_LON = -106.32173;
  static const double Point3_LAT = 35.89531, Point3_LON = -106.3228;
  static const double Point4_LAT = 35.89222, Point4_LON = -106.32542;

  // Print to serial port the latitude and longitude coordinates of the device
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);

  // Other useful information for later?
  //printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  //printInt(gps.hdop.value(), gps.hdop.isValid(), 5);
  //printInt(gps.location.age(), gps.location.isValid(), 5);
  //printDateTime(gps.date, gps.time);
  //printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  //printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  //printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  //printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.value()) : "*** ", 6);

  unsigned long distanceKmToPoint1 =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Point1_LAT, 
      Point1_LON) / 1000; /// 1000
  printInt(distanceKmToPoint1, gps.location.isValid(), 9);

  unsigned long distanceKmToPoint2 =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Point2_LAT, 
      Point2_LON) / 1000; /// 1000
  printInt(distanceKmToPoint2, gps.location.isValid(), 9);

    unsigned long distanceKmToPoint3 =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Point3_LAT, 
      Point3_LON) / 1000; /// 1000
  printInt(distanceKmToPoint3, gps.location.isValid(), 9);

    unsigned long distanceKmToPoint4 =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      Point4_LAT, 
      Point4_LON) / 1000; /// 1000
  printInt(distanceKmToPoint4, gps.location.isValid(), 9);

 /* 
  *  If outside of fence area, turn on LED, otherwise the LED is off
*/
  bool b1, b2, b3;
  bool b4, b5, b6;
  
  b1 = (gps.location.lat() - Point2_LAT) * (Point1_LON - Point2_LON) - (Point1_LAT - Point2_LAT) * (gps.location.lng() - Point2_LON)< 0.0f;
  b2 = (gps.location.lat() - Point3_LAT) * (Point2_LON - Point3_LON) - (Point2_LAT - Point3_LAT) * (gps.location.lng() - Point3_LON)< 0.0f;
  b3 = (gps.location.lat() - Point1_LAT) * (Point3_LON - Point1_LON) - (Point3_LAT - Point1_LAT) * (gps.location.lng() - Point1_LON)< 0.0f;
  
  b4 = (gps.location.lat() - Point4_LAT) * (Point1_LON - Point4_LON) - (Point1_LAT - Point4_LAT) * (gps.location.lng() - Point4_LON)< 0.0f;
  b5 = (gps.location.lat() - Point3_LAT) * (Point4_LON - Point3_LON) - (Point4_LAT - Point3_LAT) * (gps.location.lng() - Point3_LON)< 0.0f;
  b6 = (gps.location.lat() - Point1_LAT) * (Point3_LON - Point1_LON) - (Point3_LAT - Point1_LAT) * (gps.location.lng() - Point1_LON)< 0.0f;



if (((b1 == b2) && (b2 == b3)) || ((b4 == b5) && (b5 == b6))){
  //point is inside rectangle - inside fence
          Serial.println("Inside of fence!!");  
        digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  }
else
{
  // point is outside of rectangle - outside fence
        Serial.println("Out of fence!!");          
        digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)   
  }

    Serial.println();
    smartDelay(1000);

// Throw an error if there is a time-out
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// Custom version of delay() ensures that the gps object
// is being read entirely.
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
