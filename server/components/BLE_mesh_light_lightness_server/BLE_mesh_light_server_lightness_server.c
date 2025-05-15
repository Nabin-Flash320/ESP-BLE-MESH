
#include <math.h>

#include "esp_ble_mesh_lighting_model_api.h"
#include "esp_ble_mesh_networking_api.h"

#include "BLE_mesh_light_server.h"

static void light_lightness_model_server_state_change_event_processor(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param);

static esp_ble_mesh_light_lightness_state_t light_lightness_states_server = {
    // The linear lightness corresponds to the actual light intensity emitted by the light source i.e. illuminance measured in lumens/m^2
    .lightness_linear = 0,
    .target_lightness_linear = 0,
    // The linear lightness corresponds to the actual light intensity reaching the human eye i.e. luminance measured in candelas/m^2(This happens due to scattering of light to surrounding)
    .lightness_actual = 0,
    .target_lightness_actual = 0,

    .lightness_last = 0,
    .lightness_default = 0,

    .status_code = 0,
    .lightness_range_min = 0,
    .lightness_range_max = 0,
};

static esp_ble_mesh_light_lightness_state_t light_lightness_states_server_setup = {
    .lightness_linear = 0,
    .target_lightness_linear = 0,

    .lightness_actual = 0,
    .target_lightness_actual = 0,

    .lightness_last = 0,
    .lightness_default = 0,

    .status_code = 0,
    .lightness_range_min = 0,
    .lightness_range_max = 0,
};

static esp_ble_mesh_light_lightness_setup_srv_t light_lightness_server_setup_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_lightness_states_server_setup,
};

static esp_ble_mesh_light_lightness_srv_t light_lightness_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = &light_lightness_states_server,
};

void BLE_mesh_light_lightness_model_setup_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "%s", __func__);
    switch (event)
    {
    case ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT");
        break;
    }
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT");
        break;
    }
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT");
        break;
    }
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT");
        break;
    }
    default:
    {
        break;
    }
    }
}

void BLE_mesh_light_server_lightness_server_callback(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    ESP_LOGI(__FILE__, "%s", __func__);
    switch (event)
    {
    case ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT");
        light_lightness_model_server_state_change_event_processor(event, param);
        break;
    }
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT");
        break;
    }
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT");
        break;
    }
    case ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT");
        break;
    }
    default:
    {
        break;
    }
    }
}

void *BLE_mesh_light_server_get_lightness_server_data(uint16_t model_id)
{
    if (ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV == model_id)
    {
        return &light_lightness_server_setup_user_data;
    }
    else if (ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV == model_id)
    {
        return &light_lightness_server_user_data;
    }

    return NULL;
}

static void light_lightness_model_server_state_change_event_processor(esp_ble_mesh_lighting_server_cb_event_t event, esp_ble_mesh_lighting_server_cb_param_t *param)
{
    esp_ble_mesh_server_state_value_t state;
    switch (param->ctx.recv_op)
    {
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET");
        esp_ble_mesh_server_model_send_msg(param->model,
                                           &param->ctx,
                                           ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,
                                           sizeof(light_lightness_states_server.lightness_actual),
                                           (uint8_t *)&light_lightness_states_server.lightness_actual);
        break;
    }
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET:
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET");

        // Change lightness actual value to linear using given formula
        uint16_t lightness = (uint16_t)ceil(65535.0f * pow((double)param->value.state_change.lightness_set.lightness / 65535.0f, 2));
        state.light_lightness_linear.lightness = lightness;
        esp_ble_mesh_server_model_update_state(param->model, ESP_BLE_MESH_LIGHT_LIGHTNESS_LINEAR_STATE, &state);

        // Change generic onoff model value for light lightness server
        state.gen_onoff.onoff = lightness ? 1 : 0;
        esp_ble_mesh_server_model_update_state(param->model, ESP_BLE_MESH_GENERIC_ONOFF_STATE, &state);

        // Change generic level model value for light lightness server
        state.gen_level.level = lightness - 32768;
        esp_ble_mesh_server_model_update_state(param->model, ESP_BLE_MESH_GENERIC_LEVEL_STATE, &state);

        // Finally send acknowledgement response
        esp_ble_mesh_server_model_send_msg(param->model,
                                           &param->ctx,
                                           ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS,
                                           sizeof(light_lightness_states_server.lightness_actual),
                                           (uint8_t *)&light_lightness_states_server.lightness_actual);
        break;
    }
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET");
        esp_ble_mesh_server_model_send_msg(param->model,
                                           &param->ctx,
                                           ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS,
                                           sizeof(light_lightness_states_server.lightness_linear),
                                           (uint8_t *)&light_lightness_states_server.lightness_linear);
        break;
    }
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET:
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET");

        // Change lightness linear value to actual using given formula
        uint16_t lightness = (uint16_t)floor(sqrt((float)param->value.state_change.lightness_linear_set.lightness / 65535.0f) * 65535.0f);
        state.light_lightness_actual.lightness = lightness;
        esp_ble_mesh_server_model_update_state(param->model, ESP_BLE_MESH_LIGHT_LIGHTNESS_ACTUAL_STATE, &state);

        // Change generic onoff model value for light lightness server
        state.gen_onoff.onoff = lightness ? 1 : 0;
        esp_ble_mesh_server_model_update_state(param->model, ESP_BLE_MESH_GENERIC_ONOFF_STATE, &state);

        // Change generic level model value for light lightness server
        state.gen_level.level = lightness - 32768;
        esp_ble_mesh_server_model_update_state(param->model, ESP_BLE_MESH_GENERIC_LEVEL_STATE, &state);

        // Finally send acknowledgement response
        esp_ble_mesh_server_model_send_msg(param->model,
                                           &param->ctx,
                                           ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS,
                                           sizeof(light_lightness_states_server.lightness_linear),
                                           (uint8_t *)&light_lightness_states_server.lightness_linear);
        break;
    }
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET");
        esp_ble_mesh_server_model_send_msg(param->model,
                                           &param->ctx,
                                           ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS,
                                           sizeof(light_lightness_states_server.lightness_last),
                                           (uint8_t *)&light_lightness_states_server.lightness_last);
        break;
    }
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET");
        esp_ble_mesh_server_model_send_msg(param->model,
                                           &param->ctx,
                                           ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS,
                                           sizeof(light_lightness_states_server.lightness_default),
                                           (uint8_t *)&light_lightness_states_server.lightness_default);
        break;
    }
    case ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET:
    {
        ESP_LOGI(__FILE__, "ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET");
        break;
    }
    default:
    {
        ESP_LOGE(__FILE__, "Received opcode is untracked(code: %lu)", param->ctx.recv_op);
        break;
    }
    }
}
