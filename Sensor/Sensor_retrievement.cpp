#include "Sensor_retrievement.h"
#include <WEMOS_SHT3X.h>
SHT3X sht30(0x45);
/*
#include "SdsDustSensor.h"

SdsDustSensor sds(1, 2);*/

const int wakeupDelay = 15000;
const double offset=8.00;

void instruments_setup(){
/*
  sds.begin();
*/
}

double  pm2__5,pm_10;

void sds_setup(){/*
  sds.wakeup();
  Serial.print("waking up! wait for ");
  Serial.print(wakeupDelay / 1000);
  Serial.println(" seconds");
  delay(wakeupDelay); // working 15 seconds

  PmResult pm = sds.queryPm();
  if (pm.isOk()) {
    pm2__5 = pm.pm25;
    pm_10 = pm.pm10;

    // if you want to just print the measured values, you can use toString() method as well
    //Serial.println(pm.toString());
  }
  else {
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }

  WorkingStateResult state = sds.sleep();
  if (state.isWorking()) {
    Serial.println("Problem with sleeping the sensor.");
  }
  else {
    Serial.println("Sensor is sleeping");
  }*/

}

double temperature()
{
  /*
  return sht30.cTemp - offset;
*/
return random(-1000,2000)/100.00;
}
double co2()
{
  /*
  return sht30.cTemp - offset;
*/
return random(10000,20000)/100.00;
}
double humidity()
{
  /*
  return sht30.humidity;
*/
return random(0,10000)/100.00;
}
double pm2_5()
{
/*
  return pm2__5;
*/
return random(200,600)/100.00;
}
double pm10()
{
  /*
  return pm_10;
  */
  
return random(0,200)/100.00;
}

