
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_bt.h"
#include "esp_mac.h"
#include "esp_chip_info.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"
#include "ble_mesh_example_init.h"

// Servers headers for different SIG models
#include "esp_ble_mesh_lighting_model_api.h"
#include "esp_ble_mesh_time_scene_model_api.h"
#include "esp_ble_mesh_sensor_model_api.h"
#include "mesh/access.h" // This holds BLE_MESH_MODEL_VND_CB and BLE_MESH_MODEL_CB

#include "BLE_mesh_models.h"
#include "BLE_mesh_initializer.h"
#include "BLE_mesh_definitions.h"
#include "BLE_mesh_prov_indicator.h"

#define TAG "BLE_mesh_initializer"

static uint8_t dev_uuid[ESP_BLE_MESH_OCTET16_LEN] = {0x32, 0x10};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEMENT(0, light_lightness_server_models, ESP_BLE_MESH_MODEL_NONE),
};

static esp_ble_mesh_comp_t composition = {
    .cid = CID_ESP,
    .element_count = ARRAY_SIZE(elements),
    .elements = elements,
};

static esp_ble_mesh_prov_t provision = {
    .uuid = dev_uuid,
};

static s_custom_model_user_data_struct_t *ble_mesh_sig_custom_model_user_args[10];

static void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    ESP_LOGI(TAG, "net_idx 0x%03x, addr 0x%04x", net_idx, addr);
    ESP_LOGI(TAG, "flags 0x%02x, iv_index 0x%08lx" PRIx32, flags, iv_index);
}

