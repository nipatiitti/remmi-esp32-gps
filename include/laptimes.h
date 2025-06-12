#include <TinyGPS++.h>

#include "math_utils.h"

extern Point last_position;
extern Point current_position;

Point locationToPoint(TinyGPSLocation &location);

extern Line finish_line;

struct LaptimeTarget {
    double lap_time_diff;
    bool slower_than_target;
};

extern int lap;
extern int target_laps;  // To calculate laps left

extern double min_laptime;  // How long should we show the info of the last lap
                            // (in milliseconds)
extern double target_laptime;  // Target lap time in milliseconds

// Additional variables for lap timing state
extern bool showing_lap_result;
extern LaptimeTarget target_lap;

// Helper functions
bool isShowingLapResult();
double getCurrentLapTime();
double getCurrentLapTimeSeconds();
String getCurrentLapTimeFormatted();
double getTargetLapTimeSeconds();
String formatLapTime(double time_ms);
void initializeLapTiming();
void setTargetLapTime(double target_seconds);
double timeToSeconds(uint32_t time_value);

// Manual lap trigger for debugging
void triggerLapManually();

// Separated functions for timing and location processing
void updateLapTiming();  // Call every loop iteration to update times
void processGPSLocation(
    TinyGPSLocation &location);  // Call only when GPS updates
