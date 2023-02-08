//MHZ-19B
#include <HardwareSerial.h>
#define RX_PIN 26                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 27                                         // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)#include <Arduino.h>
#include "MHZ19.h"

MHZ19 myMHZ19;                                             // Constructor for library
#if defined(ESP32)
HardwareSerial mySerial(1);                                // On ESP32 we do not require the SoftwareSerial library, since we have 2 USARTS available
#else
#include <SoftwareSerial.h>                                //  Remove if using HardwareSerial or non-uno compatible device
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
#endif
unsigned long getDataTimer;
unsigned long preheat_time = 10000;
//unsigned long preheat_time = 60000;
int CO2;

/******************************************************/
//SDS011

#include <SDS011.h>
float p10, p25;
int err;
SDS011 my_sds;
#ifdef ESP32
HardwareSerial port(2);
#endif

/******************************************************/
//DHT11

#include "DHT.h"
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);                                     // Device to serial monitor feedback
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);                         // (Uno example) device to MH-Z19 serial start
  myMHZ19.begin(mySerial);                                // *Serial(Stream) reference must be passed to library begin().
  myMHZ19.setRange(5000);
  myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
  my_sds.begin(&port);
  //Serial.println(F("DHTxx test!"));
  dht.begin();
}

void loop() {

  if (millis() - getDataTimer >= preheat_time) {
    
    err = my_sds.read(&p25, &p10);
    if (!err) {
      Serial.print("P2.5: " + String(p25));
      Serial.println("  P10:  " + String(p10));
    }

    /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even
      if below background CO2 levels or above range (useful to validate sensor). You can use the
      usual documented command with getCO2(false) */
    delay(500);
    CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
    Serial.print("CO2 (ppm): ");
    Serial.print(CO2);
    int8_t Temp;
    Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
    Serial.print(" Temperature (C): ");
    Serial.println(Temp);

    double h = dht.readHumidity();
    double t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    // Compute heat index in Celsius (isFahreheit = false)
    //float hic = dht.computeHeatIndex(t, h, false);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));
    //Serial.print(F("Heat index: "));
    //Serial.print(hic);
    //Serial.println(F("°C "));
    Serial.println("");
    Serial.println("-----------------------");
    Serial.println("");

    getDataTimer = millis();
  }
}
