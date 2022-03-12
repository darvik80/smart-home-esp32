//
// Created by Ivan Kishchenko on 06.03.2022.
//

#include "IotService.h"
#include "service/display/DisplayService.h"
#include <ArduinoJson.h>

LOG_COMPONENT_SETUP(iot);

void mqtt_event_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ((IotService *) event_handler_arg)->onEvent((esp_mqtt_event_handle_t) event_data);
}

ServiceId IotService::getServiceId() const {
    return MQTT;
}

void IotService::onEvent(esp_mqtt_event_handle_t event) {
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(_client, "v1/devices/me/rpc/request/+", 1);
            iot::log::info("connected");
            getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(1, "MQTT connected");
//            _ticker.attach_ms<IotService *>(5000, [](IotService *service) {
//                iot::log::debug("send status");
//                std::string data = R"({"status": "online"})";
//                esp_mqtt_client_publish(service->_client, "v1/devices/me/telemetry", data.c_str(), data.size(), 0,
//                                        false);
//            }, this);
            break;
        case MQTT_EVENT_DISCONNECTED:
            _ticker.detach();
            iot::log::info("disconnected");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            iot::log::info("subscribed");
            break;

        case MQTT_EVENT_ERROR:
            iot::log::info("error");
            break;
        default:
            break;

    }
}

void IotService::setup() {
    getMessageBus()->subscribe(this);

    Service::setup();
}

void IotService::onMessage(const WifiConnected &msg) {
    iot::log::info("handle wifi-conn, try connect to mqtt");
    const esp_mqtt_client_config_t config{
            .uri = "mqtt://iot.crearts.xyz:1883",
            .client_id = "esp32-dev",
            .username = "esp32-dev-token",
            .user_context = (void *) this,
    };

    _client = esp_mqtt_client_init(&config);
    esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, mqtt_event_callback, this);
    esp_mqtt_client_start(_client);

    getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(1, "MQTT configured");
}

void IotService::onMessage(const SystemMonitoringEvent &msg) {
    DynamicJsonDocument doc(64);
    doc["cpu-temp"] = msg.cpuTemp;
    String event;
    serializeJson(doc, event);

    esp_mqtt_client_publish(_client, "v1/devices/me/telemetry", event.c_str(), (int)event.length(), 0, false);
    iot::log::info("send cpu mon: {}", event.c_str());

    getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(0, fmt::format("CPU: {} C", msg.cpuTemp));
}


