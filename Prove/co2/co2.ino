#include <Arduino.h>
#include <SoftwareSerial.h>
#define D3 3 //tested pins for esp32
#define D4 21

SoftwareSerial co2Serial(D3, D4);  // define MH-Z19 RX D3 (GPIO3) and TX D4 (GPIO21)

void setup() {
  // put your setup code here, to run once:
  co2Serial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(readCO2UART());
}

int readCO2UART() {
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char response[9];
  Serial.println("Sending CO2 request...");
  co2Serial.write(cmd, 9); //request PPM CO2
  
  // clear the buffer
  memset(response, 0, 9);
  int i = 0;
  while (co2Serial.available() == 0) {
  
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
  return checksum;}
