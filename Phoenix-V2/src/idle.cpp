#include <iostream>
#include "idle.h"


Idle::Idle() : i(0) {}

bool Idle::switch_condition() const {
    /* checks if it is time to switch states */
    /* TODO */
    return true;
}

bool Idle::loop() {
    if (switch_condition()) {
        std::cout << "Switching states" << std::endl;
        return true;
    } else {
        /* Do stuff... */


        return false;
    }
}
