#include <Arduino.h>
#include <U8g2lib.h>

#include "animation.h"
#include "battery.h"

void begin_display();
void draw_data(float speed);
void loading_screen(bool gps_reading);
void update_loading_screen(bool gps_reading);
void draw_battery();
void draw_lap_counter();
