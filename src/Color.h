#pragma once

#include <cstdint>

class Color
{
public:
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha); // Updated constructor

    void print();

    uint8_t getRed() const;
    uint8_t getGreen() const;
    uint8_t getBlue() const;
    uint8_t getAlpha() const;

private:
    uint8_t m_red;
    uint8_t m_green;
    uint8_t m_blue;
    uint8_t m_alpha;
};
