
#include "esp_log.h"
#include "esp_ble_mesh_generic_model_api.h"

#include "BLE_mesh_light_server.h"

static esp_ble_mesh_gen_onoff_srv_t light_lightness_generic_onoff_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = {
        .onoff = 0,
        .target_onoff = 0,
    },
};

void BLE_mesh_light_server_generic_on_off_model_callback(esp_ble_mesh_generic_server_cb_event_t event, esp_ble_mesh_generic_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "%s", __func__);
}

void *BLE_mesh_light_server_get_generic_on_off_server_data()
{
    return &light_lightness_generic_onoff_server_user_data;
}

