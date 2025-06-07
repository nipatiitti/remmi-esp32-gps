#include <Arduino.h>
#include <TinyGPS++.h>

void sendGPSData(float latitude, float longitude, TinyGPSTime time,
                 TinyGPSDate date, float speed, float altitude,
                 float trueCourse, float hdop, uint8_t satellites);