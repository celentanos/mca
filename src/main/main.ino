#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS3231.h>

#include "functions.h"

// DEFINES #####################################################################

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// FUNCTIONS ###################################################################

void printLine(int8_t line, const char *s)
{
    lcd.setCursor(0, line);
    lcd.print(s);
}

void delLine(int8_t line)
{
    lcd.setCursor(0, line);
    lcd.print("                ");
    lcd.setCursor(0, line);
}

void printCapacity(int value, int8_t line = 1)
{
    delLine(line);
    char s[16] = "capacity: ";
    char s1[4];
    sprintf(s1, "%d", value);
    strcat(s, s1);
    strcat(s, "%");

    lcd.setCursor(0, line);
    lcd.print(s);
}

void printTempValue(const char *s)
{
    if(tempSensorOld != tempSensor) {
        tempSensorOld = tempSensor;
        delLine(0);
        lcd.print(s);
        lcd.print(tempSensor);
    }
}

// MAIN ########################################################################

OneWire oneWire(TEMP_SENSOR);          // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);    // Pass our oneWire reference to Dallas Temperature.

void setup()
{
    // init Temp-Sensor --------------------------------------------------------
    sensors.begin();                            // Start up the library
    sensors.requestTemperatures();              // Send the command to get temperatures

    // init LCD ----------------------------------------------------------------
    lcd.init();                                 // initialize the lcd
    lcd.setBacklight(1);

    // init BUTTONS ------------------------------------------------------------
    pinMode(pin1.buttonPin = 2, INPUT_PULLUP);  // setup pin1
    pinMode(pin2.buttonPin = 3, INPUT_PULLUP);  // setup pin2
    pinMode(pin3.buttonPin = 4, INPUT_PULLUP);  // setup pin3

    // init LED-pin ------------------------------------------------------------
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // init Charge-pin ---------------------------------------------------------
    pinMode(chargePin, OUTPUT);
    digitalWrite(chargePin, LOW);
}

void loop()
{

    switch (state) {
    case BAT_CAPACITY:
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "charge capacity");
            printCapacity(voltage.percent);
            setVoltage(voltage.percent);
            lcdPrintFlag++;
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = CHECK;
        }
        if(getPinState(pin2)) {
            pin2.buttonState = LOW;
            decPercent();
            setVoltage(voltage.percent);
            printCapacity(voltage.percent);

            digitalWrite(ledPin, HIGH);
        }
        if(getPinState(pin3)) {
            pin3.buttonState = LOW;
            incPercent();
            setVoltage(voltage.percent);
            printCapacity(voltage.percent);

            digitalWrite(ledPin, HIGH);
        }
        digitalWrite(ledPin, LOW);
        break;
    case CHECK:
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "CHECK VALUES");
            delLine(1);
            lcdPrintFlag++;
        }
        if (getSensorValue(analogRead(A7))) {
            delLine(1);
            lcd.print("A7:");
            lcd.print(sensorValue);

            lcd.print(" U:");
            lcd.print(getVoltage(sensorValue));
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = CHARG1;
            sensorValue = 0;
        }
        break;
    case CHARG1:
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);
            lcdPrintFlag++;
            digitalWrite(chargePin, HIGH);
            tempSensorOld = 0;
        }
        // TEMP_SENSOR ---------------------------------------------------------
        sensors.requestTemperatures(); // Send the command to get temperatures
        tempSensor = sensors.getTempCByIndex(0);
        printTempValue("CHARGING T:");

        if(tempSensor > TEMP_CRITICAL) {
            lcdPrintFlag = 0;
            state = COOLING;
            sensorValue = 0;
            digitalWrite(chargePin, LOW);
        }

        if(analogRead(A7) >= voltage.value) {
            lcdPrintFlag = 0;
            state = WAITING;
            sensorValue = 0;
            digitalWrite(chargePin, LOW);
        }

        if (getSensorValue(analogRead(A7))) {
            delLine(1);
            lcd.print("A7:");
            lcd.print(sensorValue);

            lcd.print(" U:");
            lcd.print(getVoltage(sensorValue));
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = CHECK;
            sensorValue = 0;
            digitalWrite(chargePin, LOW);
        }
        break;
    case COOLING:
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);
            lcdPrintFlag++;
            tempSensorOld = 0;
        }
        // TEMP_SENSOR ---------------------------------------------------------
        sensors.requestTemperatures(); // Send the command to get temperatures
        tempSensor = sensors.getTempCByIndex(0);
        printTempValue("COOLING T:");

        if(tempSensor < TEMP_CRITICAL) {
            lcdPrintFlag = 0;
            state = CHARG1;
            sensorValue = 0;
        }

        if (getSensorValue(analogRead(A7))) {
            delLine(1);
            lcd.print("A7:");
            lcd.print(sensorValue);

            lcd.print(" U:");
            lcd.print(getVoltage(sensorValue));
        }
        break;
    case WAITING:
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);
            lcdPrintFlag++;
            tempSensorOld = 0;
        }
        // TEMP_SENSOR ---------------------------------------------------------
        sensors.requestTemperatures(); // Send the command to get temperatures
        tempSensor = sensors.getTempCByIndex(0);
        printTempValue("WAITING T:");

        if (getSensorValue(analogRead(A7))) {
            delLine(1);
            lcd.print("A7:");
            lcd.print(sensorValue);

            lcd.print(" U:");
            lcd.print(getVoltage(sensorValue));
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = CHARG1;
            sensorValue = 0;
        }
        break;
    default:
        break;
    }
}
