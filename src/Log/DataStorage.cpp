/**
 * @file DataStorage.cpp
 * @author USST Avionics
 * @brief This file has all the logic for saving telemetry data to the SD and flash
 * @version 0.1
 * @date 2025-06-28
 *
 * @copyright Copyright (c) 2025
 *
 */

// #include <Arduino.h>
#include <string>

#include <SerialFlash.h>
#include <SD.h>
#include <SPI.h>

#include "Log/DataStorage.h"
#include "SensorData.h"
#include "Avionics_HAL.h"

const int SDchipSelect    = 4; // Audio Shield has SD card CS on pin 10
const int FlashChipSelect = 6; // digital pin for flash chip CS pin
// const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash

bool IsSdCardReady = false;

void InitSdCard()
{
    if (SD.begin(SDchipSelect))
    {
        IsSdCardReady = true;
    }
    else
    {
        Serial.println("Unable to access SD card - logging to SD will be disabled");
    }

    if (IsSdCardReady)
    {
        // Create/open the binary log file on the SD card.
        // Use the correct flags (bitwise OR) so the file is created if missing.
        if (SD.exists("/FlightData.bin")) { SD.remove("/FlightData.bin"); }
        File F = SD.open("/FlightData.bin", O_CREAT | O_WRITE);
        if (F) { F.close(); }
        else
        {
            Serial.println("Failed to create/open FlightData.bin on SD card");
            IsSdCardReady = false;
        }
    }
}

void InitDataStorage()
{
    InitSdCard();
    Serial.printf("InitDataStorage()\n");
}

void StoreBytes(char bytes[], int len)
{
    if (!IsSdCardReady) return;

    File file = SD.open("/FlightData.bin", O_APPEND);
    if (file)
    {
        file.write(bytes, len);
        file.close();
    }
}

void StoreStringLine(std::string s)
{
    if (!IsSdCardReady) return;

    File file = SD.open("/FlightData.fdat", O_APPEND);
    if (file)
    {
        file.write(s.c_str());
        file.close();
    }
}

void TransferFileData(File to)
{
    File from           = SD.open("/FlightData.fdat", O_READ);
    unsigned long count = from.size();
    while (count > 0)
    {
        char buff[128];
        unsigned long n = count;
        if (n > 128) n = 128;
        from.read(buff, n);
        to.write(buff, n);
        count = count - n;
    }
    from.close();
}

std::string ConvertDataToCSVRow(FlightState State, const SensorData& SensorData)
{
    /* State,Altitude,Pressure,Temperature,Acceleration_X,Acceleration_Y,Acceleration_Z,gyroX,gyroY,gyroZ,
        magneticX,magneticY,magneticX,Timestamp
    */

    return std::to_string(State) + "," +

           std::to_string(SensorData.BMP280.Altitude) + "," + std::to_string(SensorData.BMP280.Pressure) + "," +
           std::to_string(SensorData.BMP280.Temperature) + "," +

           std::to_string(SensorData.AccelGyro.Accel.x()) + "," + std::to_string(SensorData.AccelGyro.Accel.y()) + "," +
           std::to_string(SensorData.AccelGyro.Accel.z()) + "," +

           std::to_string(SensorData.AccelGyro.Gyro.x()) + "," + std::to_string(SensorData.AccelGyro.Gyro.y()) + "," +
           std::to_string(SensorData.AccelGyro.Gyro.z()) + "," +

           std::to_string(SensorData.Magnetic.x()) + "," + std::to_string(SensorData.Magnetic.y()) + "," +
           std::to_string(SensorData.Magnetic.z()) + "," +

           std::to_string(ra::hal::SysUptimeMs()) + "\n";
}

void StoreData(FlightState S, const SensorData& SensorData) { StoreStringLine(ConvertDataToCSVRow(S, SensorData)); }
