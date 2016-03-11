#include "defines.h"

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

void printDate(char *s, int8_t size, const RtcDateTime &dt)
{
    memset(s, 0, size);
    sprintf(s, "%02u/%02u/%04u", dt.Day(), dt.Month(), dt.Year());
}

void printTime(char *s, int8_t size, const RtcDateTime &dt)
{
    memset(s, 0, size);
    sprintf(s, "%02u:%02u:%02u", dt.Second(), dt.Minute(), dt.Hour());
}

void setRunningPrint(uint8_t lineNr, const char *s)
{
    line = lineNr;
    memset(runString, 0, sizeof(runString) / sizeof(runString[0]));
    strcpy(runString, s);
}

void printRunning(int8_t line, uint8_t counter)
{
    lcd.setCursor(0, line);
    lcd.print(runString + counter);
}
// -----------------------------------------------------------------------------
void setDay(uint8_t month, uint8_t flag)
{
    if(dateTime.day > daysInMonth[month])
        dateTime.day = 0;
    else {
        if(flag)
            dateTime.day++;
        else
            dateTime.day--;
    }
}

// -----------------------------------------------------------------------------

float getVoltage(int16_t value)
{
    return value * UFACTOR;
}

/**
 * @brief getSensorValue Die Fktn. reduziert die ADC-Schwingung.
 * @param value
 * @return
 */
int8_t getSensorValue(int16_t value)
{
    if(sensorValue - value > 1 || sensorValue - value < -1)
        sensorValue = value;
    else
        return 0;
    return 1;
}

int8_t getPinState(pin &pin)
{
    pin.reading = digitalRead(pin.buttonPin);   // read the state of the switch into a local variable

    if (pin.reading == H) {                     // If the switch changed, due to noise or pressing
        pin.lastDebounceTime = millis();        // reset the debouncing timer
        pin.lastButtonState = HIGH;
    }
    if ((millis() - pin.lastDebounceTime) > pin.debounceDelay)
        if (pin.lastButtonState) {
            pin.buttonState = HIGH;
            pin.lastButtonState = LOW;
        }
    return pin.buttonState;
}

void incPercent()
{
    switch (voltage.percent) {
    case V60:
        voltage.percent = V70;
        break;
    case V70:
        voltage.percent = V75;
        break;
    case V75:
        voltage.percent = V80;
        break;
    case V80:
        voltage.percent = V85;
        break;
    case V85:
        voltage.percent = V60;
        break;
    default:
        voltage.percent = V60;
        break;
    }
}

void decPercent()
{
    switch (voltage.percent) {
    case V60:
        voltage.percent = V85;
        break;
    case V70:
        voltage.percent = V60;
        break;
    case V75:
        voltage.percent = V70;
        break;
    case V80:
        voltage.percent = V75;
        break;
    case V85:
        voltage.percent = V80;
        break;
    default:
        voltage.percent = V60;
        break;
    }
}

void setVoltage(e_voltage i)
{
    switch (i) {
    case V60:
        voltage.value = 778;
        break;
    case V70:
        voltage.value = 799;
        break;
    case V75:
        voltage.value = 819;
        break;
    case V80:
        voltage.value = 840;
        break;
    case V85:
        voltage.value = 860;
        break;
    default:
        voltage.value = 778;    // 60%
        break;
    }
}

