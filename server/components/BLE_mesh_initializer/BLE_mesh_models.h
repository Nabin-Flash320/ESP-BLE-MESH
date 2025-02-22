

#ifndef _BLE_MESH_MODELS_H_
#define _BLE_MESH_MODELS_H_

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_lighting_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"

#include "BLE_mesh_definitions.h"
#include "BLE_mesh_opcode.h"
#include "BLE_mesh_publication.h"

static esp_ble_mesh_cfg_srv_t config_server = {
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay = ESP_BLE_MESH_RELAY_DISABLED,
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
};

// Publication and light lightness server models definitions
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_setup_srv_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_srv_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_server_generic_level_server_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_server_generic_onoff_server_pub, 5, ROLE_NODE);
static esp_ble_mesh_model_t light_lightness_server_models[] = {
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(&light_lightness_setup_srv_pub, NULL),
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&light_lightness_srv_pub, NULL),
    ESP_BLE_MESH_MODEL_GEN_LEVEL_SRV(&light_lightness_server_generic_level_server_pub, NULL),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&light_lightness_server_generic_onoff_server_pub, NULL),
};

#endif // _BLE_MESH_MODELS_H_
