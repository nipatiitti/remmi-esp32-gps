#include <Arduino.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <mcp_canbus.h>

#include "CAN.h"
#include "battery.h"
#include "config.h"
#include "display.h"
#include "laptimes.h"

HardwareSerial GPSSerial(1);

// GPS Pins
#define GPS_RX_PIN D3  // RX pin for GPS
#define GPS_TX_PIN D2  // TX pin for GPS

TinyGPSPlus gps;

#define BATTERY_PIN A0  // Analog pin for battery voltage measurement

void setup(void) {
    begin_display();
    init_screen();

    Serial.begin(115200);

    GPSSerial.begin(38400, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    pinMode(BATTERY_PIN, INPUT);  // ADC

    while (CAN_OK !=
           CAN.begin(CAN_1000KBPS))  // init can bus : baudrate = 1 Mbps
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");
}

float speed = 0;

bool update = false;
bool first_update = false;
bool gps_reading = false;
bool lap_timing_initialized = false;

void loop(void) {
    // Update lap timing every loop iteration for real-time precision
    // This provides millisecond-accurate timing regardless of GPS update rate
    updateLapTiming();

    while (GPSSerial.available() > 0) {
        char c = GPSSerial.read();
        gps_reading = true;
        if (gps.encode(c)) {
            if (gps.location.isValid() && gps.location.isUpdated()) {
                update = true;
                first_update = true;

                speed = gps.speed.kmph();

                // Initialize lap timing on first valid GPS reading
                if (!lap_timing_initialized) {
                    initializeLapTiming();
                    lap_timing_initialized = true;
                }

                // Process GPS location data for finish line detection
                processGPSLocation(gps.location);
            }
        }
    }

    if (gps.location.isValid()) {
        draw_data(speed);
    } else if (!first_update) {
        loading_screen(gps_reading);
    }

    if (update) {
        // Don't bomb the CAN bus with data if nothing has changed
        sendGPSData(gps.location.lat(), gps.location.lng(), gps.time, gps.date,
                    gps.speed.kmph(), gps.altitude.meters(), gps.course.deg(),
                    gps.hdop.hdop(), gps.satellites.value());
        update = false;
    }
}
