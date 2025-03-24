#pragma once

#include "Sensor.h"

#include <Adafruit_LIS3MDL.h>
#include <cstdint>

class LIS3MDL : public Sensor
{
public:
    LIS3MDL(uint8_t i2c_addr, TwoWire* i2c_wire) : m_I2C_Addr(i2c_addr), m_I2C_Wire(i2c_wire) {}

    bool Init();
    bool CollectData(SensorData&);

private:
    Adafruit_LIS3MDL m_LIS3MDL;

    uint8_t m_I2C_Addr;
    TwoWire* m_I2C_Wire;
};
