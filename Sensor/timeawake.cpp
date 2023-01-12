#include "timeawake.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

  // Define NTP Client to get time
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
int interval[8] = { 14400, 10800, 3600, 1200, 600, 240, 120, 80 };
// test int interval[8] = { 14, 10800, 150, 130, 600, 240, 120, 80 };
void timesetup() {
  timeClient.begin();
  timeClient.setTimeOffset(3600);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
}
int getdeepsleep() {
  int h = timeClient.getHours(), m = timeClient.getMinutes(), Deepsleep = 0;
  if (((h >= 0) && (h < 4)) || ((h >= 20) && (h < 24))) {
    Deepsleep = interval[0];
  }
  if ((h >= 17) && (h < 20)) {
    Deepsleep = interval[1];
  }
  if (((h >= 4) && (h < 6)) || ((h >= 15) && (h < 17)) || ((h >= 10) && (h < 12))) {
    Deepsleep = interval[2];
  }
  if ((h == 6) || (h == 9) || (h == 14)) {
    Deepsleep = interval[3];
  }
  if (((h == 7) && (m < 30)) || ((h == 12) && (m < 30)) || ((h == 8) && (m >= 30)) || ((h == 13) && (m >= 30))) {
    Deepsleep = interval[4];
  }
  if (((h == 7) && (m >= 30) && (m < 50)) || ((h == 8) && (m >= 10) && (m < 30)) || ((h == 12) && (m >= 30) && (m < 50))) {
    Deepsleep = interval[5];
  }
  if (((h == 7) && (m >= 50)) || ((h == 8) && (m < 10))) {
    Deepsleep = interval[6];
  }
  if (((h == 12) && (m >= 50)) || ((h == 13) && (m < 30))) {
    Deepsleep = interval[7];
  }
  
  return Deepsleep;
}
