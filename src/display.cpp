#include <display.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, /* reset=*/U8X8_PIN_NONE, /* clock=*/7, /* data=*/6); // ESP32 Thing, HW I2C with pin remapping

const uint8_t d_width = u8g2.getDisplayWidth();
const uint8_t d_height = u8g2.getDisplayHeight();

void begin_display()
{
    u8g2.begin();
}

void draw_data(float speed, int relativeLapTime, boolean direction)
{
    // Round to 1 decimal and split
    uint8_t integer = min((int)speed, 99);
    uint8_t decimal = (int)(speed * 10) % 10;

    // The lap time will be drawn as a bar on the bottom line of the display
    // It has 60 seconds on either side of the center line with so something like: -5  xxxx|     +
    uint8_t relativeLength = map(relativeLapTime, 0, 60, 0, 32);

    u8g2.firstPage();
    do
    {
        // Draw speed
        u8g2.setFont(u8g2_font_inr46_mn);
        const uint8_t font_width = 38;
        const uint8_t font_baseline = 46;

        if (integer >= 10)
        {
            u8g2.drawStr(0, font_baseline, String(integer).c_str());
        }
        else
        {
            // Pad zero to the left
            u8g2.drawStr(0, font_baseline, "0");
            u8g2.drawStr(0 + font_width, font_baseline, String(integer).c_str());
        }
        u8g2.drawCircle(0 + font_width * 2 + 4, 42, 2, U8G2_DRAW_ALL);
        u8g2.drawStr(0 + font_width * 2 + 8, font_baseline, String(decimal).c_str());

        // Draw 2px center box for the laptimebar on the last line of display
        uint8_t x = d_width / 2 - 1;         // 63
        uint8_t y = d_height - d_height / 4; // 48
        u8g2.drawBox(x, y, 2, d_height / 4);

        // Add + and - to the ends of the line
        u8g2.setFont(u8g2_font_inr16_mn);
        u8g2.drawStr(0, 64, "-");
        u8g2.drawStr(d_width - 13, 64, "+");

        // Draw the relative lap time bar
        // Laptime is always positive so use the direction to determine where to draw the bar
        uint8_t bar_height = d_height / 4 - 8; // 8px margin
        uint8_t y_baseline = y + 4;
        uint8_t x_baseline = x + 1;
        u8g2.setFont(u8g2_font_8x13_mn);
        if (direction)
        {
            u8g2.drawBox(x_baseline, y_baseline, relativeLength, bar_height);
            // Draw the text at the end of the line
            u8g2.drawStr(d_width - 29, y_baseline + 9, String(relativeLapTime).c_str());
        }
        else
        {
            u8g2.drawBox(x_baseline - relativeLength, y_baseline, relativeLength, bar_height);
            // Draw the text at the beginning of the line
            u8g2.drawStr(0 + 8, y_baseline + 9, String(relativeLapTime).c_str());
        }

    } while (u8g2.nextPage());
}

int c_frame = 0;
int last_frame = millis();

void loading_screen()
{
    if (millis() - last_frame > FRAME_DELAY)
    {
        c_frame = (c_frame + 1) % FRAME_COUNT;
        last_frame = millis();
        update_loading_screen();
    }
}

void update_loading_screen()
{
    u8g2.firstPage();
    do
    {
        u8g2.setBitmapMode(1);
        u8g2.drawXBMP(0, 4, FRAME_WIDTH, FRAME_HEIGHT, animation[c_frame]);

        // At the last line center "Waiting for GPS"
        u8g2.setFont(u8g2_font_6x10_mf);
        u8g2.drawStr(d_width / 2 - (8 * 6), d_height - 2, "Waiting for GPS!");
    } while (u8g2.nextPage());
}