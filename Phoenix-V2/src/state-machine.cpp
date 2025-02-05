#include <iostream>

class StateMachine {
public:
    enum State {
        STATE_Idle,
        STATE_Ascent,
        STATE_Descent,
        STATE_Parachute,
        STATE_Landed
    };

    StateMachine() : state(STATE_INIT) {}

    void state_run() {
        switch (state) {
            case STATE_Idle:
                std::cout << "State: Idle" << std::endl;
                // Add logic for Idle state
                break;
            case STATE_Ascent:
                std::cout << "State: Ascent" << std::endl;
                // Add logic for Ascent state
                break;
            case STATE_Descent:
                std::cout << "State: Descent" << std::endl;
                // Add logic for Descent state
                break;
            case STATE_Parachute:
                std::cout << "State: Parachute" << std::endl;
                // Add logic for Parachute state
                break;
            case STATE_Landed:
                std::cout << "State: Landed" << std::endl;
                // Add logic for Landed state
                break;
            default:
                std::cout << "Unknown State" << std::endl;
                break;
        }
    }

    void loop() {
        state_run();

        /* TODO: call watchdog */
    }

private:
    State state;
};
