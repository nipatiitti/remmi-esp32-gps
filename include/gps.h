#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>

// GPS Hardware Configuration
#define GPS_RX_PIN D3  // RX pin for GPS
#define GPS_TX_PIN D2  // TX pin for GPS
#define GPS_BAUD_RATE 38400

// GPS State Structure
struct GPSState {
    bool update;
    bool first_update;
    bool gps_reading;
    bool lap_timing_initialized;
    float speed;
};

// GPS Hardware
extern HardwareSerial GPSSerial;
extern TinyGPSPlus gps;
extern GPSState gpsState;
extern bool DEBUG;

// Function declarations
void initGPS();
void updateGPS();
bool isGPSValid();
float getGPSSpeed();
void setGPSDebugMode(bool debug_enabled);

// Getter functions for CAN module access
TinyGPSPlus& getGPSObject();
GPSState& getGPSState();

#endif  // GPS_H
