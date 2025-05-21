
#include "esp_log.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_networking_api.h"

#include "BLE_mesh_light_server.h"
#include "BLE_mesh_device_controller.h"

#define TAG "ONOFF-Generic"

static esp_ble_mesh_gen_onoff_srv_t light_lightness_generic_onoff_server_user_data = {
    .rsp_ctrl = {
        .get_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
        .set_auto_rsp = ESP_BLE_MESH_SERVER_RSP_BY_APP,
    },
    .state = {
        .onoff = 1,
        .target_onoff = 0,
    },
};

void BLE_mesh_light_server_generic_on_off_model_callback(esp_ble_mesh_generic_server_cb_event_t event, esp_ble_mesh_generic_server_cb_param_t *param)
{

    /*
    
    TODO: Delay, state transition and optional parameter

    */
    esp_ble_mesh_gen_onoff_srv_t *user_state = (esp_ble_mesh_gen_onoff_srv_t*)param->model->user_data;
    switch (event)
    {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
    {
        ESP_LOGE(TAG, "Generic OnOff server state change event");
        break;
    }
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT:
    {
        ESP_LOGW(TAG, "Generic OnOff server receive get message eventto 0x%04X from 0x%04X", param->ctx.recv_dst, param->ctx.addr);
        switch (param->ctx.recv_op)
        {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET:
        {
            ESP_LOGI(TAG, "Get state message opcode");
            ESP_ERROR_CHECK(esp_ble_mesh_server_model_send_msg(param->model, &param->ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, sizeof(user_state->state.onoff), &user_state->state.onoff));
            break;
        }
        default:
        {
            ESP_LOGE(TAG, "Unknown get msg opcode(Opcode: 0x%04lX)", param->ctx.recv_op);
            break;
        }
        }
        break;
    }
    case ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT:
    {
        ESP_LOGW(TAG, "Generic OnOff server receive set message event to 0x%04X from 0x%04X", param->ctx.recv_dst, param->ctx.addr);
        switch (param->ctx.recv_op)
        {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK:
        {
            ESP_LOGI(TAG, "Generic on off set opcode");
            esp_ble_mesh_server_recv_gen_onoff_set_t on_off_set = param->value.set.onoff;
            BLE_mesh_device_controllet_change_onoff_state(on_off_set.onoff);
            
            user_state->state.onoff = on_off_set.onoff;
            ESP_ERROR_CHECK(esp_ble_mesh_server_model_send_msg(param->model, &param->ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS, sizeof(user_state->state.onoff), &user_state->state.onoff));
            break;
        }
        default:
        {
            ESP_LOGE(TAG, "Unknown opcode received for Generic on off receive set message event(opcode: 0x%04lX)", param->ctx.recv_op);
            break;
        }
        }
        break;
    }
    default:
    {
        ESP_LOGE(TAG, "Unknown event received(event: %d)", event);
        break;
    }
    }
}

void *BLE_mesh_light_server_get_generic_on_off_server_data()
{
    BLE_mesh_device_controllet_change_onoff_state(light_lightness_generic_onoff_server_user_data.state.onoff);
    return &light_lightness_generic_onoff_server_user_data;
}
