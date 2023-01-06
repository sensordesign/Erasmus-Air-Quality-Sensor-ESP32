//****************************SENSORS****************************//
// Include Libraries for sensors and sensor readings
//#include <WEMOS_SHT3X.h>
//#include "SdsDustSensor.h"
//#include <DHT.h>
#include "src/sensor_retrievement/Sensor_retrievement.h"
//#include <SoftwareSerial.h>


#include "src/config/config.h"

//****************************OTA UPDATES****************************//
// Over the air updates through a web page
#include "src/ota_updates/OTA_updates.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>


//****************************TIME****************************//
// Include Libraries for time and sleeping
#include "src/timeawake/timeawake.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
int deepsleepTime = 0;

//****************************WIFI****************************//
// This should be substituted with ESP_Now On Satellites devices so to have just one Esp connected to wifi// MAYBE NOT
// Include the libraries for wifi
#include <WiFi.h>
// WiFi AP SSID
char *WIFI_SSID = wifi_name();
// WiFi password
char *WIFI_PASSWORD = wifi_PWD();


//****************************INFLUX DB****************************//
// Include the libraries
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
// INFLUXDB Setups where to write
char *INFLUXDB_URL = iNFLUXDB_URL();
char *INFLUXDB_TOKEN = iNFLUXDB_TOKEN();
char *INFLUXDB_ORG = iNFLUXDB_ORG();
char *INFLUXDB_BUCKET = iNFLUXDB_BUCKET();
// Time zone info
#define TZ_INFO "UTC1"
// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
// Declare Data point
Point sensor_readings("measurements");

void setup() {
  Serial.begin(115200);
  // Setup wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  timesetup();


  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  // ... code in setup() from Initialize Client

  // Add tags to the data point
  sensor_readings.addTag("device", "Esp32-Kri-Prova");
  sds_setup();
  setup_ota();
}
void loop() {
  loop_ota();
  // Clear fields for reusing the point. Tags will remain the same as set above.
  sensor_readings.clearFields();

  sensor_readings.addField("Temperature", temperature()); /*Get Data from a class which retrieves data from the temperature sensor*/
  sensor_readings.addField("co2", co2());                 /*Get Data from a class which retrieves data from the co2 sensor*/
  sensor_readings.addField("humidity", humidity());       /*Get Data from a class which retrieves data from the humidity sensor*/
  sds_setup();
  sensor_readings.addField("pm2_5", pm2_5()); /*Get Data from a class which retrieves data from the PM2_5 sensor*/
  sensor_readings.addField("pm10", pm10());   /*Get Data from a class which retrieves data from the pm10 sensor*/
  sensor_readings.addField("Limite PM10", 40.00);
  sensor_readings.addField("Limite PM2.5", 25.00);
  sensor_readings.addField("Deepsleep", getdeepsleep());


  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor_readings.toLineProtocol());

  // Check WiFi connection and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor_readings)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  if (wantdeepSleep()) {
    Serial.print("Waiting ");
    Serial.print(getdeepsleep());
    Serial.println(" Second");
    delay(getdeepsleep() * 1000);
  } else
    delay(5000);
}
