#include "Sensor_retrievement.h"
#include <WEMOS_SHT3X.h>
#include <DHT.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define D3 0
#define D4 2

SoftwareSerial co2Serial(D3, D4);  // define MH-Z19 RX TX D3 (GPIO0) and D4 (GPIO2)


const int wakeupDelay = 15000;
const double offset = 3.00;

void instruments_setup() {
    co2Serial.begin(9600);
}

double pm2__5, pm_10;

void sds_setup() {
}

double temperature() {
  return dht.readTemperature() - offset;
}
double co2() {
  return readCO2UART();
}
double humidity() {
  return dht.readHumidity();
}
double pm2_5() {
  return 0;
}
double pm10() {
  return 0;
}
int readCO2UART() {
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char response[9];
  Serial.println("Sending CO2 request...");
  co2Serial.write(cmd, 9); //request PPM CO2
  
  // clear the buffer
  memset(response, 0, 9);
  int i = 0;
  while (co2Serial.available() == 0 && i<10) {
  
       Serial.print("Waiting for response ");
       Serial.print(i);
        Serial.println(" s");
    delay(1000);
    i++;
  }
  if (co2Serial.available() > 0) {
    co2Serial.readBytes(response, 9);
  
  }
  // print out the response in hexa
  for (int i = 0; i < 9; i++) {
    Serial.print(String(response[i], HEX));
    Serial.print("   ");
  }
  Serial.println("");
  // checksum
  byte check = getCheckSum(response);
  if (response[8] != check) {
    Serial.println("Checksum not OK!");
    Serial.print("Received: ");
    Serial.println(response[8]);
    Serial.print("Should be: ");
    Serial.println(check);
  }
  // ppm
  int ppm_uart = 256 * (int)response[2] + response[3];
  Serial.print("UART CO2 PPM: ");
  Serial.println(ppm_uart);
  // temp
  byte temp = response[4] - 40;
  Serial.print("Sensor Temperature: ");
  Serial.println(temp);
  // status
  byte status = response[5];
  Serial.print("Status: ");
  Serial.println(status);
  if (status == 0x40) {
    Serial.println("Status OK");
  }
  return ppm_uart;
}
byte getCheckSum(char *packet) {
  byte i;
  unsigned char checksum = 0;
  for (i = 1; i < 8; i++) {
    checksum += packet[i];
  }
  checksum = 0xff - checksum;
  checksum += 1;
  return checksum;
}