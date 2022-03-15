//
// Created by Ivan Kishchenko on 12.03.2022.
//

#include "SystemMonitoringService.h"
#include "service/iot/IotService.h"
#include <driver/temp_sensor.h>
#include "ArduinoJson.h"

void SystemMonitoringService::setup() {
    Service::setup();

    temp_sensor_config_t cfg = TSENS_CONFIG_DEFAULT();
    if (ESP_OK != temp_sensor_set_config(cfg)) {
        logging::warning("can't init temp sensor");
        return;
    }

    if (ESP_OK != temp_sensor_start()) {
        logging::warning("can't init temp sensor");
        return;
    }

    _ticker.attach_ms<SystemMonitoringService *>(5000, [](SystemMonitoringService *service) {
        service->onTimer();
    }, this);

    logging::info("system-monitoring configured");
}

void SystemMonitoringService::onTimer() {
    auto iot = getRegistry()->getService<IotService>(LibServiceId::IOT);
    if (iot) {
        float cpuTemp = {};
        if (ESP_OK == temp_sensor_read_celsius(&cpuTemp)) {
            DynamicJsonDocument doc(128);
            doc["cpu-temp"] = cpuTemp;
            String data;
            serializeJson(doc, data);

            iot->telemetry(data.c_str());
            logging::info("send cpu mon: {}", data.c_str());

            sendMessage(
                    Service::getMessageBus(),
                    DisplayText{
                            .line = 0,
                            .text = fmt::format("CPU: {} C", cpuTemp)
                    }
            );
        } else {
            logging::warning("can't init get temp from sensor");
        }
    }
}
