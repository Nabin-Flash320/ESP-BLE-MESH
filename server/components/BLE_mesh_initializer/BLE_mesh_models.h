

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

ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_light_ctl_setup_server_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_light_ctl_server_pub, 5, ROLE_NODE);

ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_light_hsl_setup_server_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_light_hsl_server_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_light_hsl_hue_server_pub, 5, ROLE_NODE);
ESP_BLE_MESH_MODEL_PUB_DEFINE(light_lightness_light_hsl_saturation_server_pub, 5, ROLE_NODE);

static esp_ble_mesh_model_t light_lightness_server_models[] = {
    // Controls overall on-off state of the light
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&light_lightness_server_generic_onoff_server_pub, NULL),

    // Controls the brightness of the light
    ESP_BLE_MESH_MODEL_GEN_LEVEL_SRV(&light_lightness_server_generic_level_server_pub, NULL),

    // Standard dimming functionalities
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(&light_lightness_setup_srv_pub, NULL),
    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(&light_lightness_srv_pub, NULL),

    // Light CTL server models for for cpntriling CCT(color coded temperature) for CW and temperature and delta uv if needed
    ESP_BLE_MESH_MODEL_LIGHT_CTL_SETUP_SRV(&light_lightness_light_ctl_setup_server_pub, NULL),
    ESP_BLE_MESH_MODEL_LIGHT_CTL_SRV(&light_lightness_light_ctl_server_pub, NULL),

    // Scene server model(0x1203) optional for saving and recalling predefined light settings
    // Time server model(0x1200) optional for time-based controlling of light
};

static esp_ble_mesh_model_t light_server_hsl_server_models[] = {
    // Light HSL server model for Hue, Saturation and lightness of the RGB channel
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV(&light_lightness_light_hsl_setup_server_pub, NULL),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SRV(&light_lightness_light_hsl_server_pub, NULL),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_HUE_SRV(&light_lightness_light_hsl_hue_server_pub, NULL),
    ESP_BLE_MESH_MODEL_LIGHT_HSL_SAT_SRV(&light_lightness_light_hsl_saturation_server_pub, NULL),
};

#endif // _BLE_MESH_MODELS_H_
