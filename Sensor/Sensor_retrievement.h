#ifndef Sensor_retrievement
#define Sensor_retrievement
#include <Arduino.h>
#include <WEMOS_SHT3X.h>

#include "SdsDustSensor.h"
void sds_setup();
void instruments_setup();
double temperature();
double co2();
double humidity();
double pm2_5();
double pm10();

#endif