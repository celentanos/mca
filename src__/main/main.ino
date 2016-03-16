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

    // init LED-pin ------------------------------------------------------------
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    // init Charge-pin ---------------------------------------------------------
    pinMode(chargePin, OUTPUT);
    digitalWrite(chargePin, LOW);

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
            setVoltage(voltage.percent);
            printCapacity(voltage.percent);
        }
        if(getPinState(pin3)) {
            pin3.buttonState = LOW;
            incPercent();
            digitalWrite(ledPin, HIGH);
            setVoltage(voltage.percent);
            printCapacity(voltage.percent);
        }
        digitalWrite(ledPin, LOW);
        break;
    case RTC_DATE:
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);

            now = Rtc.GetDateTime();
            // init dateTime struct
            dateTime.year = now.Year();
            dateTime.month = now.Month();
            dateTime.day = now.Day();

            // print current dateTime
            char sTemp[20];
            strcpy(sTemp, "CURRENT DATE:");
            printDate(sTemp, strlen(sTemp), now);
            setRunningPrint(0, sTemp);
            printLine(0, sTemp);
            memset(sTemp, 0, strlen(sTemp));
            strcpy(sTemp, "SET Y:");
            printDate(sTemp, strlen(sTemp), dateTime);
            printLine(1, sTemp);

            lcdPrintFlag++;
        }
        char sTemp[20];
        switch (stateDateTime) {
        case YEAR:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                now = Rtc.GetDateTime();
                if(dateTime.year > now.Year())
                    dateTime.year--;
                strcpy(sTemp, "SET Y:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                dateTime.year++;
                strcpy(sTemp, "SET Y:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                strcpy(sTemp, "SET M:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
                stateDateTime = MONTH;
            }
            break;
        case MONTH:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                setMonth(0);
                strcpy(sTemp, "SET M:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                setMonth(1);
                strcpy(sTemp, "SET M:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
                strcpy(sTemp, "SET D:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
                stateDateTime = DAY;
            }
            break;
        case DAY:
            if(getPinState(pin2)) {
                pin2.buttonState = LOW;
                setDay(0);
                strcpy(sTemp, "SET D:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
            }
            if(getPinState(pin3)) {
                pin3.buttonState = LOW;
                setDay(1);
                strcpy(sTemp, "SET D:");
                printDate(sTemp, strlen(sTemp), dateTime);
                printLine(1, sTemp);
            }
            if(getPinState(pin1)) {
                pin1.buttonState = LOW;
//                strcpy(sTemp, "SET H:");
//                printTime(sTemp, strlen(sTemp), dateTime);
//                printLine(1, sTemp);
                setRunningPrint(0, 0);
                lcdPrintFlag = 0;
                stateDateTime = YEAR;
                state = RTC_TIME;
            }
            break;
        default:
            break;
        }
        break;
    case RTC_TIME:
        now = Rtc.GetDateTime();
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);

            // init dateTime struct
            dateTime.hour = now.Hour();
            dateTime.min_temp = now.Minute();

            // print current dateTime
            char sTemp[20];
            strcpy(sTemp, "CURRENT TIME:");
            printTime(sTemp, strlen(sTemp), now);
            setRunningPrint(0, sTemp);
            printLine(0, sTemp);
            memset(sTemp, 0, strlen(sTemp));
            strcpy(sTemp, "SET H:");
            printTime(sTemp, strlen(sTemp), dateTime);
            printLine(1, sTemp);

            lcdPrintFlag++;
        }
        if(dateTime.hour < now.Hour()) {                // Stunde aktualisieren
            setHour(1);
            char sTemp[20];
            strcpy(sTemp, "SET H:");
            printTime(sTemp, strlen(sTemp), dateTime);
            printLine(1, sTemp);
        }
        if(dateTime.min_temp != now.Minute()) {         // Minute aktualisieren
            dateTime.min_temp = now.Minute();
            setRunningPrint(0, 0);
            char sTemp[20];
            strcpy(sTemp, "CURRENT TIME:");
            printTime(sTemp, strlen(sTemp), now);
            setRunningPrint(0, sTemp);
            printLine(0, sTemp);
        }
        if(getPinState(pin2)) {
            pin2.buttonState = LOW;
            setHour(0);
            strcpy(sTemp, "SET H:");
            printTime(sTemp, strlen(sTemp), dateTime);
            printLine(1, sTemp);
        }
        if(getPinState(pin3)) {
            pin3.buttonState = LOW;
            setHour(1);
            strcpy(sTemp, "SET H:");
            printTime(sTemp, strlen(sTemp), dateTime);
            printLine(1, sTemp);
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            setRunningPrint(0, 0);
            lcdPrintFlag = 0;
            state = RTC_CHECK;
        }
        break;
    case RTC_CHECK:
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);

            char sTemp[20];
            printDateTime(sTemp, strlen(sTemp), dateTime);
            printLine(0, sTemp);
            printLine(1, "YES           NO");
            lcdPrintFlag++;
        }
        if(getPinState(pin1)) {
            pin1.buttonState = LOW;
            lcdPrintFlag = 0;
            state = RTC_CHECK;
        }
        if(getPinState(pin3)) {
            pin3.buttonState = LOW;
            lcdPrintFlag = 0;
            state = RTC_DATE;
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
            state = CHARGE1;
            sensorValue = 0;
        }
        break;
    case CHARGE1:
        now = Rtc.GetDateTime();
        if(!lcdPrintFlag) {
            delLine(0);
            delLine(1);

            digitalWrite(chargePin, HIGH);
            digitalWrite(chargePinMode, LOW);

            lcdPrintFlag++;
        }
        // TODO
//        if(dateTime.year == now.Year() && dateTime.month == now.Month() && dateTime.day == now.Day())

        break;
    case CHARGE2:
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
            state = CHARGE2;
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
            state = CHARGE2;
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
