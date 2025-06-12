#include "gps.h"

#include "laptimes.h"

// GPS Hardware instances
HardwareSerial GPSSerial(1);
TinyGPSPlus gps;

// GPS State
GPSState gpsState = {.update = false,
                     .first_update = false,
                     .gps_reading = false,
                     .lap_timing_initialized = false,
                     .speed = 0.0};

bool DEBUG = false;

/**
 * @brief Initialize GPS hardware and serial communication
 */
void initGPS() {
    GPSSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    // Reset GPS state
    gpsState.update = false;
    gpsState.first_update = false;
    gpsState.gps_reading = false;
    gpsState.lap_timing_initialized = false;
    gpsState.speed = 0.0;
}

/**
 * @brief Update GPS data by reading from serial and processing incoming data
 * This function should be called in the main loop to continuously process GPS
 * data
 */
void updateGPS() {
    while (GPSSerial.available() > 0) {
        char c = GPSSerial.read();
        gpsState.gps_reading = true;

        if (DEBUG) {
            Serial.print(c);  // Print raw GPS data for debugging
        }

        if (gps.encode(c)) {
            if (gps.location.isValid() && gps.location.isUpdated()) {
                gpsState.update = true;
                gpsState.first_update = true;
                gpsState.speed = gps.speed.kmph();

                // Initialize lap timing on first valid GPS reading
                if (!gpsState.lap_timing_initialized) {
                    initializeLapTiming();
                    gpsState.lap_timing_initialized = true;
                }

                // Process GPS location data for finish line detection
                processGPSLocation(gps.location);
            }
        }
    }
}

/**
 * @brief Check if GPS has a valid location fix
 * @return true if GPS location is valid, false otherwise
 */
bool isGPSValid() { return gps.location.isValid(); }

/**
 * @brief Get current GPS speed
 * @return Current speed in km/h
 */
float getGPSSpeed() { return gpsState.speed; }

/**
 * @brief Enable or disable GPS debug mode
 * @param debug_enabled true to enable debug output, false to disable
 */
void setGPSDebugMode(bool debug_enabled) { DEBUG = debug_enabled; }

/**
 * @brief Get reference to the GPS object for external modules (like CAN)
 * @return Reference to the TinyGPSPlus object
 */
TinyGPSPlus& getGPSObject() { return gps; }

/**
 * @brief Get reference to the GPS state for external modules
 * @return Reference to the GPSState structure
 */
GPSState& getGPSState() { return gpsState; }
