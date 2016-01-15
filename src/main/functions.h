#include "defines.h"

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

