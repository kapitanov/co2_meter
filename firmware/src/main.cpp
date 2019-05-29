#include <Arduino.h>

#include "log.h"
#include "config.h"
#include "mq135.h"

#define MEASURE_INTERVAL 500
#define BATTERY_MEASURE_INTERVAL 1000

enum state_t
{
    STATE_IDLE,
    STATE_HEATING,
    STATE_ACTIVE,
};

mq135_t mq135(MQ135_SENSOR_PIN);
state_t state = STATE_IDLE;
u32 state_time;
u32 batt_time;

void setup()
{
    pinMode(DANGER_LED_PIN, OUTPUT);
    pinMode(HEATER_LED_PIN, OUTPUT);
    pinMode(LOW_BATTERY_LED_PIN, OUTPUT);
    pinMode(MQ135_HEATER_PIN, OUTPUT);
    pinMode(METER_PIN, OUTPUT);
    pinMode(POWER_LED_PIN, OUTPUT);
    pinMode(BATTERY_SENSOR_PIN, INPUT);

    analogWrite(METER_PIN, 0);

    log_init(F("co2_meter"), BAUD_RATE);
    log_printf(F("start up"));

    mq135.init();
}

void loop()
{
    switch (state)
    {
    case STATE_IDLE:
        log_printf(F("heating..."));
        digitalWrite(POWER_LED_PIN, 1);
        digitalWrite(HEATER_LED_PIN, 1);
        digitalWrite(MQ135_HEATER_PIN, 1);

        state = STATE_HEATING;
        state_time = millis();
        break;

    case STATE_HEATING:
        if (millis() - state_time >= MQ135_HEATING_TIME)
        {
            digitalWrite(HEATER_LED_PIN, 0);
            log_printf(F("heating completed"));
            state = STATE_ACTIVE;
            state_time = millis();
        }
        break;

    case STATE_ACTIVE:
        if (millis() - state_time >= MEASURE_INTERVAL)
        {
            mq135_reading_t reading;
            mq135.read(reading);

            int val = 255. * reading.ppm / 1500.;
            analogWrite(METER_PIN, val);

            log_printf(F("resistance: %f Ohm, CO2: %f PPM"), &reading.resistance, &reading.ppm);

            if (reading.ppm >= MAX_CO_PPM || reading.ppm <= 0)
            {
                digitalWrite(DANGER_LED_PIN, 1);
            }
            else
            {
                digitalWrite(DANGER_LED_PIN, 0);
            }
            state_time = millis();
        }
        break;
    }

    if (millis() - batt_time >= BATTERY_MEASURE_INTERVAL)
    {
        int batt_val = analogRead(BATTERY_SENSOR_PIN);
        float batt_voltage = 2 * 5. * batt_val / 1023.;

        if (batt_voltage < MIN_BATTERY_VOLTAGE)
        {
            float v = MIN_BATTERY_VOLTAGE;
            log_printf(F("low battery: %f V (need at least %f V)"), &batt_voltage, &v);
            digitalWrite(LOW_BATTERY_LED_PIN, 1);
        }
        else
        {
            digitalWrite(LOW_BATTERY_LED_PIN, 0);
        }

        batt_time = millis();
    }
}