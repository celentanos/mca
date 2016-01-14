#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

// MAIN ########################################################################

void setup()
{
    lcd.init();                                 // initialize the lcd
    lcd.setBacklight(1);

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
    case START:
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
            state = PROCESS;
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
    case PROCESS:
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "check values");
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
            state = WATCH;
            sensorValue = 0;
        }
        break;
    case WATCH:
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "charging");
            delLine(1);
            lcdPrintFlag++;
            digitalWrite(chargePin, HIGH);
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
            state = PROCESS;
            sensorValue = 0;
            digitalWrite(chargePin, LOW);
        }
        break;
    default:
        break;
    }
}
