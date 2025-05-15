
// Standard C-libs
#include <string.h>

// ESP non-BLE-MESH APIs
#include "esp_log.h"
#include "esp_err.h"

// Mesh related APIs
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_random.h"

// Mesh model related APIs
#include "esp_ble_mesh_generic_model_api.h"

// Provisioner APIs
#include "BLE_mesh_prov_definitions.h"
#include "BLE_mesh_prov_initializer.h"

#define TAG "PROV-Init"

#define PROV_OWN_ADDR 0x0001
#define CID_ESP 0x02E5

#define MSG_SEND_TTL 3
#define MSG_TIMEOUT 0
#define MSG_ROLE ROLE_PROVISIONER

extern void ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event, esp_ble_mesh_prov_cb_param_t *param);
extern void ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event, esp_ble_mesh_cfg_client_cb_param_t *param);

static struct esp_ble_mesh_key
{
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t app_key[16];
} prov_key;

static uint8_t dev_uuid[16] = {
    0x03, 0x63, 0xe2, 0x34,
    0xa2, 0x51,
    0x40, 0xcf,
    0xa2, 0x87,
    0xf4, 0xc8, 0x88, 0xe5, 0x64, 0x67};

static esp_ble_mesh_prov_t provision = {
    .prov_uuid = dev_uuid,
    .prov_unicast_addr = PROV_OWN_ADDR,
    .prov_start_address = 0x0005,
    .prov_attention = 0x00,
    .prov_algorithm = 0x00,
    .prov_pub_key_oob = 0x00,
    .prov_static_oob_val = NULL,
    .prov_static_oob_len = 0x00,
    .flags = 0x00,
    .iv_index = 0x00,
};

static esp_ble_mesh_cfg_srv_t config_server = {
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay = ESP_BLE_MESH_RELAY_DISABLED,
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
};

static esp_ble_mesh_client_t config_client;
static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
};

static esp_ble_mesh_comp_t composition = {
    .cid = CID_ESP,
    .element_count = ARRAY_SIZE(elements),
    .elements = elements,
};

s_ble_mesh_provisioner_node_struct_t provisioner_node = {
    .net_idx = ESP_BLE_MESH_KEY_PRIMARY,
    .app_idx = 0x0000,
};

static void ble_mesh_get_dev_uuid(uint8_t *dev_uuid)
{
    if (dev_uuid == NULL)
    {
        ESP_LOGE(TAG, "%s, Invalid device uuid", __func__);
        assert(false);
    }

    /* Copy device address to the device uuid with offset equals to 2 here.
     * The first two bytes is used for matching device uuid by Provisioner.
     * And using device address here is to avoid using the same device uuid
     * by different unprovisioned devices.
     */
    memcpy(dev_uuid + 2, esp_bt_dev_get_address(), BD_ADDR_LEN);
}

int bluetooth_init(void)
{
    esp_err_t ret;

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret)
    {
        ESP_LOGE(TAG, "%s initialize controller failed", __func__);
        return ret;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret)
    {
        ESP_LOGE(TAG, "%s enable controller failed", __func__);
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret)
    {
        ESP_LOGE(TAG, "%s init bluetooth failed", __func__);
        return ret;
    }
    ret = esp_bluedroid_enable();
    if (ret)
    {
        ESP_LOGE(TAG, "%s enable bluetooth failed", __func__);
        return ret;
    }

    return ret;
}

int ble_mesh_prov_init()
{
    uint8_t match[2] = {0xdd, 0xdd};
    esp_err_t err = ESP_OK;

    memset(&provisioner_node, 0, sizeof(provisioner_node));
    for (int i = 0; i < BLE_MESH_APP_KEY_COUNT; i++)
    {
        provisioner_node.app_keys[i].app_key_idx = i;
        esp_fill_random(provisioner_node.app_keys[i].app_key, BLE_MESH_APP_KEY_SIZE);
        ESP_LOG_BUFFER_HEXDUMP("App key", provisioner_node.app_keys[i].app_key, BLE_MESH_APP_KEY_SIZE, ESP_LOG_ERROR);
    }

    provisioner_node.app_idx = 0;
    ESP_ERROR_CHECK(esp_ble_mesh_register_prov_callback(ble_mesh_provisioning_cb));
    ESP_ERROR_CHECK(esp_ble_mesh_register_config_client_callback(ble_mesh_config_client_cb));

    ble_mesh_get_dev_uuid(dev_uuid);
    provision.prov_uuid = dev_uuid;
    err = esp_ble_mesh_init(&provision, &composition);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize mesh stack (err %d)", err);
        return err;
    }

    // err = esp_ble_mesh_provisioner_set_dev_uuid_match(match, sizeof(match), 0x0, false);
    // if (err != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to set matching device uuid (err %d)", err);
    //     return err;
    // }

    return 0;
}

