
#include "esp_log.h"
#include "esp_ble_mesh_generic_model_api.h"

#include "BLE_mesh_light_server.h"

static esp_ble_mesh_gen_level_srv_t light_lightness_generic_level_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = {
        .level = 0,
        .target_level = 0,
        .last_level = 0,
        .last_delta = 0,
        .move_start = false,
        .positive = false,
    },
};



void BLE_mesh_light_lightness_generic_level_model_callback(esp_ble_mesh_generic_server_cb_event_t event, esp_ble_mesh_generic_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "%s", __func__);
}

void *BLE_mesh_light_server_get_generic_level_server_data()
{
    return &light_lightness_generic_level_server_user_data;
}
