#include <Arduino.h>
#include <SdsDustSensor.h>

int SDS_RX_PIN = D5;
int SDS_TX_PIN = D6;
SoftwareSerial softwareSerial(SDS_RX_PIN, SDS_TX_PIN);
SdsDustSensor sds(softwareSerial); //  additional parameters: retryDelayMs and maxRetriesNotAvailable
const int MINUTE = 60000;
const int WAKEUP_WORKING_TIME = MINUTE / 2; // 30 seconds.
const int SLEEPING_INTERVAL = 2 * MINUTE;

void setup() {
  Serial.begin(115200);
  sds.begin();
}

void loop() {
  sds.wakeup();
  Serial.print("waking up! wait for ");
  Serial.print(WAKEUP_WORKING_TIME / 1000);
  Serial.println(" seconds");
  delay(WAKEUP_WORKING_TIME);
  
  PmResult pm = sds.queryPm();
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25); // float, μg/m3
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);
  } else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }
  
  WorkingStateResult state = sds.sleep();
  if (state.isWorking()) {
    Serial.println("Problem with sleeping the SDS011 sensor.");
  } else {
    Serial.print("now deep sleep for ");
    Serial.print(SLEEPING_INTERVAL / 1000);
    Serial.println(" seconds!");
    delay(SLEEPING_INTERVAL);         //120 secondi, 2 minuti
  }
}
