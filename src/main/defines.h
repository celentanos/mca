#include <stdint.h>

#define H           0
#define L           1

#define RANGE       1024
#define UREF        5.0
#define UFACTOR     UREF/RANGE

#define TEMP_SENSOR 5   // Data wire is plugged into port 5 on the Arduino

const int8_t ledPin = 13;           // LED-pin-13
const int8_t chargePin = 7;         //

// -----------------------------------------------------------------------------
int8_t lcdPrintFlag = 0;

int16_t sensorValue = 0;

/**
 * @brief The pin struct
 *
 * - pin1 = nemu
 * - pin2 = '-'
 * - pin3 = '+'
 */
struct pin {
    int8_t buttonPin;               // the number of the pushbutton pin
    int8_t buttonState = 0;         // the current reading from the input pin
    int8_t lastButtonState = LOW;   // the previous reading from the input pin
    long lastDebounceTime = 0;      // the last time the output pin was toggled
    long debounceDelay = 50;        // the debounce time; increase if the output flickers
    int8_t reading;
} pin1, pin2, pin3;

enum e_state {
    START,
    PROCESS,
    WATCH,
    STOP
} state = START;

enum e_voltage {
    V50 = 50,       // 3,85V = 770
    V60 = 60,       // 3,92V = 784
    V70 = 70,       // 3,99V = 798
    V80 = 80,       // 4,06V = 812
    V90 = 90,       // 4,13V = 826
    V100 = 100      // 4,20V = 840
};

struct voltage_type {
    e_voltage percent = V50;
    int16_t value;
} voltage;
