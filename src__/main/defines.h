#include <stdint.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS3231.h>

#define H               0
#define L               1

#define RANGE           1024
#define UREF            5.0
#define UFACTOR         UREF/RANGE

#define TEMP_SENSOR     5           // Data wire is plugged into port 5 on the Arduino
#define TEMP_CRITICAL   30.0

#define LINE_LENGTH     16
#define DEFAULT_STRLEN  50

// DEFINES #####################################################################

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

RtcDS3231 Rtc;
RtcDateTime now;

struct DateTime {
    uint8_t day = 0;
    uint8_t month = 0;
    uint16_t year = 0;
    uint8_t min = 0;
    uint8_t min_temp = 0;
    uint8_t hour = 0;
} dateTime;

OneWire oneWire(TEMP_SENSOR);           // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature.

const uint8_t daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

unsigned long currentMs = 0;
uint8_t counter = 0;
uint8_t line = 0;
char runString[DEFAULT_STRLEN];

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
    RTC_DATE,
    RTC_TIME,
    RTC_CHECK,
    BAT_CAPACITY,
    CHECK,
    CHARGE1,        /// Aufbewahrung
    CHARGE2,        /// Aufladen
    CHARGE3,        /// Aufladen voll
    COOLING,
    WAITING,
    STOP
} state = BAT_CAPACITY;

enum e_date {
    YEAR,
    MONTH,
    DAY,
    MIN,
    HOUR
} stateDateTime = YEAR;

enum e_voltage {
    V50 = 50,       // 3,7V
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
