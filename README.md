# Air Quality ESP32: Build a Portable IoT Sensor
Hi everyone! In this repo you will find everything you need to recreate our project, for details visit xxxyyyzzz. This is a starting project about air quality monitoring with ESP32 and SDS011.
The first things you need are the libraries for sensors, they are in the dedicated directory, then there are "Case and PCB", here you will find the Gerber file and the .dxf one for the cover of the case, we used 4mm wood but you can also print it.
## Code
The sketches for testing if sensors and connection work are in the "Test sketch" directory. 
In "Sensor_InfluxDB_and_OTA" you will find the tested sketch while new code is available in "Last sketch"

```
//OTA

const char* ssid = "xxxxxxxxxxxxxxxxx";
const char* password = "yyyyyyyyyyyyyy";

#define DEVICE "ESP32"

#define WIFI_SSID "xxxxxxxxxxxxxxxxxx"
#define WIFI_PASSWORD "yyyyyyyyyyyyyyyyyyyyy"

#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "zzzzzzzzzzzzzzzzzzzzz"
#define INFLUXDB_ORG "xxxxxxxxxxxxxx"
#define INFLUXDB_BUCKET "Sensor"
// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "UTC2"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor_readings("measurements");
```
Replace ssid and password with your ones, for token and organization you can read [this guide](https://randomnerdtutorials.com/esp32-influxdb/) to set InfluxDB and get this information.
