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

void printTempValue(const char *s, int8_t line = 0)
{
    if(tempSensorOld != tempSensor) {
        tempSensorOld = tempSensor;
        delLine(line);
        lcd.setCursor(0, line);
        lcd.print(s);
        lcd.print(tempSensor);
    }
}

void printDate(char *s, int8_t pos, const RtcDateTime &dt)
{
    sprintf(s + pos, "%02u/%02u/%04u", dt.Day(), dt.Month(), dt.Year());
}

void printTime(char *s, int8_t pos, const RtcDateTime &dt)
{
    sprintf(s + pos, "%02u:%02u", dt.Hour(), dt.Minute());
}

void printDateTime(char *s, int8_t pos, const RtcDateTime &dt)
{
    sprintf(s + pos, "%02u/%02u/%04u %02u:%02u", dt.Day(), dt.Month(), dt.Year(), dt.Hour(), dt.Minute());
}

void setRunningPrint(uint8_t lineNr, const char *s)
{
    line = lineNr;
    counter = 0;
    memset(runString, 0, DEFAULT_STRLEN);
    if(s)
        strcpy(runString, s);
}

//void resetRunning()
//{
//    memset(runString, 0, DEFAULT_STRLEN);
//    counter = 0;
//}

void printRunning(int8_t line, uint8_t counter)
{
    lcd.setCursor(0, line);
    lcd.print(runString + counter);
}
// -----------------------------------------------------------------------------
uint32_t getMin()
{
    return 60;
}

uint32_t getHour()
{
    return getMin() * 60;
}

uint32_t getDay()
{
    return getHour() * 24;
}

uint32_t getMonth(RtcDateTime &dt, int8_t operation = 0)
{
    switch (operation) {
    case -1:
        if(dt.Month() == 1)
            return daysInMonth[11] * getDay();
        else
            return daysInMonth[dt.Month() - 2] * getDay();
        break;
    case 1:
        return daysInMonth[dt.Month() - 1] * getDay();
        break;
    default:
        return getDay() * 30;
        break;
    }
}

uint32_t getYear()
{
    return getDay() * 365;
}

// -----------------------------------------------------------------------------
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
    case V50:
        voltage.value = 758;
        break;
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

float getVoltage(int16_t value)
{
    return value * UFACTOR;
}

float getVoltage(e_voltage i)
{
    switch (i) {
    case V50:
        return 758;
    case V60:
        return 778;
    case V70:
        return 799;
    case V75:
        return 819;
    case V80:
        return 840;
    case V85:
        return 860;
    default:
        return 0;
    }
}