static void ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                     esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code %d", param->prov_register_comp.err_code);
        // BLE_mesh_indicator_init();
        break;
    }
    case ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT, err_code %d", param->node_prov_enable_comp.err_code);
        break;
    }
    case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT, bearer %s",
                 param->node_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        // BLE_mesh_prov_link_open_indicate();
        break;
    }
    case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT, bearer %s",
                 param->node_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    }
    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT");
        prov_complete(param->node_prov_complete.net_idx, param->node_prov_complete.addr,
                      param->node_prov_complete.flags, param->node_prov_complete.iv_index);
        // BLE_mesh_prov_complete_indicate();
        break;
    }
    case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_RESET_EVT");
        break;
    }
    case ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT, err_code %d", param->node_set_unprov_dev_name_comp.err_code);
        break;
    }
    /*
        After successfult subscription of an address using `esp_ble_mesh_model_subscribe_group_addr(...)` function, the This event is triggered.
    */
    case ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT on model 0x%x for group 0x%x with error code %d", param->model_sub_group_addr_comp.model_id, param->model_sub_group_addr_comp.group_addr, param->model_sub_group_addr_comp.err_code);
        break;
    }
    /*
        After successfult subscription of an address using `esp_ble_mesh_model_unsubscribe_group_addr(...)` function, the This event is triggered.
    */
    case ESP_BLE_MESH_MODEL_UNSUBSCRIBE_GROUP_ADDR_COMP_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT on model 0x%x for group 0x%x with error code %d", param->model_unsub_group_addr_comp.model_id, param->model_unsub_group_addr_comp.group_addr, param->model_unsub_group_addr_comp.err_code);
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
                                      esp_ble_mesh_cfg_server_cb_param_t *param)
{
    if (event == ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT)
    {
        switch (param->ctx.recv_op)
        {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
        {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD");
            ESP_LOGI(TAG, "net_idx 0x%04x, app_idx 0x%04x",
                     param->value.state_change.appkey_add.net_idx,
                     param->value.state_change.appkey_add.app_idx);
            ESP_LOG_BUFFER_HEX("AppKey", param->value.state_change.appkey_add.app_key, 16);
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
        {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND");
            ESP_LOGI(TAG, "elem_addr 0x%04x, app_idx 0x%04x, cid 0x%04x, mod_id 0x%04x",
                     param->value.state_change.mod_app_bind.element_addr,
                     param->value.state_change.mod_app_bind.app_idx,
                     param->value.state_change.mod_app_bind.company_id,
                     param->value.state_change.mod_app_bind.model_id);
            break;
        }
        /*
            When set publication function is called for a model, this event is triggerred.
            If pub message is NULL and message len is 0, an error will be thrown.
        */
        case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET:
        {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET for model id 0x%x to the address 0x%x", param->value.state_change.mod_pub_set.model_id, param->value.state_change.mod_pub_set.pub_addr);

            esp_ble_mesh_model_t *model = NULL;
            esp_ble_mesh_elem_t *element = esp_ble_mesh_find_element(param->value.state_change.mod_pub_set.element_addr);
            if (element)
            {
                for (int i = 0; i < element->sig_model_count; i++)
                {
                    if (element->sig_models[i].model_id == param->value.state_change.mod_pub_set.model_id)
                    {
                        model = &element->sig_models[i];
                    }
                }
                if (!model)
                {
                    for (int i = 0; i < element->vnd_model_count; i++)
                    {
                        if (element->vnd_models[i].vnd.model_id == param->value.state_change.mod_pub_set.model_id)
                        {
                            model = &element->vnd_models[i];
                        }
                    }
                }
            }

            if (model)
            {
                uint8_t data[1] = {0x01};
                model->pub->msg->data = data;
                model->pub->msg->len = sizeof(data);
            }
            break;
        }
        /*
            This event indicates that the model wants to subscribe to a address.
            Therefor it is mandatory to call the `esp_ble_mesh_model_subscribe_group_addr(...)` function inorder to subscribe the address.
        */
        case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD:
        {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD for model id 0x%x to the address 0x%x", param->value.state_change.mod_sub_add.model_id, param->value.state_change.mod_sub_add.sub_addr);
            esp_ble_mesh_model_subscribe_group_addr(
                param->value.state_change.mod_sub_add.element_addr,
                param->value.state_change.mod_sub_add.company_id,
                param->value.state_change.mod_sub_add.model_id,
                param->value.state_change.mod_sub_add.sub_addr);
            break;
        }
        /*
            This event indicates that the mmodel wants to unsubscribe to an already subscribed address.
            Therefor call the `esp_ble_mesh_model_unsubscribe_group_addr(...)` function inorder to unsubscribe the address.
        */
        case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE:
        {
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD for model id 0x%x to the address 0x%x", param->value.state_change.mod_sub_delete.model_id, param->value.state_change.mod_sub_delete.sub_addr);
            esp_ble_mesh_model_unsubscribe_group_addr(
                param->value.state_change.mod_sub_delete.element_addr,
                param->value.state_change.mod_sub_delete.company_id,
                param->value.state_change.mod_sub_delete.model_id,
                param->value.state_change.mod_sub_delete.sub_addr);
            break;
        }
        default:
        {
            break;
        }
        }
    }
}

static void ble_mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event,
                                     esp_ble_mesh_model_cb_param_t *param)
{
    switch (event)
    {
    /*
        This event is triggered when any of the custom model received data from the sender.
        Date can be using one to one connection, or through broadcast using unicast, multicast or virtual address.
        Since, all kind of incoming message triggers this event it is required to differentiate between those addresses.
        Only the opcode that are registered in opcode array triggers the event, else the message is ignored by the accesslayer.
    */
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
    {
        ESP_LOGI(TAG, "Operation event from 0x%x to 0x%x to model 0x%x", param->model_operation.ctx->addr, param->model_operation.ctx->recv_dst, param->model_operation.model->model_id);
        if (ESP_BLE_MESH_ADDR_IS_GROUP(param->model_operation.ctx->recv_dst) && esp_ble_mesh_is_model_subscribed_to_group(param->model_operation.model, param->model_operation.ctx->recv_dst))
        {
            ESP_LOGI(TAG, "Incoming subscribed message len for model id 0x%x on opcode 0x%lx is %d", param->model_operation.model->model_id, param->model_operation.opcode, param->model_operation.length);
            if (param->model_operation.length == 1)
            {
                uint8_t data = *(uint8_t *)param->model_operation.msg;
                printf("Message is %d\n", data);
            }
        }
        else if (ESP_BLE_MESH_ADDR_IS_UNICAST(param->model_operation.ctx->recv_dst))
        {
            s_custom_model_user_data_struct_t *model_user_args = (s_custom_model_user_data_struct_t *)param->model_operation.model->user_data;
            if (model_user_args && model_user_args->model_cb.custom_model_cb)
            {
                printf("%s[%d]: Calling model callback at %p for model id 0x%x\n", __FILE__, __LINE__, model_user_args->model_cb.custom_model_cb, param->model_operation.model->vnd.model_id);
                model_user_args->model_cb.custom_model_cb(param, model_user_args->args);
            }
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
    {
        if (param->model_send_comp.err_code)
        {
            ESP_LOGE(TAG, "Failed to send message 0x%06" PRIx32, param->model_send_comp.opcode);
            break;
        }
        ESP_LOGI(TAG, "Send 0x%06" PRIx32, param->model_send_comp.opcode);
        break;
    }
    /*
        After publish update event, on calling `esp_ble_mesh_model_publish(...)`, this event is triggered to indicate the status of the publication i.e. success, or any other error occured.
    */
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
    {
        ESP_LOGI(TAG, "Publish completed for model id %d with error: %d", param->model_publish_comp.model->vnd.model_id, param->model_publish_comp.err_code);
        break;
    }
    /*
        This event is triggered for custom model if publication timer goes off. For example if publication period is set to 5sec, this event is triggred every 5 sec.
        If pub message is NULL and message len is 0, an error will be thrown.
    */
    case ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT");
        s_custom_model_user_data_struct_t *model_user_args = (s_custom_model_user_data_struct_t *)param->model_publish_update.model->user_data;
        if (model_user_args && model_user_args->publication_cb.custom_publication_cb)
        {
            model_user_args->publication_cb.custom_publication_cb(param->model_publish_update.model, model_user_args->args);
        }
        break;
    }
    case ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT:
    {
        ESP_LOGI(TAG, "ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT");
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ble_mesh_generic_model_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                      esp_ble_mesh_generic_server_cb_param_t *param)
{
    for (int i = 0; i < 10; i++)
    {
        if (ble_mesh_sig_custom_model_user_args[i] && ble_mesh_sig_custom_model_user_args[i]->model_id == param->model->model_id)
        {
            ble_mesh_sig_custom_model_user_args[i]->model_cb.sig_generic_model_cb(event, param);
        }
    }
}

static void esp_ble_mesh_lighting_model_cb(esp_ble_mesh_lighting_server_cb_event_t event,
                                           esp_ble_mesh_lighting_server_cb_param_t *param)
{
    for (int i = 0; i < 10; i++)
    {
        if (ble_mesh_sig_custom_model_user_args[i] && ble_mesh_sig_custom_model_user_args[i]->model_id == param->model->model_id)
        {
            ble_mesh_sig_custom_model_user_args[i]->model_cb.sig_lighting_model_cb(event, param);
        }
    }
}

static char *get_device_name(esp_chip_model_t chip_model)
{
    char *device_name = NULL;
    switch (chip_model)
    {
    case CHIP_ESP32:
    {
        device_name = "ESP32";
        break;
    }
    case CHIP_ESP32S3:
    {
        device_name = "ESP32S3";
        break;
    }
    case CHIP_ESP32C3:
    {
        device_name = "ESP32C3";
        break;
    }
    case CHIP_ESP32H2:
    {
        device_name = "ESP32H2";
        break;
    }
    default:
    {
        break;
    }
    }
    return device_name;
}

esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    ble_mesh_get_dev_uuid(dev_uuid);

    esp_ble_mesh_register_prov_callback(ble_mesh_provisioning_cb);
    esp_ble_mesh_register_config_server_callback(ble_mesh_config_server_cb);
    esp_ble_mesh_register_custom_model_callback(ble_mesh_custom_model_cb);
    esp_ble_mesh_register_generic_server_callback(ble_mesh_generic_model_cb);
    esp_ble_mesh_register_lighting_server_callback(esp_ble_mesh_lighting_model_cb);

    err = esp_ble_mesh_init(&provision, &composition);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize mesh stack");
        return err;
    }

    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    char name[32];
    memset(name, 0, sizeof(name));
    snprintf(name, sizeof(name), "%s_%x_%x_%x", get_device_name(chip_info.model), mac[3], mac[4], mac[5]);

    err = esp_ble_mesh_set_unprovisioned_device_name(name);
    if (err != ESP_OK)
    {
        printf("Failed to set unprovisioned node's name\n");
        return err;
    }

    err = esp_ble_mesh_node_prov_enable((esp_ble_mesh_prov_bearer_t)(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to enable mesh node");
        return err;
    }

    ESP_LOGI(TAG, "BLE Mesh Node initialized");

    return ESP_OK;
}

int ble_mesh_register_vendor_model_user_args(uint16_t element_id, uint16_t model_id, custom_model_callback model_callback,
                                             custom_model_publication_callback publication_callback, void *args)
{
    if (element_id >= composition.element_count)
    {
        ESP_LOGE(TAG, "Invalid element id; element id should be less than %d", composition.element_count);
        return -1;
    }

    size_t model_size = elements[element_id].vnd_model_count;
    if ((model_size > elements[element_id].vnd_model_count) || (0 == model_id))
    {
        ESP_LOGE(TAG, "Invalid model id; model id should be greater than 0 and less than %d", model_size);
        return -1;
    }

    for (int i = 0; i < model_size; i++)
    {
        if (elements[element_id].vnd_models[i].vnd.model_id == model_id)
        {
            s_custom_model_user_data_struct_t *model_user_data = (s_custom_model_user_data_struct_t *)malloc(sizeof(s_custom_model_user_data_struct_t));
            if (!model_user_data)
            {
                ESP_LOGE(TAG, "Error while allocating memory for model(id: %d) user args.", model_id);
                return -1;
            }
            memset(model_user_data, 0, sizeof(s_custom_model_user_data_struct_t));
            model_user_data->model_id = model_id;
            model_user_data->model_cb.custom_model_cb = model_callback;
            model_user_data->publication_cb.custom_publication_cb = publication_callback;
            model_user_data->args = NULL;

            elements[element_id].vnd_models[i].user_data = (void *)model_user_data;
            printf("%s[%d]: User data registered for model id 0x%x, callbacks at (model: %p and pub: %p)\n", __FILE__, __LINE__, model_id, model_callback, publication_callback);
        }
    }
    return 0;
}

int ble_mesh_register_sig_lighting_model_user_args(uint16_t element_id, uint16_t model_id, sig_lighting_model_callback model_callback, void *args)
{
    if (element_id >= composition.element_count)
    {
        ESP_LOGE(TAG, "Invalid element id; element id should be less than %d", composition.element_count);
        return -1;
    }

    size_t model_size = elements[element_id].sig_model_count;
    if ((model_size > elements[element_id].sig_model_count) || (0 == model_id))
    {
        ESP_LOGE(TAG, "Invalid model id; model id should be greater than 0 and less than %d", model_size);
        return -1;
    }

    for (int i = 0; i < model_size; i++)
    {
        if (elements[element_id].sig_models[i].model_id == model_id)
        {
            s_custom_model_user_data_struct_t *model_user_data = (s_custom_model_user_data_struct_t *)malloc(sizeof(s_custom_model_user_data_struct_t));
            if (!model_user_data)
            {
                ESP_LOGE(TAG, "Error while allocating memory for model(id: %d) user args.", model_id);
                return -1;
            }
            memset(model_user_data, 0, sizeof(s_custom_model_user_data_struct_t));
            model_user_data->model_id = model_id;
            model_user_data->model_cb.sig_lighting_model_cb = model_callback;
            model_user_data->publication_cb.custom_publication_cb = NULL;
            model_user_data->args = NULL;

            elements[element_id].sig_models[i].user_data = args;

            for (int i = 0; i < sizeof(ble_mesh_sig_custom_model_user_args); i++)
            {
                if (NULL == ble_mesh_sig_custom_model_user_args[i])
                {
                    ble_mesh_sig_custom_model_user_args[i] = model_user_data;
                    break;
                }
            }
        }
    }
    return 0;
}

int ble_mesh_register_sig_generic_model_user_args(uint16_t element_id, uint16_t model_id, sig_generic_model_callback model_callback, void *args)
{
    if (element_id >= composition.element_count)
    {
        ESP_LOGE(TAG, "Invalid element id; element id should be less than %d", composition.element_count);
        return -1;
    }

    size_t model_size = elements[element_id].sig_model_count;
    if ((model_size > elements[element_id].sig_model_count) || (0 == model_id))
    {
        ESP_LOGE(TAG, "Invalid model id; model id should be greater than 0 and less than %d", model_size);
        return -1;
    }

    for (int i = 0; i < model_size; i++)
    {
        if (elements[element_id].sig_models[i].model_id == model_id)
        {
            s_custom_model_user_data_struct_t *model_user_data = (s_custom_model_user_data_struct_t *)malloc(sizeof(s_custom_model_user_data_struct_t));
            if (!model_user_data)
            {
                ESP_LOGE(TAG, "Error while allocating memory for model(id: %d) user args.", model_id);
                return -1;
            }
            memset(model_user_data, 0, sizeof(s_custom_model_user_data_struct_t));
            model_user_data->model_id = model_id;
            model_user_data->model_cb.sig_generic_model_cb = model_callback;
            model_user_data->publication_cb.custom_publication_cb = NULL;
            model_user_data->args = NULL;

            elements[element_id].sig_models[i].user_data = args;
            for (int i = 0; i < sizeof(ble_mesh_sig_custom_model_user_args); i++)
            {
                if (NULL == ble_mesh_sig_custom_model_user_args[i])
                {
                    ble_mesh_sig_custom_model_user_args[i] = model_user_data;
                    break;
                }
            }
        }
    }
    return 0;
}
