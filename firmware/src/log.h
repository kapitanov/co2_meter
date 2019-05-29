#pragma once
#include <Arduino.h>

void log_init(const __FlashStringHelper* prefix, int baud_rate);
void log_printf(const __FlashStringHelper* format, ...);