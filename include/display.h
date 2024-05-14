#include <Arduino.h>
#include <U8g2lib.h>

#include "animation.h"

void begin_display();
void draw_data(float speed, int relativeLapTime, boolean direction);
void loading_screen();
void update_loading_screen();