#include "battery.h"

int readings = 16;  // Number of readings for averaging

// Battery cache variables
static float cached_percentage = -1.0;  // -1 indicates no cached value yet
static unsigned long last_read_time = 0;
static const unsigned long CACHE_DURATION_MS = 4000;  // 4 seconds

float get_battery_percentage() {
    unsigned long current_time = millis();

    // Return cached value if within cache duration and we have a cached value
    if (cached_percentage >= 0.0 &&
        (current_time - last_read_time) < CACHE_DURATION_MS) {
        return cached_percentage;
    }

    // Perform actual battery reading
    uint32_t Vbatt = 0;
    for (int i = 0; i < readings; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0);  // ADC with correction
    }
    float Vbattf =
        2 * Vbatt / readings / 1000.0;  // attenuation ratio 1/2, mV --> V

    float percentage = (Vbattf - 3.0) / (4.2 - 3.0) * 100.0;  // 3.0V to 4.2V
    percentage = constrain(percentage, 0.0, 100.0);           // Clamp to 0-100%

    // Update cache
    cached_percentage = percentage;
    last_read_time = current_time;

    return percentage;
}