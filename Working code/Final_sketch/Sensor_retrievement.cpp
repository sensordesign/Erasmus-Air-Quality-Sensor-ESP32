#include "Sensor_retrievement.h"
#include "DHT.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#define DHTPIN 5
#define DHTTYPE DHT11
DHT DHT1(DHTPIN, DHTTYPE);
/***************************************/
#include <SDS011.h>
float p10, p25;
int err;
SDS011 my_sds;
HardwareSerial port(2);

/***************************************/
#include "MHZ19.h"
#include <HardwareSerial.h>

#define RX_PIN 16                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 17                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)

MHZ19 myMHZ19;                                             // Constructor for library

HardwareSerial mySerial(2);                                // On ESP32 we do not require the SoftwareSerial library, since we have 2 USARTS available

void instruments_setup() {

  Serial.println(F("DHTxx test!"));
  DHT1.begin();

  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);                            // (Uno example) device to MH-Z19 serial start
  myMHZ19.begin(mySerial);                                // *Serial(Stream) reference must be passed to library begin().

  myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))

  my_sds.begin(&port);

}

double temperature() {
  return DHT1.readTemperature();
}
double co2() {
  return readco2();
}
double humidity() {
  return DHT1.readHumidity();
}
int pm2_5() {
  return p25;
}
int pm10() {
  return p10;
}
int sds_loop() {

  err = my_sds.read(&p25, &p10);
  if (!err) {
    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
  }

  return p10;
  return p25;
}

int readco2() {

  int CO2;
  CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
  Serial.print("CO2 (ppm): ");
  Serial.println(CO2);
  int8_t Temp;
  Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
  Serial.print("Temperature (C): ");
  Serial.println(Temp);

  return CO2;
}
