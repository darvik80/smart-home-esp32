//
// Created by Ivan Kishchenko on 12.03.2022.
//

#pragma once

#include "system/Timer.h"
#include "service/Registry.h"
#include "UserServiceId.h"

class SystemMonitoringService : public TService<Service_SysMonitor> {
public:
    void setup(Registry &registry) override;
};
