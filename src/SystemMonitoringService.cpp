//
// Created by Ivan Kishchenko on 12.03.2022.
//

#include "SystemMonitoringService.h"
#include <driver/temp_sensor.h>

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
        float result;
        if (ESP_OK == temp_sensor_read_celsius(&result)) {
            SystemMonitoringEvent event;
            event.cpuTemp = result;
            sendMessage(service->getMessageBus(),event);
        } else {
            logging::warning("can't init get temp from sensor");
        }
    }, this);
}
