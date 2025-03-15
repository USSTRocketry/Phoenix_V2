#pragma once

#include "Sensor.h"

#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <cstdint>

class LIS3MDL : public Sensor
{
public:
    LIS3MDL(uint8_t i2c_addr, uint8_t i2c_wire);
    LIS3MDL(uint8_t cs, uint8_t miso, uint8_t mosi, uint8_t sck);
    bool Init();

private:
    Adafruit_LIS3MDL m_LIS3MDL;
};
