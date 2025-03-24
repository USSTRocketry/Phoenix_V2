#pragma once

#include <Adafruit_BMP280.h>
#include <cstdint>
#include "Sensor.h"

class BMP280 : public Sensor
{
public:
    BMP280(uint8_t i2c_addr, TwoWire* i2c_wire) : m_BMP(i2c_wire), m_I2C_Addr(i2c_addr), m_I2C_Wire(i2c_wire) {}

    bool Init();
    bool CollectData(SensorData&);

private:
    Adafruit_BMP280 m_BMP;
    float m_GroundAltitude;
    uint8_t m_I2C_Addr;
    TwoWire* m_I2C_Wire;
};
