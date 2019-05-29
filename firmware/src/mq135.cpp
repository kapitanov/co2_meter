#include "mq135.h"
#include "config.h"
#include "log.h"

mq135_t::mq135_t(int pin)
{
    _pin = pin;
}

void mq135_t::init()
{
    pinMode(_pin, INPUT);
    float ro = MQ135_RO;
    log_printf(F("sensor Ro = %f"), &ro);
}

void mq135_t::read(mq135_reading_t &reading)
{
    float r = read_resistance();
    float ratio = r / MQ135_RO;
    float ppm = exp((log(ratio) - 1.92) / (-0.42));

    reading.resistance = r;
    reading.ppm = ppm;
}

float mq135_t::read_resistance()
{
    float rs = 0;
    for (int i = 0; i < MQ135_SAMPLE_TIMES; i++)
    {
        int val = analogRead(_pin);
        float r = ((1023. / (float)val) * 5. - 1.) * MQ135_R_LOAD;
        rs += r;
        delay(MQ135_SAMPLE_INTERVAL);
    }
    rs = rs / MQ135_SAMPLE_TIMES;
    return rs;
}