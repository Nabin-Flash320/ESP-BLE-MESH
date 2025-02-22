
#include "esp_log.h"
#include "esp_ble_mesh_lighting_model_api.h"

#include "BLE_mesh_definitions.h"
#include "BLE_mesh_initializer.h"

#include "BLE_mesh_lighting_server_model.h"

static esp_ble_mesh_light_ctl_state_t lighting_server_states = {
    .lightness = 0,
    .target_lightness = 32767,

    .temperature = 3500,
    .target_temperature = 4000,

    .delta_uv = 0,
    .target_delta_uv = 0,

    .status_code = 0,
    .temperature_range_min = 2700,
    .temperature_range_max = 6500,

    .lightness_default = 32767,
    .temperature_default = 4000,
    .delta_uv_default = 0,
};

static esp_ble_mesh_light_ctl_setup_srv_t lighting_setup_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &lighting_server_states,
};

static esp_ble_mesh_light_ctl_srv_t lighting_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &lighting_server_states,
};

static void lighting_model_setup_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "Lighting model callback");
}

void BLE_mesh_lighting_model_intialize()
{
    // Register callback for lighting ctl server.
    ble_mesh_register_sig_lighting_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV,
        lighting_model_setup_server_callback,
        &lighting_server_user_data);

    // Register callback for lighting ctl setup server.
    ble_mesh_register_sig_lighting_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV,
        lighting_model_setup_server_callback,
        &lighting_setup_server_user_data);
}
