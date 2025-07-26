#include <SDHandler.h>
// For examples, go to SDHandler.h > SD.h > from SD.h ../examples/Files/Files.ino

File myFile;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// Teensy audio board: pin 10
// Teensy 3.5 & 3.6 & 4.1 on-board: BUILTIN_SDCARD
// Wiz820+SD board: pin 4
// Teensy 2.0: pin 0
// Teensy++ 2.0: pin 20

#define OPTIONAL_FLASH_CHIP = 7

const int chipSelect = BUILTIN_SDCARD;
// const int flashModule;
int savecount        = 0;

bool InitSD()
{
    // UNCOMMENT THESE TWO LINES FOR TEENSY AUDIO BOARD:
    // SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
    // SPI.setSCK(14);  // Audio shield has SCK on pin 14
    FLASHMEM

    Serial.println("Initializing SD card...");
    if (!SD.begin(chipSelect))
    {
        Serial.println("initialization failed!");
        return false;
    }
    // Open serial communications and wait for port to open:
    // Serial.begin(9600);
    //  while (!Serial) {
    //   ; // wait for serial port to connect.
    // }
    Serial.println("SD card initialized.");

    return true;
}

void StoreStringLineToCSV(std::string stringline)
{
    File f = SD.open("FlightData.csv", FILE_WRITE);
    f.println(stringline.c_str());
    f.close();
}
