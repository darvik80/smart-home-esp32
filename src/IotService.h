//
// Created by Ivan Kishchenko on 06.03.2022.
//

#pragma once

#include <service/Service.h>
#include "UserMessage.h"

class IotService : public Service, public TMessageSubscriber<IotService, SystemMonitoringEvent> {
public:
    explicit IotService(IRegistry *registry) : Service(registry) {}

    [[nodiscard]] ServiceId getServiceId() const override;

    void setup() override;

    void onMessage(const SystemMonitoringEvent &msg);
};

