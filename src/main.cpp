#include <Arduino.h>
#include <Wire.h>
#include <display.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>

HardwareSerial GPSSerial(1);
TinyGPSPlus gps;

void setup(void)
{
  begin_display();
  GPSSerial.begin(9600, SERIAL_8N1, 9, 8); // RX, TX
}

int laptime = 30;
float speed = 0;
boolean direction = true;

boolean first_update = false;
boolean update = false;

void loop(void)
{
  while (GPSSerial.available() > 0)
  {
    char c = GPSSerial.read();
    // Do something with C
    if (gps.encode(c))
    {
      if (gps.location.isValid())
      {
        update = true;
        first_update = true;
        speed = gps.speed.kmph();
      }
    }
  }

  if (update)
  {
    draw_data(speed, laptime, direction);
    update = false;
  } else if (!first_update)
  {
    loading_screen();
  }
}
