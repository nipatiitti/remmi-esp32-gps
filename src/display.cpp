#include <display.h>

#include "laptimes.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(
    U8G2_R2, /* reset=*/U8X8_PIN_NONE, /* clock=*/7,
    /* data=*/6);  // ESP32 Thing, HW I2C with pin remapping

const uint8_t d_width = u8g2.getDisplayWidth();
const uint8_t d_height = u8g2.getDisplayHeight();

void begin_display() { u8g2.begin(); }

void draw_data(float speed) {
    // Round to 1 decimal and split
    uint8_t integer = min((int)speed, 99);
    uint8_t decimal = (int)(speed * 10) % 10;

    u8g2.firstPage();
    do {
        // Draw speed
        u8g2.setFont(u8g2_font_inr46_mn);
        const uint8_t font_width = 38;
        const uint8_t font_baseline = 46;

        if (integer >= 10) {
            u8g2.drawStr(0, font_baseline, String(integer).c_str());
        } else {
            // Pad zero to the left
            u8g2.drawStr(0, font_baseline, "0");
            u8g2.drawStr(0 + font_width, font_baseline,
                         String(integer).c_str());
        }
        u8g2.drawCircle(0 + font_width * 2 + 4, 42, 2, U8G2_DRAW_ALL);
        u8g2.drawStr(0 + font_width * 2 + 8, font_baseline,
                     String(decimal).c_str());

        // Laptimes
        u8g2.setFont(u8g2_font_helvR14_tf);  // 16px font
        if (showing_lap_result) {
            // Show lap result: how much faster/slower than target (in seconds
            // only)
            String diff_str;
            int diff_seconds = (int)(abs(target_lap.lap_time_diff) / 1000);
            if (target_lap.slower_than_target) {
                diff_str = "-" + String(diff_seconds);
            } else {
                diff_str = "+" + String(diff_seconds);
            }

            // Align the difference text to bottom right
            uint8_t text_width =
                diff_str.length() *
                9;  // Approximate character width for 16px font
            uint8_t start_x = d_width - text_width;
            u8g2.drawStr(start_x, d_height, diff_str.c_str());
        } else {
            // Show current lap time
            String current_time =
                getCurrentLapTimeFormatted();  // Align the current lap time to
                                               // bottom right
            uint8_t text_width =
                current_time.length() *
                9;  // Approximate character width for 16px font
            uint8_t start_x = d_width - text_width;
            u8g2.drawStr(start_x, d_height, current_time.c_str());
        }  // Draw the battery level or lap counter
        if (showing_lap_result) {
            draw_lap_counter();
        } else {
            draw_battery();
        }
    } while (u8g2.nextPage());
}

int c_frame = 0;
int last_frame = millis();

void loading_screen(bool gps_reading) {
    if (millis() - last_frame > FRAME_DELAY) {
        c_frame = (c_frame + 1) % FRAME_COUNT;
        last_frame = millis();
        update_loading_screen(gps_reading);
    }
}

void update_loading_screen(bool gps_reading) {
    u8g2.firstPage();
    do {
        u8g2.setBitmapMode(1);
        u8g2.drawXBMP(
            0, 4, FRAME_WIDTH, FRAME_HEIGHT,
            animation[c_frame]);  // At the last line center "Waiting for GPS"
        u8g2.setFont(u8g2_font_helvR14_tf);  // 16px font
        // u8g2.drawStr(d_width / 2 - (8 * 6), d_height - 2, "Waiting for
        // GPS!");

        if (gps_reading) {
            // Waiting for location
            u8g2.drawStr(d_width / 2 - (8 * 9), d_height, "Space 404");
        } else {
            // Waiting for GPS
            u8g2.drawStr(d_width / 2 - (8 * 9), d_height, "GPS 404");
        }

        draw_battery();
    } while (u8g2.nextPage());
}

// tell lap counter in the bottom left corner (current lap / total laps)
void draw_lap_counter() {
    u8g2.setFont(u8g2_font_helvR14_tf);  // 16px font
    String lap_str = String(lap) + " / " + String(target_laps);
    u8g2.drawStr(0, d_height, lap_str.c_str());
}

// tell battery percentage in the bottom left corner
void draw_battery() {
    auto level = get_battery_percentage();

    // constrain to 0-100%
    level = constrain(level, 0.0, 100.0);

    u8g2.setFont(u8g2_font_helvR14_tf);  // 16px font
    String battery_str = String(level, 0) + "%";
    u8g2.drawStr(0, d_height, battery_str.c_str());
}