int ble_mesh_provisioning_begin()
{
    esp_err_t err = esp_ble_mesh_provisioner_prov_enable((esp_ble_mesh_prov_bearer_t)(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to enable mesh provisioner (err %d)", err);
        return err;
    }

    err = esp_ble_mesh_provisioner_add_local_app_key(provisioner_node.app_keys[provisioner_node.app_idx].app_key, provisioner_node.net_idx, provisioner_node.app_idx);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add local AppKey (err %d)", err);
        return err;
    }

    ESP_LOGI(TAG, "BLE Mesh Provisioner initialized");
    return 0;
}

int ble_mesh_prov_set_node_info(uint16_t node_idx, uint8_t uuid[16], uint16_t unicast_address, uint8_t elemet_num)
{
    if (node_idx > sizeof(provisioner_node.nodes))
    {
        ESP_LOGI(TAG, "Illegal node_idx expected 0 to %d got %d", sizeof(provisioner_node.nodes), node_idx);
        return -1;
    }

    if (!uuid || !ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_address))
    {
        ESP_LOGE(TAG, "Illegal uuid or unicast address");
        return -1;
    }

    if (0 == memcmp(provisioner_node.nodes[node_idx].uuid, uuid, sizeof(provisioner_node.nodes[node_idx].uuid)))
    {
        ESP_LOGI(TAG, "Device has already been provisioned");
        return 0;
    }

    provisioner_node.nodes[node_idx].element_num = elemet_num;
    provisioner_node.nodes[node_idx].unicast_address = unicast_address;
    provisioner_node.nodes[node_idx].is_provisioned = BLE_MESH_BOOL_TRUE;
    memcpy(provisioner_node.nodes[node_idx].uuid, uuid, sizeof(provisioner_node.nodes[node_idx].uuid));
    ESP_LOGI(TAG, "Node info set complete");

    return 0;
}

uint16_t ble_mesh_get_own_address()
{
    return PROV_OWN_ADDR;
}

