#include "battery.h"

float get_battery_percentage() {
    uint32_t Vbatt = 0;
    for (int i = 0; i < 16; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0);  // ADC with correction
    }
    float Vbattf = 2 * Vbatt / 16 / 1000.0;  // attenuation ratio 1/2, mV --> V

    float percentage = (Vbattf - 3.0) / (4.2 - 3.0) * 100.0;  // 3.0V to 4.2V
    percentage = constrain(percentage, 0.0, 100.0);           // Clamp to 0-100%
    return percentage;
}