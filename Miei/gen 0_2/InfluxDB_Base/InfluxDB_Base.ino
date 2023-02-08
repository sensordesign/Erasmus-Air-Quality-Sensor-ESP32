#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


#define WIFI_SSID "Linkem2.4GHz_8B464B"

#define WIFI_PASSWORD "hgmmjksj"
#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "TcfzPq142TWxAC7D9cjkcza0KWNvb5HU9DrwDYg6zX0L4x6hs9Bft8h077KJHpVdiz9kDHEjAcUzcNNWqomj9g=="
#define INFLUXDB_ORG "2e1bcae08357dacf"
#define INFLUXDB_BUCKET "Sensor"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "UTC1"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor_readings("measurements");

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  // Add tags
  sensor_readings.addTag("device", DEVICE);
  sensor_readings.addTag("SSID", WiFi.SSID());

  // Alternatively, set insecure connection to skip server certificate validation
  //client.setInsecure();

 timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  sensor_readings.clearFields();
  sensor_readings.addField("rssi", WiFi.RSSI());
  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(sensor_readings));
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }
  // Write point
  if (!client.writePoint(sensor_readings)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  Serial.println("Wait 10s");
  delay(10000);
}
