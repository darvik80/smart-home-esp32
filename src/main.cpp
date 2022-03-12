#include <logging/Logging.h>
#include "MyConfig.h"
#include "MyProps.h"
#include <Arduino.h>
#include "service/Application.h"
#include "IotService.h"
#include "SystemMonitoringService.h"

class SmartHomeApp : public Application {
public:
    SmartHomeApp()
            : Application(logging::level::debug, new TMessageBus<10>()) {

    }

    void setup() override {
        logging::debug("setup");
        auto props = getRegistry()->getProperties();
        // Wifi
        props->setStr(PROP_WIFI_SSID, WIFI_SSID);
        props->setStr(PROP_WIFI_PASS, WIFI_PASS);

        // MQTT
        props->setStr(PROP_MQTT_SERVER_HOST, MQTT_SERVER_HOST);
        props->setUint16(PROP_MQTT_SERVER_PORT, MQTT_SERVER_PORT);
        props->setStr(PROP_MQTT_CLIENT_ID, MQTT_CLIENT_ID);
        props->setStr(PROP_MQTT_USER, MQTT_ACCESS_TOKEN);

        getRegistry()->create<IotService>();
        getRegistry()->create<SystemMonitoringService>();
        Application::setup();

        getRegistry()->getService<DisplayService>(LibServiceId::OLED)->setText(4, "Device is ready");
    }
};

SmartHomeApp app;

#include <pins_arduino.h>

void setup() {
// write your initialization code here
    app.setup();
}

void loop() {
// write your code here
    app.loop();
}