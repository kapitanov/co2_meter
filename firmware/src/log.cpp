#include "log.h"

enum print_placeholder_result
{
    RESULT_ERROR,
    RESULT_ONE_CHAR,
    RESULT_TWO_CHARS,
};

print_placeholder_result print_format_placeholder(const char c, const char extra, va_list &args);

const __FlashStringHelper* log_prefix;

void log_init(const __FlashStringHelper *prefix, int baud_rate)
{
    log_prefix = prefix;

    Serial.begin(baud_rate);
    while (!Serial)
    {
    }
}

void log_printf(const __FlashStringHelper *format, ...)
{
    if(log_prefix != NULL)
    {
        Serial.print(log_prefix);
        Serial.print(':');
        Serial.print(' ');
    }

    va_list args;
    va_start(args, format);

    uint16_t address = reinterpret_cast<uint16_t>(format);
    --address;
    while (true)
    {
        ++address;
        char c = pgm_read_byte(address);
        if (c == '\0')
        {
            break;
        }

        if (c == '%')
        {
            ++address;
            c = pgm_read_byte(address);
            char e = c != '\0'
                         ? pgm_read_byte(address + 1)
                         : '\0';
            switch (print_format_placeholder(c, e, args))
            {
            case RESULT_ERROR:
                return;

            case RESULT_ONE_CHAR:
                break;

            case RESULT_TWO_CHARS:
                ++address;
                break;
            }
        }
        else
        {
            Serial.print(c);
        }
    }

    Serial.println();
}

template <typename T>
const T get_argument(va_list &args)
{
    int value = va_arg(args, int);
    return reinterpret_cast<T>(value);
}

template <>
const bool get_argument(va_list &args)
{
    int value = va_arg(args, int);
    return static_cast<bool>(value);
}

template <>
const int8_t get_argument(va_list &args)
{
    int value = va_arg(args, int);
    return static_cast<int8_t>(value);
}

template <>
const int16_t get_argument(va_list &args)
{
    int value = va_arg(args, long);
    return static_cast<int16_t>(value);
}

template <>
const char get_argument(va_list &args)
{
    int value = va_arg(args, int);
    return static_cast<char>(value);
}

print_placeholder_result print_format_placeholder(const char c, const char extra, va_list &args)
{
    switch (c)
    {
    case '\0':
        // %\0
        // End of string
        break;

    case '%':
        // %%
        // '%' literal
        Serial.print('%');
        break;

    case 'c':
        // %c
        // Character
        {
            const char s = get_argument<char>(args);
            Serial.print(s);
        }
        break;

    case 's':
        // %s
        // Null-terminated string
        {
            const char *s = get_argument<char *>(args);
            Serial.print(s);
        }
        break;

    case 'p':
        // PROGMEM null-terminated string
        switch (extra)
        {
        case 's':
            // %ps
            {
                const __FlashStringHelper *s = get_argument<__FlashStringHelper *>(args);
                Serial.print(s);
            }
            break;

        default:
            return RESULT_ERROR;
        }
        return RESULT_TWO_CHARS;

    case 'b':
        // %b
        // signed 1-byte decimal integer
        {
            const int8_t x = get_argument<int8_t>(args);
            Serial.print(x, DEC);
        }
        break;

    case 'd':
        // %d
        // signed 2-byte decimal integer
        {
            const int16_t x = get_argument<int16_t>(args);
            Serial.print(x, DEC);
        }
        break;

    case 'l':
        // %l
        // signed 4-byte decimal integer
        {
            const int32_t x = *get_argument<int32_t *>(args);
            Serial.print(x, DEC);
        }
        break;

    case 'f':
        // %f
        // floating-point number
        {
            const float s = *get_argument<float *>(args);
            Serial.print(s);
        }
        break;

    case 'u':
        // Unsigned decimal integer
        switch (extra)
        {
        case 'b':
            // %ub
            // unsigned 1-byte decimal integer
            {
                const uint8_t x = static_cast<uint8_t>(get_argument<int8_t>(args));
                Serial.print(x, DEC);
            }
            break;

        case 'd':
            // %ud
            // unsigned 2-byte decimal integer
            {
                const uint16_t x = static_cast<uint16_t>(get_argument<int16_t>(args));
                Serial.print(x, DEC);
            }
            break;

        case 'l':
            // %ul
            // unsigned 4-byte decimal integer
            {
                const uint32_t x = *get_argument<uint32_t *>(args);
                Serial.print(x, DEC);
            }
            break;

        default:
            return RESULT_ERROR;
        }
        return RESULT_TWO_CHARS;

    case 'x':
        // Hexadecimal integer
        switch (extra)
        {
        case 'b':
            // %xb
            // unsigned 1-byte decimal integer
            {
                const uint8_t x = static_cast<uint8_t>(get_argument<int8_t>(args));
                Serial.print(x, HEX);
            }
            break;

        case 'd':
            // %xd
            // unsigned 2-byte decimal integer
            {
                const uint16_t x = static_cast<uint16_t>(get_argument<int16_t>(args));
                Serial.print(x, HEX);
            }
            break;

        case 'l':
            // %xl
            // unsigned 4-byte decimal integer
            {
                const uint32_t x = *get_argument<uint32_t *>(args);
                Serial.print(x, HEX);
            }
            break;

        default:
            return RESULT_ERROR;
        }

        return RESULT_TWO_CHARS;
    }

    return RESULT_ONE_CHAR;
}