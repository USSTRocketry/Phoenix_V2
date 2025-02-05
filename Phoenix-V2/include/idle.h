#ifndef IDLE_H
#define IDLE_H

class Idle {
public:
    Idle();
    ~Idle();

    bool loop() const;

private:
    bool running;
};

#endif
