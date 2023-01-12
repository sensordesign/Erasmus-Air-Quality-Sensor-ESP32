#include "Sensor_retrievement.h"
#include "DHT.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#define DHTPIN 5
#define DHTTYPE DHT11
DHT DHT1(DHTPIN, DHTTYPE);
#define D3 3
#define D4 21

SoftwareSerial co2Serial(D3, D4);  // define MH-Z19 RX D3 (GPIO3) and TX D4 (GPIO21)

#include <Sds011.h>
#define SDS_PIN_RX 15
#define SDS_PIN_TX 13

#ifdef ESP32
HardwareSerial& serialSDS(Serial2);
Sds011Async< HardwareSerial > sds011(serialSDS);
#else
SoftwareSerial serialSDS;
Sds011Async< SoftwareSerial > sds011(serialSDS);
#endif

constexpr int pm_tablesize = 20;
int pm25_table[pm_tablesize];
int pm10_table[pm_tablesize];

bool is_SDS_running = true;

int PM25;
int PM10;

void instruments_setup() {
  co2Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  DHT1.begin();
}

void start_SDS() {
  Serial.println("Start wakeup SDS011");

  if (sds011.set_sleep(false)) {
    is_SDS_running = true;
  }

  Serial.println("End wakeup SDS011");
}

void stop_SDS() {
  Serial.println("Start sleep SDS011");

  if (sds011.set_sleep(true)) {
    is_SDS_running = false;
  }

  Serial.println("End sleep SDS011");
}

void sds_setup() {

#ifdef ESP32
  serialSDS.begin(9600, SERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX);
  delay(100);
#else
  serialSDS.begin(9600, SWSERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX, false, 192);
#endif

  Serial.println("SDS011 start/stop and reporting sample");

  Sds011::Report_mode report_mode;
  if (!sds011.get_data_reporting_mode(report_mode)) {
    Serial.println("Sds011::get_data_reporting_mode() failed");
  }
  if (Sds011::REPORT_ACTIVE != report_mode) {
    Serial.println("Turning on Sds011::REPORT_ACTIVE reporting mode");
    if (!sds011.set_data_reporting_mode(Sds011::REPORT_ACTIVE)) {
      Serial.println("Sds011::set_data_reporting_mode(Sds011::REPORT_ACTIVE) failed");
    }
  }
}

double temperature() {
  return DHT1.readTemperature();
}
double co2() {
  return readCO2UART();
}
double humidity() {
  return DHT1.readHumidity();
}
int pm2_5() {
  return PM25;
}
int pm10() {
  return PM10;
}
int sds_loop() {
  // Per manufacturer specification, place the sensor in standby to prolong service life.
  // At an user-determined interval (here 210s down plus 30s duty = 4m), run the sensor for 30s.
  // Quick response time is given as 10s by the manufacturer, thus the library drops the
  // measurements obtained during the first 10s of each run.

  constexpr uint32_t down_s = 210;

  stop_SDS();
  Serial.print("stopped SDS011 (is running = ");
  Serial.print(is_SDS_running);
  Serial.println(")");

  uint32_t deadline = millis() + down_s * 1000;
  while (static_cast<int32_t>(deadline - millis()) > 0) {
    delay(1000);
    Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
    sds011.perform_work();
  }

  constexpr uint32_t duty_s = 30;

  start_SDS();
  Serial.print("started SDS011 (is running = ");
  Serial.print(is_SDS_running);
  Serial.println(")");

  sds011.on_query_data_auto_completed([](int n) {
    Serial.println("Begin Handling SDS011 query data");
    int PM25;
    int PM10;
    Serial.print("n = "); Serial.println(n);
    if (sds011.filter_data(n, pm25_table, pm10_table, PM25, PM10) &&
        !isnan(PM10) && !isnan(PM25)) {
      Serial.print("PM10: ");
      Serial.println(float(PM10) / 10);
      Serial.print("PM2.5: ");
      Serial.println(float(PM25) / 10);
    }
    Serial.println("End Handling SDS011 query data");
  });

  if (!sds011.query_data_auto_async(pm_tablesize, pm25_table, pm10_table)) {
    Serial.println("measurement capture start failed");
  }

  deadline = millis() + duty_s * 1000;
  while (static_cast<int32_t>(deadline - millis()) > 0) {
    delay(1000);
    Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
    sds011.perform_work();
  }
  return PM10;
  return PM25;
}

int readCO2UART() {
  byte cmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  char response[9];
  Serial.println("Sending CO2 request...");
  co2Serial.write(cmd, 9); //request PPM CO2

  // clear the buffer
  memset(response, 0, 9);
  int i = 0;
  while ((co2Serial.available() == 0) && (i < 20)) {

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
