#include <Arduino.h>
#include "Watchdog_t4.h"
#include "StateMachine.h"
#include "SensorAggregator.h"

StateMachine SM;
SensorAggregator SensorAccum;
WDT_T4<WDT2> WatchDog;
// put function declarations here:
void WatchDogInterrupt()
{
    // soft reset
    WatchDog.feed();
}

void setup()
{
    Serial.begin(0);

    // soft reset(s), hard reset(s), pin, func ptr for soft reset
    WatchDog.begin({.trigger = 10.0, .timeout = 20.0, .pin = 13, .callback = WatchDogInterrupt});
}

void loop()
{
    WatchDog.feed();

    auto [Result, Data] = SensorAccum.Collect();
    if (!Result)
    {
        Serial.println("data collection failed");
    }

    SM.Run(Data);
    Serial.printf("Hello World\n");
    delay(500);
    // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) { return x + y; }
