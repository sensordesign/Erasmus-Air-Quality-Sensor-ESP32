# Sensors pack for Esp32 With OTA updates via WEB

## Before Compiling and uploading to Arduino
Create 2 files named config.h and config.cpp and put them in src/config/ \
The asterisk is where you should input your own information \
Those files need to be created using the structures indicated below \
Also include the libraries for the sensors and modify any eventual difference in pinout structures 
### config.cpp
```
#include "config.h"

char *wifi_name() {
  return "**"; // wifi name
}
char *wifi_PWD() {
  return "**"; // wifi password
}
char *iNFLUXDB_URL(){
  return "**"; // server url
}
char *iNFLUXDB_TOKEN(){
  return "**"; // influxdb token
}
char *iNFLUXDB_ORG() {
  return "**"; //org ID
}
char *iNFLUXDB_BUCKET(){
  return "**"; //bucket name
}
bool wantdeepSleep(){
  return false; // insert whether you want to go to deep sleep or not. the latter is useful for installations which are not permanent
}
```
### config.h
```
#ifndef config
#define config

char *wifi_name();
char *wifi_PWD();
char *iNFLUXDB_URL();
char *iNFLUXDB_TOKEN();
char *iNFLUXDB_ORG();
char *iNFLUXDB_BUCKET();
bool wantdeepSleep();

#endif
```
