#include <stdint.h>

#define H               0
#define L               1

#define RANGE           1024
#define UREF            5.0
#define UFACTOR         UREF/RANGE

#define TEMP_SENSOR     5           // Data wire is plugged into port 5 on the Arduino
#define TEMP_CRITICAL   30.0

float tempSensor = 0;
float tempSensorOld = 0;

const int8_t ledPin = 13;           // LED-pin-13
const int8_t chargePin = 7;         // Ladeschaltung aktivieren
const int8_t chargePinMode = 8;     // Ladestrategie umschalten

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
    RTC_TIME,
    RTC_DATE,
    BAT_CAPACITY,
    CHECK,
    CHARG1,
    CHARG2,
    CHARG3,
    COOLING,
    WAITING,
    STOP
} state = BAT_CAPACITY;

enum e_voltage {
    V60 = 60,       // 3,8V
    V70 = 70,       // 3,9V
    V75 = 75,       // 4,0V
    V80 = 80,       // 4,1V
    V85 = 85        // 4,2V
};

struct voltage_type {
    e_voltage percent = V60;    // das sind die %
    int16_t value;              // das ist der Digit-Wert
} voltage;
