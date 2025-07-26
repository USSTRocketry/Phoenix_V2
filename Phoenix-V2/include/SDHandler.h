#include <SD.h>
#include <SPI.h>
#include <string>

bool InitSD();

void StoreStringLineToCSV(std::string stringline);
