#pragma once
#include <Arduino.h>

#define BAUD_RATE 9600

#define MQ135_SENSOR_PIN A0
#define MQ135_HEATER_PIN 2
#define MQ135_RO 138.5

#define DANGER_LED_PIN 3
#define HEATER_LED_PIN 4
#define LOW_BATTERY_LED_PIN 5
#define POWER_LED_PIN 7

#define METER_PIN 6
#define BATTERY_SENSOR_PIN A5
#define MIN_BATTERY_VOLTAGE 7.2
#define MAX_CO_PPM 900