
#include "esp_log.h"
#include "esp_ble_mesh_lighting_model_api.h"

static esp_ble_mesh_light_hsl_state_t light_hsl_server_states;
static esp_ble_mesh_light_hsl_state_t light_hsl_setup_server_states;
static esp_ble_mesh_light_hsl_state_t light_hsl_hue_server_states;
static esp_ble_mesh_light_hsl_state_t light_hsl_saturation_server_states;

static esp_ble_mesh_light_hsl_srv_t light_hsl_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_hsl_server_states,
};

static esp_ble_mesh_light_hsl_setup_srv_t light_hsl_setup_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_hsl_setup_server_states,
};

static esp_ble_mesh_light_hsl_setup_srv_t light_hsl_hue_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_hsl_hue_server_states,
};

static esp_ble_mesh_light_hsl_setup_srv_t light_hsl_saturation_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_hsl_saturation_server_states,
};

void BLE_mesh_light_hsl_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{

    ESP_LOGI(__FILE__, "HSL sever(%d)", event);
}

void BLE_mesh_light_hsl_model_setup_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "HSL setup sever(%d)", event);
}

void BLE_mesh_light_hsl_hue_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "HSL hue sever(%d)", event);
}

void BLE_mesh_light_hsl_saturation_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "HSL hue sever(%d)", event);
}

void *BLE_mesh_light_server_get_hsl_server_data(uint16_t model_id)
{
    if (ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV == model_id)
    {
        return &light_hsl_server_user_data;
    }
    else if (ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV == model_id)
    {
        return &light_hsl_setup_server_user_data;
    }
    else if (ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV == model_id)
    {
        return &light_hsl_hue_server_user_data;
    }
    else if (ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV == model_id)
    {
        return &light_hsl_saturation_server_user_data;
    }

    return NULL;
}