int ble_mesh_prov_add_unprov_device_info(esp_ble_mesh_unprov_dev_add_t *unprov_dev_info)
{
    if (!unprov_dev_info)
    {
        ESP_LOGE(TAG, "Invalid un-provisioned device info");
        return -1;
    }

    for (int i = 0; i < BLE_MESH_TOTAL_NODE_COUNT; i++)
    {
        if (provisioner_node.unprov_devices.unprov_device_info[i])
        {
            if (0 == memcmp(provisioner_node.unprov_devices.unprov_device_info[i]->addr, unprov_dev_info->addr, sizeof(esp_ble_mesh_bd_addr_t)))
            {
                return 1;
            }
        }
    }

    ESP_LOGI(TAG, "Provisioner received unprovisioned advertisement packet");
    ESP_LOGI(TAG, "address: %s, address type: %d", bt_hex(unprov_dev_info->addr, BD_ADDR_LEN), unprov_dev_info->addr_type);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(unprov_dev_info->uuid, 16));
    ESP_LOGI(TAG, "oob info: %d, bearer: (%d)%s", unprov_dev_info->oob_info, unprov_dev_info->bearer, (unprov_dev_info->bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");
    for (int i = 0; i < BLE_MESH_TOTAL_NODE_COUNT; i++)
    {
        if (!provisioner_node.unprov_devices.unprov_device_info[i])
        {
            provisioner_node.unprov_devices.unprov_device_info[i] = (esp_ble_mesh_unprov_dev_add_t *)malloc(sizeof(esp_ble_mesh_unprov_dev_add_t));
            if (!provisioner_node.unprov_devices.unprov_device_info[i])
            {
                ESP_LOGE(TAG, "Unable to allocate memory for unprovisioned device info");
                return -1;
            }

            memset(provisioner_node.unprov_devices.unprov_device_info[i], 0, sizeof(esp_ble_mesh_unprov_dev_add_t));
            memcpy(provisioner_node.unprov_devices.unprov_device_info[i], unprov_dev_info, sizeof(esp_ble_mesh_unprov_dev_add_t));
            break;
        }
    }
    return 0;
}

void ble_mesh_provision_start_dev_prov_with_idx(uint16_t idx)
{
    if (provisioner_node.unprov_devices.unprov_device_info[idx])
    {
        esp_err_t err = esp_ble_mesh_provisioner_add_unprov_dev(provisioner_node.unprov_devices.unprov_device_info[idx],
                                                                (esp_ble_mesh_dev_add_flag_t)(ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_START_PROV_NOW_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG));

        if (err)
        {
            ESP_LOGE(TAG, "%s: Add unprovisioned device into queue failed", __func__);
        }
        else
        {
            ESP_LOGE(TAG, "Freeing device");
            free(provisioner_node.unprov_devices.unprov_device_info[idx]);
            provisioner_node.unprov_devices.unprov_device_info[idx] = NULL;
        }
    }
}

void ble_mesh_prov_list_unprov_devices()
{
    printf("**********************************************\n");
    for (int i = 0; i < BLE_MESH_TOTAL_NODE_COUNT; i++)
    {
        if (provisioner_node.unprov_devices.unprov_device_info[i])
        {
            printf("******************** Device id %d ********************\n", i);
            printf("Address: %s\nAddress type: %d\n", bt_hex(provisioner_node.unprov_devices.unprov_device_info[i]->addr, BD_ADDR_LEN), provisioner_node.unprov_devices.unprov_device_info[i]->addr_type);
            printf("Device UUID: %s\n", bt_hex(provisioner_node.unprov_devices.unprov_device_info[i]->uuid, 16));
            printf("OOB info: %d\nbearer: %s\n", provisioner_node.unprov_devices.unprov_device_info[i]->oob_info, (provisioner_node.unprov_devices.unprov_device_info[i]->bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");
        }
    }
    printf("**********************************************\n");
}

void ble_mesh_prov_list_prov_devices()
{
    printf("**********************************************\n");
    for (int i = 0; i < BLE_MESH_TOTAL_NODE_COUNT; i++)
    {
        if (BLE_MESH_BOOL_TRUE == provisioner_node.nodes[i].is_provisioned)
        {
            printf("******************** Node id %d ********************\n", i);
            printf("UUID: %s\n", bt_hex(provisioner_node.nodes[i].uuid, 16));
            printf("Company ID: 0x%04X\n", provisioner_node.nodes[i].company_id);
            printf("Product ID: 0x%04X\n", provisioner_node.nodes[i].product_id);
            printf("Version ID: 0x%04X\n", provisioner_node.nodes[i].version_id);
            printf("CRPL ID: 0x%04X\n", provisioner_node.nodes[i].crpl);
            printf("Features ID: 0x%04X\n", provisioner_node.nodes[i].features);
            printf("Unicast address: 0x%04X\n", provisioner_node.nodes[i].unicast_address);
            printf("Num. of elements: 0x%04X\n", provisioner_node.nodes[i].element_num);
        }
    }
    printf("**********************************************\n");
}

void ble_mesh_provision_get_node_composition_data(uint16_t idx)
{
    return;
}

s_ble_mesh_provisioned_node_info_t *ble_mesh_prov_get_node_info_unicast_addr(uint16_t unicast_addr)
{
    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast_addr))
    {
        ESP_LOGE(TAG, "Illegal uuid or unicast address");
        return NULL;
    }

    for (int i = 0; i < sizeof(provisioner_node.nodes); i++)
    {
        if (provisioner_node.nodes[i].unicast_address == unicast_addr)
        {
            return &provisioner_node.nodes[i];
        }
    }

    ESP_LOGE(TAG, "Cannot find node info with given unicast addr(0x%04X)", unicast_addr);
    return NULL;
}

s_ble_mesh_provisioned_node_info_t *ble_mesh_prov_get_node_info_idx(uint16_t idx)
{
    if (BLE_MESH_BOOL_TRUE == provisioner_node.nodes[idx].is_provisioned)
    {
        return &provisioner_node.nodes[idx];
    }

    return NULL;
}

int ble_mesh_prov_request_client_state(
    s_ble_mesh_provisioned_node_info_t *node_info,
    esp_ble_mesh_cfg_client_get_state_t *get_state,
    uint32_t opcode)
{
    if (!node_info)
    {
        ESP_LOGE(TAG, "Invalid node info");
        return -1;
    }

    esp_ble_mesh_client_common_param_t common = {
        .opcode = opcode,
        .model = config_client.model,
        .ctx.net_idx = provisioner_node.net_idx,
        .ctx.app_idx = provisioner_node.app_idx,
        .ctx.addr = node_info->unicast_address,
        .ctx.send_ttl = MSG_SEND_TTL,
        .msg_timeout = MSG_TIMEOUT,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
        common->msg_role = MSG_ROLE;
#endif
};

ESP_ERROR_CHECK(esp_ble_mesh_config_client_get_state(&common, get_state));
ESP_LOGI(TAG, "Client get state opcode dispatched!!");
return 0;
}
