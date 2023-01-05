# Bunch of sensors for Esp32
# With OTA updates
## Before Compiling to arduino create 2 files named credentials.h and credentials.cpp using the following structure

### credentials.cpp
```
#include "credentials.h"

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
```
### credentials.h
```

#ifndef credentials
#define credentials

char *wifi_name();
char *wifi_PWD();
char *iNFLUXDB_URL();
char *iNFLUXDB_TOKEN();
char *iNFLUXDB_ORG();
char *iNFLUXDB_BUCKET();
#endif
```
