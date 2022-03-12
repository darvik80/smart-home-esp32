//
// Created by Ivan Kishchenko on 12.03.2022.
//

#ifndef SMART_HOME_ESP32_USERMESSAGE_H
#define SMART_HOME_ESP32_USERMESSAGE_H

#include <service/ServiceMessage.h>
#include "service/LibServiceId.h"

enum UserMessageId {
    SYS_MON = USER_EVENT
};

enum UserServiceId {
    SYSTEM_MONITOR = USER_SERVICES,
    IOT
};

struct SystemMonitoringEvent : TMessage<SYS_MON> {
    float cpuTemp{};
};

#endif //SMART_HOME_ESP32_USERMESSAGE_H
