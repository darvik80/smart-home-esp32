//
// Created by Ivan Kishchenko on 12.03.2022.
//

#pragma once

#include <Ticker.h>
#include "service/Service.h"
#include "UserMessage.h"

class SystemMonitoringService : public Service {
    Ticker _ticker;
public:
    explicit SystemMonitoringService(IRegistry *registry) : Service(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override {
        return SYSTEM_MONITOR;
    }

    void setup() override;
};
