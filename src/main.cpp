#include <logging/Logging.h>
#include "MyProps.h"
#include <Arduino.h>

#include <memory>
#include "service/Application.h"
#include "service/iot/IotYaCoreService.h"
#include "SystemMonitoringService.h"
#include "cert/cert.yandex.h"

class SmartHomeApp : public TApplication<TRegistry<TMessageBus<10, 2>>> {
public:
    SmartHomeApp() : TApplication(logging::level::info) {}

    void setup() override {
        logging::debug("setup");

        auto &props = getRegistry().getProperties();
        // Wifi
        props.setStr(PROP_WIFI_SSID, WIFI_SSID);
        props.setStr(PROP_WIFI_PASS, WIFI_PASS);

        // MQTT
        auto mqttProps = new MqttProperties{
                .clientId = MQTT_CLIENT_ID,
                .username = MQTT_USERNAME,
                .password = MQTT_PASSWORD,
                .uri = MQTT_URI,
                .certDev = cert_dev,
                .rootCa = root_ca,
        };
        props.setProperty(PROP_MQTT_PROPS, mqttProps);

        getRegistry().create<IotYaCoreService>();
        getRegistry().create<SystemMonitoringService>();

        TApplication::setup();
    }
};

std::unique_ptr<SmartHomeApp> app;

void setup() {
    app = std::make_unique<SmartHomeApp>();
    app->setup();
}

void loop() {
    app->loop();
}