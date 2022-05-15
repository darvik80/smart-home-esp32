//
// Created by Ivan Kishchenko on 12.03.2022.
//

#include "logging/Logging.h"

#include "SystemMonitoringService.h"
#include <driver/temp_sensor.h>
#include "ArduinoJson.h"

void SystemMonitoringService::setup(Registry &registry) {
    temp_sensor_config_t cfg = TSENS_CONFIG_DEFAULT();
    if (ESP_OK != temp_sensor_set_config(cfg)) {
        logging::warning("can't init temp sensor");
        return;
    }

    if (ESP_OK != temp_sensor_start()) {
        logging::warning("can't init temp sensor");
        return;
    }

    schedule(registry.getMessageBus(), 1000, true, [&registry, this]() {
        float cpuTemp = {};
        if (ESP_OK == temp_sensor_read_celsius(&cpuTemp)) {
            DynamicJsonDocument doc(128);
            doc["cpu-temp"] = cpuTemp;
            String data;
            serializeJson(doc, data);

            sendMessage(registry.getMessageBus(), IoTTelemetry{data.c_str()});
            logging::info("send cpu mon: {}", data.c_str());
        } else {
            logging::warning("can't init get temp from sensor");
        }
    });

    logging::info("system-monitoring configured");
}

