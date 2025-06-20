#include "laptimes.h"

/**
 * @brief This is a module for handling lap time calculations based on GPS data
 * In most of the calculations it's enought to simplify the coordinates to
 * euclidian space, so we can use the Pythagorean theorem to calculate
 * the distance between two points.
 */

Point locationToPoint(TinyGPSLocation& location) {
    Point point;
    point.lat = location.lat();
    point.lng = location.lng();
    return point;
}

// Global variables for lap timing
Point last_position = {0, 0};
Point current_position = {0, 0};
// Both ends of the finish line. Can be just copied from Google maps
Line finish_line = {{50.529042, 18.095856}, {50.529388, 18.096329}};

int full_time = 35 * 60 * 1000;  // 35 mins
int lap = 1;
int target_laps = 11;
double min_laptime =
    15 * 1000.0;  // Minimum lap time to show result (in milliseconds)
double target_laptime =
    full_time / target_laps;  // Target lap time in milliseconds
static unsigned long lap_start_time = 0;
static unsigned long last_finish_time = 0;
bool showing_lap_result = false;
static double current_lap_elapsed = 0.0;

LaptimeTarget target_lap = {0, true};  // Default target lap time difference

// Helper function to convert HHMMSSCC time format to seconds since midnight
// This function is now only used for display purposes, not for lap timing
double timeToSeconds(uint32_t time_value) {
    uint32_t hours = time_value / 1000000;
    uint32_t minutes = (time_value / 10000) % 100;
    uint32_t seconds = (time_value / 100) % 100;
    uint32_t centiseconds = time_value % 100;

    return hours * 3600.0 + minutes * 60.0 + seconds + centiseconds / 100.0;
}

/**
 * @brief Updates lap timing calculations - call every loop iteration
 * This function handles:
 * - Updating current lap elapsed time
 * - Managing cooldown period state
 * - Real-time timing calculations
 */
void updateLapTiming() {
    unsigned long current_time = millis();

    // Check if cooldown period has ended
    if (showing_lap_result &&
        (current_time - last_finish_time >= (unsigned long)min_laptime)) {
        showing_lap_result = false;
    }

    // Calculate current elapsed time if we're timing a lap
    if (lap_start_time > 0 && !showing_lap_result) {
        current_lap_elapsed = current_time - lap_start_time;
    }
}

/**
 * @brief Processes GPS location updates for finish line detection
 * This function handles:
 * - Position updates and tracking
 * - Finish line crossing detection
 * - Lap completion logic
 * Call only when GPS location data is updated (typically ~1Hz)
 */
void processGPSLocation(TinyGPSLocation& location) {
    // Update positions
    Point previous_position = last_position;
    current_position = locationToPoint(location);
    last_position = current_position;

    unsigned long current_time = millis();

    // Check if we're in cooldown period (ignore finish line crossings)
    bool in_cooldown = showing_lap_result && (current_time - last_finish_time <
                                              (unsigned long)min_laptime);

    // Teleplot location and finish line for XY plotting on same chart
    // Send current GPS position as XY coordinates (lng as X, lat as Y)
    Serial.print(">gps_position,track_map:");
    Serial.print(current_position.lng, 8);  // X coordinate (longitude)
    Serial.print(":");
    Serial.print(current_position.lat, 8);  // Y coordinate (latitude)
    Serial.println("|xy");

    // Send finish line as two XY points (start and end) on same chart
    Serial.print(">finish_line_start,track_map:");
    Serial.print(finish_line.A.lng, 8);
    Serial.print(":");
    Serial.print(finish_line.A.lat, 8);
    Serial.println("|xy");

    Serial.print(">finish_line_end,track_map:");
    Serial.print(finish_line.B.lng, 8);
    Serial.print(":");
    Serial.print(finish_line.B.lat, 8);
    Serial.println("|xy");

    bool lines_crossed =
        linesCrossed({previous_position, current_position}, finish_line);

    // Check if the car has crossed the finish line (only if not in cooldown)
    if (!in_cooldown && lines_crossed) {
        // If this is not the very first lap, calculate the lap time
        if (lap_start_time > 0) {
            double current_lap_time = current_time - lap_start_time;

            // Calculate difference from target lap time
            target_lap.lap_time_diff = current_lap_time - target_laptime;
            target_lap.slower_than_target = target_lap.lap_time_diff > 0;

            // Update lap counter
            lap++;

            // Set state to show lap results
            showing_lap_result = true;
            last_finish_time = current_time;
        }

        // Start timing the new lap immediately
        lap_start_time = current_time;
    }
}

// Initialize lap timing - call this when starting the first lap
void initializeLapTiming() {
    lap_start_time = millis();
    showing_lap_result = false;
    current_lap_elapsed = 0;
}

// Helper functions for accessing lap timing state
bool isShowingLapResult() { return showing_lap_result; }

double getCurrentLapTime() { return current_lap_elapsed; }

// Function to set target lap time (for user configuration)
// Input is in seconds, but internally we store in milliseconds
void setTargetLapTime(double target_seconds) {
    target_laptime = target_seconds * 1000.0;
}

// Helper functions for time conversion and formatting
double getCurrentLapTimeSeconds() { return current_lap_elapsed / 1000.0; }

String getCurrentLapTimeFormatted() {
    return formatLapTime(current_lap_elapsed);
}

double getTargetLapTimeSeconds() { return target_laptime / 1000.0; }

// Format lap time from milliseconds to MM:SS.SSS format
String formatLapTime(double time_ms) {
    int total_seconds = (int)(time_ms / 1000);
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int milliseconds = (int)(time_ms) % 1000;

    char formatted[12];
    sprintf(formatted, "%02d:%02d.%03d", minutes, seconds, milliseconds);
    return String(formatted);
}

/**
 * @brief Manually trigger a lap completion for debugging purposes
 * This simulates a finish line crossing without requiring GPS data
 */
void triggerLapManually() {
    unsigned long current_time = millis();

    // Check if we're in cooldown period (ignore manual triggers during
    // cooldown)
    bool in_cooldown = showing_lap_result && (current_time - last_finish_time <
                                              (unsigned long)min_laptime);

    if (in_cooldown) {
        Serial.println("Manual lap trigger ignored - in cooldown period");
        return;
    }

    // If lap timing hasn't been initialized, initialize it first
    if (lap_start_time == 0) {
        Serial.println("Initializing lap timing for manual trigger");
        lap_start_time = current_time;
        return;
    }

    // Calculate the lap time
    double current_lap_time = current_time - lap_start_time;

    // Calculate difference from target lap time
    target_lap.lap_time_diff = current_lap_time - target_laptime;
    target_lap.slower_than_target = target_lap.lap_time_diff > 0;

    // Update lap counter
    lap++;

    // Set state to show lap results
    showing_lap_result = true;
    last_finish_time = current_time;

    // Start timing the new lap immediately
    lap_start_time = current_time;

    Serial.print("Manual lap completed - Lap ");
    Serial.print(lap - 1);
    Serial.print(": ");
    Serial.print(formatLapTime(current_lap_time));
    Serial.print(" (Target: ");
    Serial.print(formatLapTime(target_laptime));
    Serial.print(", Diff: ");
    if (target_lap.slower_than_target) {
        Serial.print("+");
    }
    Serial.print(formatLapTime(abs(target_lap.lap_time_diff)));
    Serial.println(")");
}
