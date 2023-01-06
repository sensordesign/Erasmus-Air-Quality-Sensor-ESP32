#include <DHT.h>
#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
}

void loop() {
  double t = dht.readTemperature();
  double h = dht.readHumidity();
  Serial.print(t);
  Serial.println(h);
  delay(5000);
}
