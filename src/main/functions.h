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
    case V50:
        voltage.percent = V60;
        break;
    case V60:
        voltage.percent = V70;
        break;
    case V70:
        voltage.percent = V80;
        break;
    case V80:
        voltage.percent = V90;
        break;
    case V90:
        voltage.percent = V100;
        break;
    case V100:
        voltage.percent = V50;
        break;
    default:
        voltage.percent = V50;
        break;
    }
}

void decPercent()
{
    switch (voltage.percent) {
    case V50:
        voltage.percent = V100;
        break;
    case V60:
        voltage.percent = V50;
        break;
    case V70:
        voltage.percent = V60;
        break;
    case V80:
        voltage.percent = V70;
        break;
    case V90:
        voltage.percent = V80;
        break;
    case V100:
        voltage.percent = V90;
        break;
    default:
        voltage.percent = V50;
        break;
    }
}

void setVoltage(e_voltage i)
{
    switch (i) {
    case V50:
        voltage.value = 788;
        break;
    case V60:
        voltage.value = 803;
        break;
    case V70:
        voltage.value = 817;
        break;
    case V80:
        voltage.value = 831;
        break;
    case V90:
        voltage.value = 846;
        break;
    case V100:
        voltage.value = 860;
        break;
    default:
        voltage.value = 788;    // 50%
        break;
    }
}

