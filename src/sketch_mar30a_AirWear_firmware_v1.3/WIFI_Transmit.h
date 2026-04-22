#ifndef WIFI_TRANSMIT_H
#define WIFI_TRANSMIT_H

#include <Arduino.h>
#include "BMV080_PM.h"
#include "ENS160_AQ.h"
#include "GPS_Module.h"

void WiFi_Init(void);
bool WiFi_IsConnected(void);
void WiFi_Transmit(const BMV080_Record &pm, const ENS160_Record &aq, const GPS_Record &gps);

#endif