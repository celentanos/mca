#include "functions.h"

// MAIN ########################################################################

void setup()
{
    // init Temp-Sensor --------------------------------------------------------
    sensors.begin();                            // Start up the library
    sensors.requestTemperatures();              // Send the command to get temperatures

    // init LCD ----------------------------------------------------------------
    lcd.init();                                 // initialize the lcd
    lcd.setBacklight(1);

    // init RTC ----------------------------------------------------------------
    Rtc.Begin();

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
            state = RTC_DATE;
        }
        if(getPinState(pin2)) {
            pin2.buttonState = LOW;
            decPercent();
            digitalWrite(ledPin, HIGH);
        }
        if(getPinState(pin3)) {
            pin3.buttonState = LOW;
            incPercent();
            digitalWrite(ledPin, HIGH);
        }
        setVoltage(voltage.percent);
        printCapacity(voltage.percent);
        digitalWrite(ledPin, LOW);
        break;
    case RTC_DATE:
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);
            if (!Rtc.IsDateTimeValid()) {
                // Common Cuases:
                //    1) first time you ran and the device wasn't running yet
                //    2) the battery on the device is low or even missing

                printLine(0, "RTC lost confidence in the DateTime!");

                // following line sets the RTC to the date & time this sketch was compiled
                // it will also reset the valid flag internally unless the Rtc device is
                // having an issue
            }
            if (!Rtc.GetIsRunning()) {
                Serial.println("RTC was not actively running, starting now");
                Rtc.SetIsRunning(true);
            }
            RtcDateTime now = Rtc.GetDateTime();
            char sDate[20];
            printDate(sDate, sizeof(sDate) / sizeof(sDate[0]), now);
            char sTemp[20];
            strcpy(sTemp, "CURRENT DATE:");
            strcpy(sTemp + strlen(sTemp), sDate);
            setRunningPrint(0, sTemp);
            printLine(0, sTemp);
            lcdPrintFlag++;
        }
        switch (stateDateTime) {
        case DAY:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                dateTime.day++;
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                dateTime.day--;
            }
            break;
        default:
            break;
        }
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
            state = CHARG2;
            sensorValue = 0;
        }
        break;
    case CHARG2:
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
            state = CHARG2;
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
            state = CHARG2;
            sensorValue = 0;
        }
        break;
    default:
        break;
    }
    if(millis() - currentMs > 750) {
        currentMs = millis();
        if(strlen(runString) > LINE_LENGTH) {
            counter++;
            if(counter + LINE_LENGTH > strlen(runString))
                counter = 0;
            printRunning(line, counter);
        }
    }
}
