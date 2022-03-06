//
// Created by Ivan Kishchenko on 06.03.2022.
//

#include "Esp32MqttService.h"

LOG_COMPONENT_SETUP(esp32mqtt);

extern const uint8_t server_pem_start[] asm("_binary_resources_server_pem_start");
extern const uint8_t server_pem_end[] asm("_binary_resources_server_pem_end");

void mqtt_event_callback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    ((Esp32MqttService *) event_handler_arg)->onEvent((esp_mqtt_event_handle_t) event_data);
}

ServiceId Esp32MqttService::getServiceId() const {
    return MQTT;
}

void Esp32MqttService::onEvent(esp_mqtt_event_handle_t event) {
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(_client, "v1/devices/me/rpc/request/+", 1);
            esp32mqtt::log::info("connected");
            _ticker.attach_ms<Esp32MqttService *>(5000, [](Esp32MqttService *service) {
                esp32mqtt::log::debug("send status");
                std::string data = R"({"status": "online"})";
                esp_mqtt_client_publish(service->_client, "v1/devices/me/telemetry", data.c_str(), data.size(), 0,
                                        false);
            }, this);
            break;
        case MQTT_EVENT_DISCONNECTED:
            _ticker.detach();
            esp32mqtt::log::info("disconnected");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            esp32mqtt::log::info("subscribed");
            break;

        case MQTT_EVENT_ERROR:
            esp32mqtt::log::info("error");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
                ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
                ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                         strerror(event->error_handle->esp_transport_sock_errno));
            } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
                ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
            } else {
                ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
            }
            break;
            break;
        default:
            break;

    }
}

void Esp32MqttService::setup() {
    getMessageBus()->subscribe(this);

    esp32mqtt::log::info("server pem: {}", (int) (server_pem_end - server_pem_start));
    Service::setup();
}

void Esp32MqttService::onMessage(const WifiConnected &msg) {
    esp32mqtt::log::info("handle wifi-conn, try connect to mqtt");
    const esp_mqtt_client_config_t config{
            .uri = "mqtt://iot.crearts.xyz:1883",
            .client_id = "esp32-dev",
            .username = "esp32-dev-token",
            .user_context = (void *) this,
            //.cert_pem = (const char *) server_pem_start,
            //.cert_len = (size_t) (server_pem_end - server_pem_start),
    };

    _client = esp_mqtt_client_init(&config);
    esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, mqtt_event_callback, this);
    esp_mqtt_client_start(_client);
}


