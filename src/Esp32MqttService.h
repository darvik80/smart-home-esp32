//
// Created by Ivan Kishchenko on 06.03.2022.
//

#pragma once

#include <service/Service.h>
#include <mqtt_client.h>
#include <Ticker.h>

class Esp32MqttService : public Service, public TMessageSubscriber<Esp32MqttService, WifiConnected> {
    esp_mqtt_client_handle_t _client{};

    Ticker _ticker;
public:
    explicit Esp32MqttService(IRegistry *registry) : Service(registry){}

    [[nodiscard]] ServiceId getServiceId() const override;

    void setup() override;

    void onEvent(esp_mqtt_event_handle_t event);

    void onMessage(const WifiConnected &msg);
};

