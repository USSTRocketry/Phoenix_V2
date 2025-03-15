#pragma once

struct SensorData
{
};

// sensor interface
class Sensor
{
public:
    // places the data into the param
    // true if successfully read and placed
    virtual bool CollectData(SensorData&) = 0;
};
