#ifndef NPT_TIME_H
#define NPT_TIME_H

#include <Arduino.h>
#include <WiFiNINA.h>
#include <RTCZero.h>
#include "secrets.h"

const unsigned long MAX_TIME_BETWEEN_RTC_SYNC = 3600UL;

void setup_time();

unsigned long sendNTPpacket(IPAddress& address);
unsigned long get_timestamp_now();

#endif