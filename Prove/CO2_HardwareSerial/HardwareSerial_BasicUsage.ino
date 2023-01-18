#include <Arduino.h>
#include "MHZ19.h"
#include <HardwareSerial.h>

#define RX_PIN 16                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 17                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library

HardwareSerial mySerial(2);                                // On ESP32 we do not require the SoftwareSerial library, since we have 2 USARTS available

unsigned long getDataTimer = 1000;

void setup()
{
    Serial.begin(9600);                                     // Device to serial monitor feedback

    mySerial.begin(BAUDRATE,SERIAL_8N1,RX_PIN,TX_PIN);                               // (Uno example) device to MH-Z19 serial start
    myMHZ19.begin(mySerial);                                // *Serial(Stream) reference must be passed to library begin().

    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
}

void loop()
{
    if (millis() - getDataTimer >= 2000)
    {
        int CO2;

        /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even
        if below background CO2 levels or above range (useful to validate sensor). You can use the
        usual documented command with getCO2(false) */

        CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)

        Serial.print("CO2 (ppm): ");
        Serial.println(CO2);

        int8_t Temp;
        Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
        Serial.print("Temperature (C): ");
        Serial.println(Temp);

        getDataTimer = millis();
    }
}
