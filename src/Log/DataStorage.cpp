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

#if defined(BUILTIN_SDCARD)
const int SDchipSelect = BUILTIN_SDCARD; // Teensy 4.1 onboard SDIO slot
#else
const int SDchipSelect = 4;
#endif
const int FlashChipSelect = 6; // digital pin for flash chip CS pin
// const int FlashChipSelect = 21; // Arduino 101 built-in SPI Flash

bool IsSdCardReady = false;
static bool g_sd_logging_enabled = true;
static char g_log_filename[32] = "/Flight_001.bin";

static File g_log_file;

void SetSdLoggingEnabled(bool enabled)
{
    g_sd_logging_enabled = enabled;
}

bool IsSdLoggingEnabled()
{
    return g_sd_logging_enabled;
}

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
        // Find next available filename to prevent overwrites across reboots
        for (int i = 1; i <= 999; ++i)
        {
            snprintf(g_log_filename, sizeof(g_log_filename), "/Flight_%03d.bin", i);
            if (!SD.exists(g_log_filename))
            {
                break;
            }
        }

        g_log_file = SD.open(g_log_filename, FILE_WRITE);
        if (g_log_file)
        {
            Serial.printf("Logging binary telemetry to SD file: %s\n", g_log_filename);
        }
        else
        {
            Serial.printf("Failed to create/open %s on SD card\n", g_log_filename);
            IsSdCardReady = false;
        }
    }
}

void InitDataStorage()
{
    InitSdCard();
    // Serial.printf("InitDataStorage()\n");
}

void StoreBytes(char bytes[], int len)
{
    if (!g_sd_logging_enabled || !IsSdCardReady || !g_log_file) return;

    g_log_file.write(reinterpret_cast<const uint8_t*>(bytes), len);
    g_log_file.flush();
}

void StoreStringLine(std::string s)
{
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
