#pragma once
#include <cstdint>

namespace lpm013m126c{

class LCD {
private:
    // Pin for the backlight
    uint32_t backlight_port;
    uint32_t backlight_pin;

    // Pins for the LCD
    uint32_t lcd_port;
    uint32_t sclk_pin;
    uint32_t mosi_pin;
    uint32_t cs_pin;

public:
    LCD(uint32_t backlight_port, uint32_t backlight_pin,
        uint32_t lcd_port, uint32_t sclk_pin, uint32_t mosi_pin, uint32_t cs_pin);

    // TODO, support 0x0-0x100, for now, just set on and off.
    void set_brightness(uint8_t level);
};

};

// #include <stdint>

// struct LCDPacket {
//     enum Modes {

//     };

//     uint16 mode : 6;
//     uint16 gate_address : 10;
// }