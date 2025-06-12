#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <mcp_canbus.h>

#include "CAN.h"
#include "battery.h"
#include "config.h"
#include "display.h"
#include "gps.h"
#include "laptimes.h"

#define BATTERY_PIN A0  // Analog pin for battery voltage measurement

void setup(void) {
    begin_display();
    init_screen();

    Serial.begin(115200);

    initGPS();

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

void loop(void) {
    // Update lap timing every loop iteration for real-time precision
    // This provides millisecond-accurate timing regardless of GPS update rate
    updateLapTiming();

    // Update GPS data
    updateGPS();
    if (isGPSValid()) {
        draw_data(getGPSSpeed());
    } else if (!getGPSState().first_update) {
        loading_screen(getGPSState().gps_reading);
    }

    if (getGPSState().update) {
        // Don't bomb the CAN bus with data if nothing has changed
        TinyGPSPlus& gps = getGPSObject();
        sendGPSData(gps.location.lat(), gps.location.lng(), gps.time, gps.date,
                    gps.speed.kmph(), gps.altitude.meters(), gps.course.deg(),
                    gps.hdop.hdop(), gps.satellites.value());
        getGPSState().update = false;
    }
}
