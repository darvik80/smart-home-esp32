#include <logging/Logging.h>
#include "MyConfig.h"
#include "MyProps.h"
#include <Arduino.h>
#include "service/Application.h"
#include "service/iot/IotYaCoreService.h"
#include "SystemMonitoringService.h"
#include "cert/cert.yandex.h"

class SmartHomeApp : public Application {
public:
    SmartHomeApp()
            : Application(logging::level::info, new TMessageBus<10>()) {

    }

    void setup() override {
        logging::debug("setup");
        esp_log_level_set("*", ESP_LOG_INFO);
        esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
        esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
        esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
        esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
        esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

        auto props = getRegistry()->getProperties();
        // Wifi
        props->setStr(PROP_WIFI_SSID, WIFI_SSID);
        props->setStr(PROP_WIFI_PASS, WIFI_PASS);

        // MQTT
        auto mqttProps = new MqttProperties{
                .clientId = MQTT_CLIENT_ID,
                .username = MQTT_USERNAME,
                .password = MQTT_PASSWORD,
                .uri = MQTT_URI,
                .certDev = cert_dev,
                .rootCa = root_ca,
        };
        props->setProperty(PROP_MQTT_PROPS, mqttProps);

        getRegistry()->create<IotYaCoreService>();
        getRegistry()->create<SystemMonitoringService>();
        Application::setup();

        getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(4, "Device is ready");
    }
};

SmartHomeApp app;

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}