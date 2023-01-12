#ifndef Sensor_retrievement
#define Sensor_retrievement
#include <Arduino.h>
#include <WEMOS_SHT3X.h>
#include "DHT.h"
#include <SoftwareSerial.h>

void start_SDS();
void stop_SDS();
void sds_setup();
void instruments_setup();
double temperature();
double co2();
double humidity();
double pm10();
double pm2_5();
byte getCheckSum(char *packet);
int readCO2UART();
int sds_loop();
#endif
