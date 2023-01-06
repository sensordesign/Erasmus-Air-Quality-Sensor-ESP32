#include <SDS011.h>
SDS011 sds;

float p10, p25;
int error;

void setup() {
  sds.begin(5, 6);
  Serial.begin(9600);
}

void loop() {
  error = sds.read(&p25, &p10);
  if (!error)
  {
    Serial.println("P2,5: " + String(p25));
    Serial.println("P10: " + String(p10));
  }
  else
  {
    Serial.println("Si è verificato un errore");
  }
  delay(30000);     //30 secondi
}
