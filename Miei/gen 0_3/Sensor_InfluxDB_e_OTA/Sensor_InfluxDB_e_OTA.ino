//MHZ-19B

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
//OTA

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid = "TOTOLINK N200RE";
const char* password = "";

AsyncWebServer server(80);

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

/**************************************/
//InfluxDB

#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


#define WIFI_SSID "Redmi Note 11S"

#define WIFI_PASSWORD "sebastian86"
#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "TcfzPq142TWxAC7D9cjkcza0KWNvb5HU9DrwDYg6zX0L4x6hs9Bft8h077KJHpVdiz9kDHEjAcUzcNNWqomj9g=="
#define INFLUXDB_ORG "2e1bcae08357dacf"
#define INFLUXDB_BUCKET "Sensor"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor_readings("measurements");

void setup() {
  Serial.begin(9600);                                     // Device to serial monitor feedback
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //remember to connect at same SSID

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  // Add tags
  sensor_readings.addTag("device", "Esp32-Kri-Prova");

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

  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);                         // (Uno example) device to MH-Z19 serial start
  myMHZ19.begin(mySerial);                                // *Serial(Stream) reference must be passed to library begin().
  myMHZ19.setRange(2000);
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

    sensor_readings.clearFields();
    sensor_readings.addField("rssi", WiFi.RSSI());
    sensor_readings.addField("Temperature", t); /*Get Data from a class which retrieves data from the temperature sensor*/
    sensor_readings.addField("co2", CO2);                /*Get Data from a class which retrieves data from the co2 sensor*/
    sensor_readings.addField("humidity", h);       /*Get Data from a class which retrieves data from the humidity sensor*/
    sensor_readings.addField("pm2_5", p25); /*Get Data from a class which retrieves data from the PM2_5 sensor*/
    sensor_readings.addField("pm10", p10);   /*Get Data from a class which retrieves data from the pm10 sensor*/

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
