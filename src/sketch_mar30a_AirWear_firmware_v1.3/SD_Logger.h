#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <Arduino.h>
#include "BMV080_PM.h"
#include "ENS160_AQ.h"
#include "GPS_Module.h"

bool SD_Log_Init(void);
bool SD_Log_All(const BMV080_Record &pm, const ENS160_Record &aq, const GPS_Record &gps);

#endif