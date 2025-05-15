#include <string.h>
#include <stdlib.h>

// ESP non-BLE-MESH APIs
#include "esp_log.h"
#include "esp_err.h"

// Mesh related APIs
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

// Mesh model related APIs
#include "esp_ble_mesh_generic_model_api.h"

// Provisioner APIs
#include "BLE_mesh_prov_definitions.h"
#include "BLE_mesh_prov_initializer.h"

#define TAG "PROV-CB-Handler"

int ble_mesh_provisioning_composition_data_status_processor(esp_ble_mesh_cfg_client_cb_param_t *param);

void ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
    {
        ESP_LOGI(TAG, "Provisioner register completed(status: 0x%02x)", param->prov_register_comp.err_code);
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT:
    {
        ESP_LOGI(TAG, "Provisioner provision enable completed(status: 0x%02x)", param->provisioner_prov_enable_comp.err_code);
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT:
    {
        if (ESP_OK == param->provisioner_add_app_key_comp.err_code)
        {
            ESP_LOGI(TAG, "Provisioner add local app key complete event");
            ESP_LOGI(TAG, "App key idx: %d, Net key idx: %d", param->provisioner_add_app_key_comp.app_idx, param->provisioner_add_app_key_comp.net_idx);
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
    {
        esp_ble_mesh_unprov_dev_add_t unprov_device_info = {
            .addr_type = param->provisioner_recv_unprov_adv_pkt.addr_type,
            .oob_info = param->provisioner_recv_unprov_adv_pkt.oob_info,
            .bearer = param->provisioner_recv_unprov_adv_pkt.bearer,
        };

        memset(unprov_device_info.addr, 0, sizeof(esp_ble_mesh_bd_addr_t));
        memcpy(unprov_device_info.addr, param->provisioner_recv_unprov_adv_pkt.addr, sizeof(esp_ble_mesh_bd_addr_t));

        memset(unprov_device_info.uuid, 0, 16);
        memcpy(unprov_device_info.uuid, param->provisioner_recv_unprov_adv_pkt.dev_uuid, 16);

        ble_mesh_prov_add_unprov_device_info(&unprov_device_info);

        break;
    }
    case ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT:
    {

        ESP_LOGI(TAG, "Provisioner add unprovisioned device completed(status: %d)", param->provisioner_add_unprov_dev_comp.err_code);
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
    {
        ESP_LOGI(TAG, "Provisioner link opened for `%s` bearer", param->provisioner_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "ADV Bearer" : "GATT Bearer");
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
    {
        ESP_LOGI(TAG, "Provisioner link close for `%s` bearer(reason: %d)", param->provisioner_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "ADV Bearer" : "GATT Bearer", param->provisioner_prov_link_close.reason);
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
    {
        ESP_LOGI(TAG, "Provisioner provision complete.");
        ESP_LOGI(TAG, "node index: 0x%x, unicast address: 0x%02x, element num: %d, netkey index: 0x%02x",
                 param->provisioner_prov_complete.node_idx, param->provisioner_prov_complete.unicast_addr,
                 param->provisioner_prov_complete.element_num, param->provisioner_prov_complete.netkey_idx);
        ESP_LOGI(TAG, "device uuid: %s", bt_hex(param->provisioner_prov_complete.device_uuid, 16));

        // First set node name for readability
        char name[16];
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name), "node-%d", param->provisioner_prov_complete.node_idx);
        ESP_ERROR_CHECK(esp_ble_mesh_provisioner_set_node_name(param->provisioner_prov_complete.node_idx, name));

        // Create a database to store node instances of type ble_mesh_node_info_t
        ESP_ERROR_CHECK(ble_mesh_prov_set_node_info(param->provisioner_prov_complete.node_idx,
                                                    param->provisioner_prov_complete.device_uuid,
                                                    param->provisioner_prov_complete.unicast_addr,
                                                    param->provisioner_prov_complete.element_num));
        const uint8_t *net_key = esp_ble_mesh_provisioner_get_local_net_key(param->provisioner_prov_complete.netkey_idx);
        ESP_LOG_BUFFER_HEXDUMP(TAG, net_key, 16, ESP_LOG_ERROR);
        // Populate the database if node does not contain the device
        s_ble_mesh_provisioned_node_info_t *node_info = ble_mesh_prov_get_node_info_unicast_addr(param->provisioner_prov_complete.unicast_addr);
        if (!node_info)
        {
            ESP_LOGE(TAG, "Unable to get node info");
            assert(false);
        }

        // Use the node info structure to get the model's composition
        esp_ble_mesh_cfg_client_get_state_t get_state = {
            /**
             * @brief Page value 0 in composition data get, client returns:
             *  Company ID (16-bit)
             *  Product ID & Version ID (16-bit each)
             *  Number of elements in the node
             *  List of SIG models for each element
             *  List of Vendor models for each element
             *
             */
            .comp_data_get.page = 0x00,
        };

        ESP_ERROR_CHECK(ble_mesh_prov_request_client_state(node_info, &get_state, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET));

        break;
    }
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT:
    {
        ESP_LOGI(TAG, "Provisioner node name set complete(node idx: %d)", param->provisioner_set_node_name_comp.node_index);
        break;
    }
    case ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT:
    {
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT:
    default:
    {
        ESP_LOGE(TAG, "Unhandled event for mesh provisioner callback(event: %d)", event);
        break;
    }
    }
}

void ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event, esp_ble_mesh_cfg_client_cb_param_t *param)
{
    switch (event)
    {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
    {
        ESP_LOGE(TAG, "ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT(err code: %d)(op code: %lx)", param->error_code, param->params->opcode);
        if (ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET == param->params->opcode)
        {
            ble_mesh_provisioning_composition_data_status_processor(param);
        }
        break;
    }
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
    {
        ESP_LOGE(TAG, "ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT");
        break;
    }
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
    {
        ESP_LOGE(TAG, "ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT(err code: %d)(op code: %lx)", param->error_code, param->params->opcode);
        if (ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS == param->params->opcode)
        {
            ble_mesh_provisioning_composition_data_status_processor(param);
        }
        break;
    }
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
    {
        ESP_LOGE(TAG, "ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT, %d(opcode: 0x%04lX(%p))", param->error_code, param->params->opcode, param->params->model);
        break;
    }
    default:
    {
        ESP_LOGE(TAG, "Unhandled event for mesh config client callback(event: %d)", event);
        break;
    }
    }
}

int ble_mesh_provisioning_composition_data_status_processor(esp_ble_mesh_cfg_client_cb_param_t *param)
{
    if (!param)
    {
        return -1;
    }

    struct net_buf_simple *composition_buffer = param->status_cb.comp_data_status.composition_data;
    uint8_t *composition_data = composition_buffer->data;
    uint16_t composition_data_len = composition_buffer->len;
    if (composition_data_len < 7)
    {
        ESP_LOGE(TAG, "Invalid composition data lenght: %d", composition_data_len);
        return -1;
    }

    // Extracted fields (little-endian format)
    // e5 02   (CID)
    // 00 00   (PID)
    // 00 00   (VID)
    // 0a 00   (CRPL)
    // 03 00   (feature)

    // 00 00   (First element ID)
    // 01 00   (SIG model count) (Vendor model count)
    // 00 00   (First SGI model ID)
    // 01 00   (Second element ID)
    // 04 00   (SIG model count) (Vendor model count)
    // 01 13   (First SGI model ID)
    // 00 13   (First SGI model ID)
    // 02 10   (First SGI model ID)
    // 00 10   (First SGI model ID)

    size_t composition_offset = 0;
    uint8_t low_byte = composition_data[composition_offset++];
    uint8_t high_byte = composition_data[composition_offset++];
    uint16_t company_id = (high_byte << 8) | low_byte;

    low_byte = composition_data[composition_offset++];
    high_byte = composition_data[composition_offset++];
    uint16_t product_id = (high_byte << 8) | low_byte;

    low_byte = composition_data[composition_offset++];
    high_byte = composition_data[composition_offset++];
    uint16_t version_id = (high_byte << 8) | low_byte;

    low_byte = composition_data[composition_offset++];
    high_byte = composition_data[composition_offset++];
    uint16_t crpl = (high_byte << 8) | low_byte; //  Cache Relay Protection List(CRPL)

    low_byte = composition_data[composition_offset++];
    high_byte = composition_data[composition_offset++];
    uint16_t feature = (high_byte << 8) | low_byte;

    printf("\n");
    ESP_LOGI(TAG, "Composition data at page %d for 0x%04X", param->status_cb.comp_data_status.page, param->params->ctx.addr);
    ESP_LOGI(TAG, "Company ID: 0x%04X", company_id);
    ESP_LOGI(TAG, "Product ID: 0x%04X", product_id);
    ESP_LOGI(TAG, "Version ID: 0x%04X", version_id);
    ESP_LOGI(TAG, "CRPL ID: 0x%04X", crpl);
    ESP_LOGI(TAG, "Feature ID: 0x%04X", feature);

    s_ble_mesh_provisioned_node_info_t *node = ble_mesh_prov_get_node_info_unicast_addr(param->params->ctx.addr);
    node->company_id = company_id;
    node->product_id = product_id;
    node->version_id = version_id;
    node->crpl = crpl;
    node->features = feature;

    node->elements = (s_ble_mesh_node_element_info_t *)malloc(node->element_num * sizeof(s_ble_mesh_node_element_info_t));
    assert(node->elements);
    memset(node->elements, 0, node->element_num * sizeof(s_ble_mesh_node_element_info_t));
    for (int element_idx = 0; element_idx < node->element_num; element_idx++)
    {

        s_ble_mesh_node_element_info_t *current_element = &node->elements[element_idx];

        low_byte = composition_data[composition_offset++];
        high_byte = composition_data[composition_offset++];
        current_element->element_id = (high_byte << 8) | low_byte;

        current_element->sig_model_count = composition_data[composition_offset++];
        current_element->vendor_model_count = composition_data[composition_offset++];
        ESP_LOGI(TAG, "Reading models for element: 0x%04X(SIG count: %d and Vendor count: %d)", current_element->element_id, current_element->sig_model_count, current_element->vendor_model_count);
        if (current_element->sig_model_count > 0)
        {
            current_element->sig_model_info = (s_ble_mesh_node_element_model_info_t *)malloc(current_element->sig_model_count * sizeof(s_ble_mesh_node_element_model_info_t));
            assert(current_element->sig_model_info);
            for (int sig_idx = 0; sig_idx < current_element->sig_model_count; sig_idx++)
            {
                low_byte = composition_data[composition_offset++];
                high_byte = composition_data[composition_offset++];
                current_element->sig_model_info[sig_idx].sig_model_id = (high_byte << 8) | low_byte;
                ESP_LOGI(TAG, "SIG model %d id is 0x%04X", sig_idx + 1, current_element->sig_model_info[sig_idx].sig_model_id);
            }
        }

        if (current_element->vendor_model_count > 0)
        {
            current_element->vendor_model_info = (s_ble_mesh_node_element_model_info_t *)malloc(current_element->vendor_model_count * sizeof(s_ble_mesh_node_element_model_info_t));
            assert(current_element->vendor_model_info);
            for (int vnd_idx = 0; vnd_idx < current_element->vendor_model_count; vnd_idx++)
            {
                low_byte = composition_data[composition_offset++];
                high_byte = composition_data[composition_offset++];
                current_element->vendor_model_info[vnd_idx].vendor_model_id.model_id = (high_byte << 8) | low_byte;
                current_element->vendor_model_info[vnd_idx].vendor_model_id.company_id = node->company_id;
                ESP_LOGI(TAG, "Vendor model %d id is 0x%04X", vnd_idx + 1, current_element->vendor_model_info[vnd_idx].vendor_model_id.model_id);
            }
        }
    }

    printf("\n");
    return 0;
}
