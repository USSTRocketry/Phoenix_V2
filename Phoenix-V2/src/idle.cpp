#include <iostream>

class Idle {
private:
    int i;

public:
    Idle() : i(0) {}

    bool switch_condition() {
        /* checks if it is time to switch states */
        /* TODO */
        return true;
    }

    bool loop() {
        if (switch_condition()) {
            std::cout << "Switching states" << std::endl;
            return true;
        } else {
            /* Do stuff... */


            return false;
        }
    }
};

