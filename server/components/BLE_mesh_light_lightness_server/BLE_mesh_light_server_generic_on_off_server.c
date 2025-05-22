
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_networking_api.h"

#include "BLE_mesh_light_server.h"
#include "BLE_mesh_device_controller.h"

#define TAG "ONOFF-Generic"

static int ble_mesh_begin_onoff_work(esp_ble_mesh_server_recv_gen_onoff_set_t *on_off_set, esp_ble_mesh_gen_onoff_srv_t *user_state);
static void ble_mesh_on_off_optional_worker(struct k_work *work);
static uint32_t ble_mesh_transition_multiplier(uint8_t trans_time);

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
    esp_ble_mesh_gen_onoff_srv_t *user_state = (esp_ble_mesh_gen_onoff_srv_t *)param->model->user_data;
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
            if (bt_mesh_atomic_test_bit(user_state->transition.flag, ESP_BLE_MESH_SERVER_TRANS_TIMER_START))
            {
                k_delayed_work_cancel(&user_state->transition.timer);
                k_delayed_work_free(&user_state->transition.timer);
                bt_mesh_atomic_clear_bit(user_state->transition.flag, ESP_BLE_MESH_SERVER_TRANS_TIMER_START);
            }

            user_state->last.dst = param->ctx.recv_dst;
            user_state->last.src = param->ctx.addr;
            user_state->last.tid = param->value.set.onoff.tid;
            user_state->last.timestamp = esp_timer_get_time();

            esp_ble_mesh_server_recv_gen_onoff_set_t on_off_set = param->value.set.onoff;
            k_delayed_work_init(&light_lightness_generic_onoff_server_user_data.transition.timer, ble_mesh_on_off_optional_worker);
            ble_mesh_begin_onoff_work(&on_off_set, user_state);
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

static int ble_mesh_begin_onoff_work(esp_ble_mesh_server_recv_gen_onoff_set_t *on_off_set, esp_ble_mesh_gen_onoff_srv_t *user_state)
{
    if (0 == on_off_set->delay && !on_off_set->op_en)
    {
        BLE_mesh_device_controllet_change_onoff_state(on_off_set->onoff);
        user_state->state.onoff = on_off_set->onoff;
        return 0;
    }

    user_state->transition.just_started = true;
    if (!on_off_set->op_en)
    {
        user_state->transition.trans_time = 0;
        user_state->transition.quo_tt = 0;
        user_state->transition.counter = 1; // if only transition exist, the callback should run at least once
    }
    else
    {
        user_state->transition.trans_time = on_off_set->trans_time;
        user_state->transition.quo_tt = ble_mesh_transition_multiplier(on_off_set->trans_time);
        user_state->transition.counter = 0x3F & on_off_set->trans_time;
    }
    user_state->transition.delay = on_off_set->delay;
    user_state->transition.total_duration = user_state->transition.quo_tt * user_state->transition.counter;
    user_state->transition.remain_time = user_state->transition.trans_time;
    user_state->transition.start_timestamp = esp_timer_get_time();
    user_state->state.target_onoff = on_off_set->onoff;

    ESP_LOGE(TAG, "On off transition time: 0x%X, delay: 0x%X", on_off_set->trans_time, on_off_set->delay);
    ESP_LOGE(TAG, "User sate transition: delay: %d, total duration: %ld, remaining time: %d, trans_time: 0x%X, quo_tt: %ld, counter: %ld", user_state->transition.delay,
             user_state->transition.total_duration,
             user_state->transition.remain_time,
             user_state->transition.trans_time,
             user_state->transition.quo_tt,
             user_state->transition.counter);

    bt_mesh_atomic_set_bit(user_state->transition.flag, ESP_BLE_MESH_SERVER_TRANS_TIMER_START);
    k_delayed_work_submit(&user_state->transition.timer, K_MSEC(user_state->transition.delay * 5));
    ESP_LOGI(TAG, "Target on off: %d", user_state->state.target_onoff);
    return 0;
}

static void ble_mesh_on_off_optional_worker(struct k_work *work)
{
    esp_ble_mesh_state_transition_t *transition = CONTAINER_OF(work, esp_ble_mesh_state_transition_t, timer.work);

    esp_ble_mesh_gen_onoff_srv_t *on_off_state = CONTAINER_OF(transition, esp_ble_mesh_gen_onoff_srv_t, transition);
    ESP_LOGI(TAG, "On off server state at %p(counter: %ld, delay: %ld)", on_off_state, transition->counter, transition->quo_tt);

    if (!bt_mesh_atomic_test_bit(transition->flag, ESP_BLE_MESH_SERVER_TRANS_TIMER_START))
    {
        ESP_LOGE(TAG, "Bit flag at invalid state!!");
        k_delayed_work_cancel(&transition->timer);
        k_delayed_work_free(&transition->timer);
        bt_mesh_atomic_clear_bit(transition->flag, ESP_BLE_MESH_SERVER_TRANS_TIMER_START);
        return;
    }

    if (transition->just_started)
    {
        transition->just_started = false;
        if (0x01 == on_off_state->state.target_onoff)
        {
            BLE_mesh_device_controllet_change_onoff_state(on_off_state->state.target_onoff);
            on_off_state->state.onoff = on_off_state->state.target_onoff;
        }
    }
    
    if (transition->counter == 0)
    {
        ESP_LOGI(TAG, "Target on off: %d", on_off_state->state.target_onoff);
        if (0x00 == on_off_state->state.target_onoff)
        {
            BLE_mesh_device_controllet_change_onoff_state(on_off_state->state.target_onoff);
            on_off_state->state.onoff = on_off_state->state.target_onoff;
        }
        
        ESP_LOGW(TAG, "Worker operation completed!!");
        k_delayed_work_cancel(&transition->timer);
        k_delayed_work_free((&transition->timer));
        bt_mesh_atomic_clear_bit(transition->flag, ESP_BLE_MESH_SERVER_TRANS_TIMER_START);
        return;
    }
    
    --transition->counter;
    k_delayed_work_submit(&transition->timer, K_MSEC(transition->quo_tt));
}

static uint32_t ble_mesh_transition_multiplier(uint8_t trans_time)
{
    uint32_t multiplier_ms = 0;
    uint8_t mul_indicator = 0xC0 & trans_time;
    switch (mul_indicator)
    {
    case 0x00:
    {
        multiplier_ms = 100;
        break;
    }
    case 0x40:
    {
        multiplier_ms = 1 * 1000;
        break;
    }
    case 0x80:
    {
        multiplier_ms = 10 * 1000;
        break;
    }
    case 0xC0:
    {
        multiplier_ms = 10 * 60 * 1000;
        break;
    }
    default:
    {
        break;
    }
    }

    return multiplier_ms;
}
