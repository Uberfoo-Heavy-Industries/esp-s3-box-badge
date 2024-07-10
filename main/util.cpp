#include <cmath>
#include "util.h"

lv_color_t hsl_to_rgb(uint16_t h, uint8_t is, uint8_t il) {
    float s = is / 255.0f, l = il / 255.0f;
    float r, g, b;

    float c = (1.0f - std::fabs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2) - 1.0f));
    float m = l - c / 2.0f;

    if (h >= 0 && h < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (h >= 60 && h < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (h >= 180 && h < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (h >= 240 && h < 300) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }

    r += m;
    g += m;
    b += m;

    return lv_color_make(r * 255, g * 255, b * 255);
}