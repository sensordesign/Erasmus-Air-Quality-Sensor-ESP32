/*
  Mazzone Edoardo
  Complete project details at "https://github.com/edomazzone" (instrucatbles che pubblico)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

//include all library

#include <MHZ19.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <SDS011.h>
#include <SPIFFS.h>
#include <DHT.h>
#include <WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <esp_sleep.h>

/******************************************************/
//MHZ-19

#define RX_PIN 26                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 27                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)#include <Arduino.h>

MHZ19 myMHZ19;                                             // Constructor for library
HardwareSerial mySerial(1);                                // On ESP32 we do not require the SoftwareSerial library, since we have 2 USARTS available

unsigned long getDataTimer;
unsigned long preheat_time = 15000;     //test time
int CO2;

/******************************************************/
//OTA

const char* ssid = "xxxxxxxxxxxxxxxxx";
const char* password = "yyyyyyyyyyyyyy";

AsyncWebServer server(80);

/******************************************************/
//SDS011

float p10, p25;
int err;
SDS011 my_sds;
#ifdef ESP32
HardwareSerial port(2);
#endif

/******************************************************/
//DHT11

#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/******************************************************/
//InfluxDB

WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#define WIFI_SSID "xxxxxxxxxxxxxxxxxx"
#define WIFI_PASSWORD "yyyyyyyyyyyyyyyyyyyyy"

#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"
#define INFLUXDB_ORG "zzzzzzzzzz"
#define INFLUXDB_BUCKET "Sensor"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "UTC2"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor_readings("measurements");

/**************************************/
//Global var

byte i;
unsigned long s = 30;
double Temp;
#define uS_TO_S_FACTOR 1000000    // Conversion factor for micro seconds to seconds 
#define TIME_TO_SLEEP  300        // Time ESP32 will go to sleep (in seconds) 5min*60s
int interval = 5000;              //start time for PM sensor


void setup() {
  Serial.begin(9600);                                            // Start serial monitor
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Set sleep time

  WiFi.mode(WIFI_STA);                                           //Turn on WiFi
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while ((wifiMulti.run() != WL_CONNECTED) && (i < 5)) {
    i++;
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //remember to connect at same SSID

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "AirQualitySensor");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  // Add tags
  sensor_readings.addTag("sensor", "Green");  //if you are using more sensor (we chose case color)

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

  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);    // device to MH-Z19 serial start
  myMHZ19.begin(mySerial);                                // *Serial(Stream) reference must be passed to library begin().
  myMHZ19.setRange(2000);
  myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
  my_sds.begin(&port);
  dht.begin();

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void loop() {
  //Print not sended data
  File file2 = SPIFFS.open("/PM sensor.txt");
  if (!file2) {
    Serial.println("Failed to open file for reading");
    return;
  }
  Serial.println("File Content:");
  while (file2.available()) {
    Serial.write(file2.read());
  }
  file2.close();

  //retrieve new data
  sds_011.wakeup();
  delay(interval);
  err = my_sds.read(&p25, &p10);                    // Request PM values
  if (!err) {
    Serial.print("P2.5: " + String(p25));
    Serial.println("  P10:  " + String(p10));
  }
  else{
        Serial.println("Error PM READINGS!!");
  }
  sds_011.sleep();

  CO2 = myMHZ19.getCO2();                             // Request CO2 (as ppm)
  Temp = myMHZ19.getTemperature();                    // Request Temperature (as Celsius)
  Serial.print("CO2 (ppm): ");
  Serial.print(CO2);
  Serial.print(" Temperature (C): ");
  Serial.println(Temp);

  double h = dht.readHumidity();
  double t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {                             // Check if any reads failed
    Serial.println("Error TEMPERATURE SENSOR!!");
  }

  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.println("");
  Serial.println("-----------------------");
  Serial.println("");

  //Send data to InfluxDB
  sensor_readings.clearFields();
  sensor_readings.addField("rssi", WiFi.RSSI());
  sensor_readings.addField("temperature", t);   //Get Data from temperature sensor
  sensor_readings.addField("co2", CO2);                //Get Data from from co2 sensor
  sensor_readings.addField("humidity", h);             //Get Data from humidity sensor
  sensor_readings.addField("pm2_5", p25);              //Get PM2_5
  sensor_readings.addField("pm10", p10);               //Get PM10

  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(sensor_readings));
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }
  // Write point
  if (!client.writePoint(sensor_readings)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());

    //if WiFi not works save in the SPIFFS
    String measurements = "Time: " + String(s) + ", " + "T: " + String(Temp + 4) + ", " + "H: " + String(h) + ", " + "PM10: " + String(p10) + ", " + "PM2.5: " + String(p25) + ", " + "CO2: " + String(CO2) + ", " + + "RSSI: " + String( WiFi.RSSI()) + " \n";

    File file = SPIFFS.open("/PM sensor.txt", FILE_APPEND);
    if (!file) {
      Serial.println("There was an error opening the file for writing");
      return;
    }
    if (file.print(measurements)) {
      Serial.println("File was written");
    } else {
      Serial.println("File write failed");
    }
    file.close();
  }
  
  //start sleep
  Serial.println("Going to sleep now");
  delay(100);
  Serial.flush();
  esp_deep_sleep_start();
}
