#ifndef IDLE_H
#define IDLE_H

class Idle {
public:
    Idle();
    ~Idle();

    bool loop();

private:
    int i;
    bool switch_condition() const;
};

#endif
