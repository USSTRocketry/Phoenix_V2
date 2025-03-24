#pragma once

#include "Sensor.h"

#include <Adafruit_LSM6DSOX.h>
#include <cstdint>

class LSM6 : public Sensor
{
public:
    LSM6(uint8_t i2c_addr, TwoWire* i2c_wire) : m_I2C_Addr(i2c_addr), m_I2C_Wire(i2c_wire) {}

    bool Init();
    bool CollectData(SensorData&);

private:
    Adafruit_LSM6DSOX m_LSM6;

    uint8_t m_I2C_Addr;
    TwoWire* m_I2C_Wire;
};
