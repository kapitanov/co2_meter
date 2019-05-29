#pragma once

#include <Arduino.h>

#define MQ135_SAMPLE_TIMES    10
#define MQ135_SAMPLE_INTERVAL 50
#define MQ135_R_LOAD          10.0
#define MQ135_HEATING_TIME    6000

struct mq135_reading_t {
    float resistance;
    float ppm; 
};

class mq135_t
{
public:
    mq135_t(int pin);

    void init();
    void read(mq135_reading_t& reading);

private:
    int _pin;
    
    float read_resistance();
};