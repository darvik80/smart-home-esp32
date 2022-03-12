//
// Created by Ivan Kishchenko on 06.03.2022.
//

#include "IotService.h"
#include "service/display/DisplayService.h"
#include <ArduinoJson.h>

LOG_COMPONENT_SETUP(iot);

ServiceId IotService::getServiceId() const {
    return IOT;
}

void IotService::setup() {
    getMessageBus()->subscribe(this);

    Service::setup();
}

void IotService::onMessage(const SystemMonitoringEvent &msg) {
    iot::log::info("handle cpu mon: {}", msg.cpuTemp);
    DynamicJsonDocument doc(128);
    doc["cpu-temp"] = msg.cpuTemp;
    String event;
    serializeJson(doc, event);

    sendMessage(
            Service::getMessageBus(),
            MqttMessage{
                    .topic = "v1/devices/me/telemetry",
                    .data = event.c_str(),
                    .qos = 0
            }
    );
    iot::log::info("send cpu mon: {}", event.c_str());
    getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(0, fmt::format("CPU: {} C", msg.cpuTemp));
}


