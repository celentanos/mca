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

    // init BUTTONS ------------------------------------------------------------
    pinMode(pin1.buttonPin = 2, INPUT_PULLUP);  // setup pin1
    pinMode(pin2.buttonPin = 3, INPUT_PULLUP);  // setup pin2
    pinMode(pin3.buttonPin = 4, INPUT_PULLUP);  // setup pin3
    pinMode(pin4.buttonPin = 5, INPUT_PULLUP);  // setup pin4

    // init LED-pin ------------------------------------------------------------
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // init Charge-pin ---------------------------------------------------------
    pinMode(chargePin, OUTPUT);
    digitalWrite(chargePin, HIGH);
    pinMode(chargePinMode, OUTPUT);
    digitalWrite(chargePinMode, HIGH);

    // init RTC ----------------------------------------------------------------
    Rtc.Begin();

    if (!Rtc.IsDateTimeValid()) {
        // Common Cuases:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
        printLine(0, "RTC lost confidence in the DateTime!");
        while (1);  // TODO: STOP --------------------------------------
    }
    if (!Rtc.GetIsRunning()) {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    // Serial ------------------------------------------------------------------
    Serial.begin(9600);
}

void loop()
{

    switch (state) {
    case BAT_CAPACITY:
        // tief antladen -------------------------------------------------------
        if(analogRead(A7) <= getVDigits(VLOW)) {
            lcdPrintFlag = 0;
            state = BAT_LOW_VOLTAGE;
        }
        if(checkBatPresence()) // kein Akku eingesetzt -------------------------
            break;
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, S_CHARGE_CAP);
            printCapacity(voltage.percent);
            setVDigits(voltage.percent);
            lcdPrintFlag++;
        }
        if(getPinState(pin2)) {
            pin2.buttonState = LOW;
            decPercent();
            digitalWrite(ledPin, HIGH);
            setVDigits(voltage.percent);
            printCapacity(voltage.percent);
        }
        if(getPinState(pin3)) {
            pin3.buttonState = LOW;
            incPercent();
            digitalWrite(ledPin, HIGH);
            setVDigits(voltage.percent);
            printCapacity(voltage.percent);
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            digitalWrite(chargePin, LOW);
            state = RTC_DATE;
            stateDateTime = DATE_YEAR;
        }
        digitalWrite(ledPin, LOW);
        break;
    case BAT_LOW_VOLTAGE:
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "LOW BAT VOLTAGE");
            delLine(1);
            printLine(1, "REPLACE BAT & OK");
            lcdPrintFlag++;
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = BAT_CAPACITY;
        }
        break;
    case BAT_NO_BAT:
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "NO BAT DETECTED");
            delLine(1);
            printLine(1, "PLACE BAT & OK");
            lcdPrintFlag++;
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = BAT_CAPACITY;
        }
        break;
    case RTC_DATE:
        switch (stateDateTime) {
        case DATE_YEAR:
            if(!lcdPrintFlag) {
                delLine(0);
                delLine(1);

                now = Rtc.GetDateTime();
                // init dateTime struct
                if(timeFlag) {
                    timeMy = now;
                    timeFlag = 0;
                }

                // print current dateTime
                char sTemp[20];
                strcpy(sTemp, S_CURRENT_DATE);
                printDate(sTemp, strlen(sTemp), now);
                setRunningPrint(0, sTemp);
                printLine(0, sTemp);
                memset(sTemp, 0, strlen(sTemp));
                strcpy(sTemp, S_SET_Y);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
                lcdPrintFlag++;
            }
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now = Rtc.GetDateTime();
                if(timeMy.Year() > now.Year())
                    timeMy -= getYear();
                char sTemp[20];
                strcpy(sTemp, S_SET_Y);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                timeMy += getYear();
                char sTemp[20];
                strcpy(sTemp, S_SET_Y);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                char sTemp[20];
                strcpy(sTemp, S_SET_M);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
                stateDateTime = DATE_MONTH;
            }
            break;
        case DATE_MONTH:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                if(timeMy.TotalSeconds() + getHour() > now.TotalSeconds() + getMonth(now, 1))
                    timeMy -= getMonth(timeMy, -1);
                char sTemp[20];
                strcpy(sTemp, S_SET_M);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                timeMy += getMonth(timeMy, 1);
                char sTemp[20];
                strcpy(sTemp, S_SET_M);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                char sTemp[20];
                strcpy(sTemp, S_SET_D);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
                stateDateTime = DATE_DAY;
            }
            break;
        case DATE_DAY:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                if(timeMy.TotalSeconds() + getMin() > now.TotalSeconds() + getDay())
                    timeMy -= getDay();
                char sTemp[20];
                strcpy(sTemp, S_SET_D);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                timeMy += getDay();
                char sTemp[20];
                strcpy(sTemp, S_SET_D);
                printDate(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                lcdPrintFlag = 0;
                stateDateTime = DATE_HOUR;
            }
            break;
        case DATE_HOUR:
            now = Rtc.GetDateTime();
            if(!lcdPrintFlag) {
                setRunningPrint(0, 0);
                delLine(0);
                delLine(1);

                now_temp = now;

                // print current dateTime
                char sTemp[DEFAULT_STRLEN];
                strcpy(sTemp, S_CURRENT_TIME);
                printTime(sTemp, strlen(sTemp), now);
                setRunningPrint(0, sTemp);
                printLine(0, sTemp);
                memset(sTemp, 0, DEFAULT_STRLEN);
                strcpy(sTemp, S_SET_H);
                printTime(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);

                lcdPrintFlag++;
            }
            // Stunde aktualisieren --------------------------------------------
            if(now_temp.TotalSeconds() + getHour() < now.TotalSeconds()) {
                now_temp += getHour();
                timeMy += getHour();
                char sTemp[DEFAULT_STRLEN];
                strcpy(sTemp, S_SET_H);
                printTime(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            // Minute aktualisieren --------------------------------------------
            if(now_temp.TotalSeconds() + getMin() < now.TotalSeconds()) {
                now_temp += getMin();
                timeMy += getMin();
                setRunningPrint(0, 0);
                char sTemp[DEFAULT_STRLEN];
                strcpy(sTemp, S_CURRENT_TIME);
                printTime(sTemp, strlen(sTemp), now);
                setRunningPrint(0, sTemp);
                printLine(0, sTemp);
            }
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                if(timeMy.TotalSeconds() + getMin() > now.TotalSeconds() + getHour())
                    timeMy -= getHour();
                char sTemp[20];
                strcpy(sTemp, S_SET_H);
                printTime(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                timeMy += getHour();
                char sTemp[20];
                strcpy(sTemp, S_SET_H);
                printTime(sTemp, strlen(sTemp), timeMy);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                setRunningPrint(0, 0);
                lcdPrintFlag = 0;
                stateDateTime = DATE_CHECK;
            }
            break;
        case DATE_CHECK:
            if(!lcdPrintFlag) {
                delLine(0);
                delLine(1);

                char sTemp[20];
                printDateTime(sTemp, 0, timeMy);
                printLine(0, sTemp);
                printLine(1, S_YES_NO);
                lcdPrintFlag++;
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                lcdPrintFlag = 0;
                state = BAT_CHECK;
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                lcdPrintFlag = 0;
                stateDateTime = DATE_YEAR;
            }
            if(getPinState(pin4)) {
                pin4.buttonState = LOW;
                lcdPrintFlag = 0;
                state = RTC_SET;
                stateDateTime = DATE_YEAR;
            }
            break;
        default:
            lcd.clear();
            printLine(0, "stateDateTime");
            break;
        }
        break;
    case RTC_SET:
        switch (stateDateTime) {
        case DATE_YEAR:
            if(!lcdPrintFlag) {
                setRunningPrint(0, 0);
                delLine(0);
                delLine(1);
                printLine(0, S_SET_DATE_Y);
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
                lcdPrintFlag++;
            }
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now -= getYear();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                now += getYear();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                delLine(0);
                char sTemp[20];
                strcpy(sTemp, S_SET_DATE_M);
                printLine(0, sTemp);
                stateDateTime = DATE_MONTH;
            }
            break;
        case DATE_MONTH:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now -= getMonth(now, -1);
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                now += getMonth(now, 1);
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                delLine(0);
                char sTemp[20];
                strcpy(sTemp, S_SET_DATE_D);
                printLine(0, sTemp);
                stateDateTime = DATE_DAY;
            }
            break;
        case DATE_DAY:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now -= getDay();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                now += getDay();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                delLine(0);
                char sTemp[20];
                strcpy(sTemp, S_SET_DATE_H);
                printLine(0, sTemp);
                stateDateTime = DATE_HOUR;
            }
            break;
        case DATE_HOUR:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now -= getHour();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                now += getHour();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                delLine(0);
                char sTemp[20];
                strcpy(sTemp, S_SET_DATE_MIN);
                printLine(0, sTemp);
                stateDateTime = DATE_MIN;
            }
            break;
        case DATE_MIN:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now -= getMin();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                now += getMin();
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                delLine(0);
                char sTemp[20];
                printDateTime(sTemp, 0, now);
                printLine(0, sTemp);
                memset(sTemp, 0, strlen(sTemp));
                strcpy(sTemp, S_YES_NO);
                printLine(1, sTemp);
                stateDateTime = DATE_CHECK;
            }
            break;
        case DATE_CHECK:
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                lcdPrintFlag = 0;
                state = RTC_DATE;
                stateDateTime = DATE_YEAR;
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                lcdPrintFlag = 0;
                state = RTC_SET;
                stateDateTime = DATE_YEAR;
            }
            break;
        }
        break;
    case BAT_CHECK:
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
            state = CHARGE1;
            sensorValue = 0;
        }
        break;
    case CHARGE1:
        if(checkBatPresence()) // kein Akku eingesetzt -------------------------
            break;
        now = Rtc.GetDateTime();
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "CHARGE1");
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
        // Aufbewahrung
        if(analogRead(A7) < getVDigits(V50)) {
            printLine(0, "CHARGE1 Aufbewahrung");
            digitalWrite(chargePin, HIGH);
            digitalWrite(chargePinMode, HIGH);
        }
        // Pause
        if(analogRead(A7) >= getVDigits(V60)) {
            printLine(0, "CHARGE1 Pause");
            digitalWrite(chargePin, LOW);
            digitalWrite(chargePinMode, HIGH);
        }
        // CHARGE2
        if(timeMy.TotalSeconds() < now.TotalSeconds() + getDay()) {
            delay(1000);
            lcdPrintFlag = 0;
            state = CHARGE2;
        }
        break;
    case CHARGE2:
        if(checkBatPresence()) // kein Akku eingesetzt -------------------------
            break;
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "CHARGE2");
            delLine(1);
            lcdPrintFlag++;
            tempSensorOld = 0;
            digitalWrite(chargePin, HIGH);
            digitalWrite(chargePinMode, LOW);
        }
        // TEMP_SENSOR ---------------------------------------------------------
        sensors.requestTemperatures();  // Send the command to get temperatures
        tempSensor = sensors.getTempCByIndex(0);
        printTempValue("CHARGING T:");

        if (getSensorValue(analogRead(A7))) {
            delLine(1);
            lcd.print("A7:");
            lcd.print(sensorValue);
            lcd.print(" U:");
            lcd.print(getVoltage(sensorValue));
        }
        if(tempSensor > TEMP_CRITICAL) {
            lcdPrintFlag = 0;
            state = COOLING;
            sensorValue = 0;
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = BAT_CHECK;
            sensorValue = 0;
        }
        if(analogRead(A7) >= voltage.value) {
            delay(1000);
            lcdPrintFlag = 0;
            sensorValue = 0;
            state = CHARGE3;
        }
        break;
    case COOLING:
        if(checkBatPresence()) // kein Akku eingesetzt -------------------------
            break;
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);
            lcdPrintFlag++;
            tempSensorOld = 0;
            digitalWrite(chargePin, LOW);
        }
        // TEMP_SENSOR ---------------------------------------------------------
        sensors.requestTemperatures();  // Send the command to get temperatures
        tempSensor = sensors.getTempCByIndex(0);
        printTempValue("COOLING T:");

        if (getSensorValue(analogRead(A7))) {
            delLine(1);
            lcd.print("A7:");
            lcd.print(sensorValue);

            lcd.print(" U:");
            lcd.print(getVoltage(sensorValue));
        }
        if(tempSensor < TEMP_CRITICAL) {
            lcdPrintFlag = 0;
            state = CHARGE2;
            sensorValue = 0;
        }
        break;
    case CHARGE3:
        if(checkBatPresence()) // kein Akku eingesetzt -------------------------
            break;
        if(!lcdPrintFlag) {
            delLine(0);
            printLine(0, "CHARGE3");
            delLine(1);
            lcdPrintFlag++;
            tempSensorOld = 0;
            digitalWrite(chargePin, HIGH);
            digitalWrite(chargePinMode, HIGH);
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
            state = BAT_CHECK;
            sensorValue = 0;
        }
        if(analogRead(A7) >= voltage.value) {
            delay(1000);
//            lcdPrintFlag = 0;
//            sensorValue = 0;
//            state = WAITING;    // TODO: WAITING
        }
        break;
    case WAITING:
        checkBatPresence(); // kein Akku eingesetzt ----------------------------
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);
            lcdPrintFlag++;
            tempSensorOld = 0;
            digitalWrite(chargePin, LOW);
        }
        // Aufbewahrung
        if(analogRead(A7) < getVDigits(V50)) {
            lcdPrintFlag = 0;
            state = CHARGE1;
        }
        // TEMP_SENSOR ---------------------------------------------------------
        sensors.requestTemperatures();  // Send the command to get temperatures
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
            state = CHARGE1;
            sensorValue = 0;
        }
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
