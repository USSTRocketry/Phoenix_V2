#pragma once

#include <string>
#include "SensorInterface.h"
#include "States.h"

void InitDataStorage();

void StoreBytes(char bytes[], int len);
void StoreStringLine(std::string s);
std::string ConvertDataToCSVRow(FlightState, const SensorData&);
void StoreData(FlightState, const SensorData&);

char* GetByteData(int len, int offset); // Return Bytes Array
