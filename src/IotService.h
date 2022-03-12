//
// Created by Ivan Kishchenko on 06.03.2022.
//

#pragma once

#include <service/Service.h>
#include <mqtt_client.h>
#include <Ticker.h>
#include "UserMessage.h"

class IotService : public Service, public TMessageSubscriber<IotService, WifiConnected, SystemMonitoringEvent> {
    esp_mqtt_client_handle_t _client{};

    Ticker _ticker;
public:
    explicit IotService(IRegistry *registry) : Service(registry){}

    [[nodiscard]] ServiceId getServiceId() const override;

    void setup() override;

    void onEvent(esp_mqtt_event_handle_t event);

    void onMessage(const WifiConnected &msg);

    void onMessage(const SystemMonitoringEvent &msg);
};

