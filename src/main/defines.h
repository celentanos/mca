#include <stdint.h>

#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS3231.h>

#define H               0
#define L               1

#define RANGE           1024
#define UREF            5.02
#define UFACTOR         UREF/RANGE

#define TEMP_SENSOR     6           // Data wire is plugged into port 6 on the Arduino
#define TEMP_CRITICAL   30.0

#define LINE_LENGTH     16
#define DEFAULT_STRLEN  50

#define S_CHARGE_CAP    "charge capacity"
#define S_CURRENT_DATE  "CURRENT DATE "
#define S_CURRENT_TIME  "CURRENT TIME "
#define S_SET_Y         "SET Y:"
#define S_SET_M         "SET M:"
#define S_SET_D         "SET D:"
#define S_SET_H         "SET HOUR "
#define S_YES_NO        "YES           NO"
#define S_SET_DATE_Y    "SET DATE: YEAR"
#define S_SET_DATE_M    "SET DATE: MONTH"
#define S_SET_DATE_D    "SET DATE: DAY"
#define S_SET_DATE_H    "SET TIME: HOUR"
#define S_SET_DATE_MIN  "SET TIME: MINUTE"

// DEFINES #####################################################################

static LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

static RtcDS3231 Rtc;
static RtcDateTime now;         /// Hilfsvariable
static RtcDateTime now_temp;
static RtcDateTime timeMy;      /// gesetzte Zeit
static uint8_t timeFlag = 1;    /// wenn die zeit einmal gesetzt wurde

static OneWire oneWire(TEMP_SENSOR);           // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
static DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature.

const uint8_t daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static unsigned long currentMs = 0;
static uint8_t counter = 0;
static uint8_t line = 0;
static char runString[DEFAULT_STRLEN];

static float tempSensor = 0;
static float tempSensorOld = 0;

const int8_t ledPin = 13;           // LED-pin-13
const int8_t chargePin = 7;         // Ladeschaltung aktivieren
const int8_t chargePinMode = 8;     // Ladestrategie umschalten

// -----------------------------------------------------------------------------
static int8_t lcdPrintFlag = 0;

static int16_t sensorValue = 0;

/**
 * @brief The pin struct
 *
 * - pin1 = nemu
 * - pin2 = '-'
 * - pin3 = '+'
 */
static struct pin {
    int8_t buttonPin;               // the number of the pushbutton pin
    int8_t buttonState = 0;         // the current reading from the input pin
    int8_t lastButtonState = LOW;   // the previous reading from the input pin
    long lastDebounceTime = 0;      // the last time the output pin was toggled
    long debounceDelay = 50;        // the debounce time; increase if the output flickers
    int8_t reading;
} pin1, pin2, pin3, pin4;

static enum e_state {
    RTC_DATE,
    RTC_SET,
    BAT_CAPACITY,
    BAT_LOW_VOLTAGE,    /// die Akku ist zu tief entladen
    BAT_NO_BAT,         /// keine Akku eingesetzt
    BAT_CHECK,          /// hier werden nur die Werte angezeigt
    CHARGE1,            /// Aufbewahrung
    CHARGE2,            /// Aufladen
    CHARGE3,            /// Aufladen voll
    COOLING,
    WAITING             /// Aufgeladen
} state = BAT_CAPACITY;

static enum e_date {
    DATE_YEAR,
    DATE_MONTH,
    DATE_DAY,
    DATE_HOUR,
    DATE_MIN,
    DATE_CHECK
} stateDateTime = DATE_YEAR;

enum e_voltage {
    VLOW,           /// 3,0V
    V50 = 50,       /// 3,7V
    V60 = 60,       /// 3,8V
    V70 = 70,       /// 3,9V
    V75 = 75,       /// 4,0V
    V80 = 80,       /// 4,1V
    V85 = 85,       /// 4,2V
    VHIGH           /// > 4,3V
};

static struct voltage_type {
    e_voltage percent = V60;    // das sind die %
    int16_t value;              // das ist der Digit-Wert
} voltage;
