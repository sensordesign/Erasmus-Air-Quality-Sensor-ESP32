#include "config.h"

char *wifi_name() {
  return "Infostrada-9398E1";
}

char *wifi_PWD() {
  return "BDLMD8P73D";
}

char *iNFLUXDB_URL(){
  return "https://europe-west1-1.gcp.cloud2.influxdata.com";
}
char *iNFLUXDB_TOKEN(){
  return "sfxn7YKbhdghIoSXpakejuUUAXcBwfW-TNu1JEIT3wmeorda1BcXm_pZJW5DAXNKV7x_y23Irponc5lbW5k3JA==";
}
char *iNFLUXDB_ORG() {
  return "8f7a0a2a53c35db0";
}
char *iNFLUXDB_BUCKET(){
  return "sensor_data_test";
}
bool wantdeepSleep(){
  return false;// insert whether you want to go to deep sleep or not. the latter is useful for installations which are not permanent
}