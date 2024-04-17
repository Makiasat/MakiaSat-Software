#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_PM25AQI.h"
#include <SPI.h>
#include <Wire.h>
#include <AltSoftSerial.h>




Adafruit_BMP280 bmp(10, 11, 12, 13);

Adafruit_HTS221 hts;

AltSoftSerial altSerial;
TinyGPSPlus gps;

SoftwareSerial PM25(4, 5);
/*
SoftwareSerial pmSerial(2, 3);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
*/

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();


void setup()
{
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);

  // Start the software serial port at the GPS's default baud
  altSerial.begin(9600);

  bmp.begin();

  hts.begin_SPI(6, 13, 12, 11);
/*
  pmSerial.begin(9600);
  aqi.begin_UART(&pmSerial);*/
  PM25.begin(9600);
  aqi.begin_UART(&PM25);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (altSerial.available() > 0)
    if (gps.encode(altSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }

}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);Serial.print(F(", "));
    Serial.print(gps.location.lng(), 6);Serial.print(F(", "));
    Serial.print(gps.altitude.meters());Serial.print(F(", "));
  }
  else
  {
    Serial.print("-, -, -,");
  }
  

  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());Serial.print(F(", "));
  }
  else
  {
    Serial.print("00:00:00,");
  }

  bmp280_print();

  hts221_print();

  pm25_print();

  delay(1000);
}

void bmp280_print()
{
  Serial.print(bmp.readTemperature()); Serial.print(F(", ")); Serial.print(bmp.readPressure()); Serial.print(F(", ")); Serial.print(bmp.readAltitude(1013.25));Serial.print(F(", "));
}

void hts221_print()
{
  sensors_event_t temp;
  sensors_event_t humidity;
  hts.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print(temp.temperature); Serial.print(F(", ")); Serial.print(humidity.relative_humidity);Serial.print(F(", "));
}

void pm25_print()
{
  PM25_AQI_Data data;
  if (! aqi.read(&data)) {
    Serial.println("-, -, -");
    delay(500);  // try again in a bit!
    return;
  }
  Serial.print(data.pm10_standard); Serial.print(F(", ")); Serial.print(data.pm25_standard); Serial.print(F(", ")); Serial.println(data.pm100_standard);
}

