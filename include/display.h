#include <Arduino.h>
#include <U8g2lib.h>

#include "animation.h"
#include "battery.h"

// Display refresh rate for smooth lap timing updates
// Set to 16ms (~60Hz) for optimal balance of smoothness and performance
// Can be adjusted: 8ms (125Hz), 16ms (60Hz), 33ms (30Hz)
#define DISPLAY_REFRESH_RATE_MS 16

void begin_display();
void draw_data(float speed);
void loading_screen(bool gps_reading);
void update_loading_screen(bool gps_reading);
void draw_battery();
void init_screen();
void draw_lap_counter();
