
#include "esp_log.h"
#include "esp_ble_mesh_lighting_model_api.h"

#include "BLE_mesh_light_server.h"

static esp_ble_mesh_light_ctl_state_t light_ctl_server_state;
static esp_ble_mesh_light_ctl_state_t light_ctl_setup_server_state;

static esp_ble_mesh_light_ctl_srv_t light_ctl_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_ctl_server_state,
};

static esp_ble_mesh_light_ctl_setup_srv_t light_ctl_setup_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_ctl_setup_server_state,
};

void BLE_mesh_light_ctl_model_setup_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGE(__FILE__, "HSL setup server");
}

void BLE_mesh_light_ctl_lightness_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGE(__FILE__, "HSL server");
}

void *BLE_mesh_light_server_get_light_ctl_server_data(uint16_t model_id)
{
    if(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV == model_id)
    {
        return &light_ctl_server_user_data;
    }
    else if(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV == model_id)
    {
        return &light_ctl_setup_server_user_data;
    }
    return NULL;
}