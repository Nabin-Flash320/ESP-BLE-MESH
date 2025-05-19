
#include <string.h>

#include "esp_ble_mesh_defs.h"

#include "provisioner.h"
#include "provisioner_definitions.h"

#define TAG "PROV-Info"

static struct provisioner_node_info
{
    s_provisioner_unprov_dev_info_t unprov_dev_info;
    s_provisioner_prov_node_info_t *prov_node_info[PROV_MAX_PROV_DEV_CNT];
} provisioner_node_info;

int provisioner_add_unprov_dev_info(esp_ble_mesh_unprov_dev_add_t *unprov_device_info)
{
    if (!unprov_device_info)
    {
        return -1;
    }

    for (int i = 0; i < PROV_MAX_UNPROV_DEV_CNT; i++)
    {
        if (provisioner_node_info.unprov_dev_info.unprov_device_info[i])
        {
            if (0 == memcmp(provisioner_node_info.unprov_dev_info.unprov_device_info[i]->addr, unprov_device_info->addr, sizeof(esp_ble_mesh_bd_addr_t)))
            {
                break;
            }

            continue;
        }

        provisioner_node_info.unprov_dev_info.unprov_device_info[i] = (esp_ble_mesh_unprov_dev_add_t *)malloc(sizeof(esp_ble_mesh_unprov_dev_add_t));
        if (!provisioner_node_info.unprov_dev_info.unprov_device_info[i])
        {
            return -1;
        }

        memset(provisioner_node_info.unprov_dev_info.unprov_device_info[i], 0, sizeof(esp_ble_mesh_unprov_dev_add_t));
        memcpy(provisioner_node_info.unprov_dev_info.unprov_device_info[i], unprov_device_info, sizeof(esp_ble_mesh_unprov_dev_add_t));

        ESP_LOGI(TAG, "address: %s, address type: %d", bt_hex(provisioner_node_info.unprov_dev_info.unprov_device_info[i]->addr, BD_ADDR_LEN), provisioner_node_info.unprov_dev_info.unprov_device_info[i]->addr_type);
        ESP_LOGI(TAG, "device uuid: %s", bt_hex(provisioner_node_info.unprov_dev_info.unprov_device_info[i]->uuid, 16));
        ESP_LOGI(TAG, "oob info: %d, bearer: %s", provisioner_node_info.unprov_dev_info.unprov_device_info[i]->oob_info, (provisioner_node_info.unprov_dev_info.unprov_device_info[i]->bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");

        break;
    }

    return 0;
}

int provisioner_add_prov_dev_info(const uint8_t uuid[16], uint16_t unicast, uint8_t elem_num)
{
    if (!uuid || !ESP_BLE_MESH_ADDR_IS_UNICAST(unicast))
    {
        return -1;
    }

    for (int i = 0; i < PROV_MAX_PROV_DEV_CNT; i++)
    {
        if (provisioner_node_info.prov_node_info[i])
        {
            continue;
        }

        provisioner_node_info.prov_node_info[i] = (s_provisioner_prov_node_info_t *)malloc(sizeof(s_provisioner_prov_node_info_t));
        if (!provisioner_node_info.prov_node_info[i])
        {
            return -1;
        }

        memset(provisioner_node_info.prov_node_info[i], 0, sizeof(s_provisioner_prov_node_info_t));
        provisioner_node_info.prov_node_info[i]->unicast_address = unicast;
        provisioner_node_info.prov_node_info[i]->element_num = elem_num;
        memcpy(provisioner_node_info.prov_node_info[i]->uuid, uuid, 16);

        memset(provisioner_node_info.prov_node_info[i]->name, 0, 32);
        snprintf(provisioner_node_info.prov_node_info[i]->name, 32, "Node-%d", i);
        break;
    }

    return 0;
}

void provisioner_log_unprov_dev_info()
{
    printf("************************* Unprovisioned devices ************************\n");
    for (int i = 0; i < PROV_MAX_UNPROV_DEV_CNT; i++)
    {
        esp_ble_mesh_unprov_dev_add_t *dev = provisioner_node_info.unprov_dev_info.unprov_device_info[i];
        if (dev)
        {
            printf("************************* Device Id: %d *************************\n", i);
            printf("Address: %X:%X:%X:%X:%X:%X\n", dev->addr[0], dev->addr[1], dev->addr[2], dev->addr[3], dev->addr[4], dev->addr[5]);
            printf("Address type: 0x%X\nUUID: %s\nOOB info: %d\nBearer: %s\n", dev->addr_type, bt_hex(dev->uuid, 16), dev->oob_info, (dev->bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");
        }
    }

    printf("********************************************************\n");
}

void provisioner_log_prov_dev_info()
{
    printf("************************* provisioned devices ************************\n");
    for (int node_idx = 0; node_idx < PROV_MAX_PROV_DEV_CNT; node_idx++)
    {
        s_provisioner_prov_node_info_t *node = provisioner_node_info.prov_node_info[node_idx];
        if (node)
        {
            printf("************************* Node Id: %d *************************\n", node_idx);
            printf("Name: %s\n", node->name);
            printf("UUID: %s\n", bt_hex(node->uuid, 16));
            printf("Company ID: 0x%04X\n", node->cid);
            printf("Product ID: 0x%04X\n", node->pid);
            printf("Version ID: 0x%04X\n", node->vid);
            printf("CRPL ID: 0x%04X\n", node->crpl);
            printf("Features ID: 0x%04X\n", node->features);
            printf("Unicast address: 0x%04X\n", node->unicast_address);
            printf("Num. of elements: 0x%04X\n", node->element_num);
            for(int elem_idx = 0; elem_idx < node->element_num; elem_idx++)
            {
                printf("\tElement id: %d\n", node->elements[elem_idx].element_id);
                printf("\tSIG model count: 0x%04X\n", node->elements[elem_idx].sig_model_count);
                for(int sig_cnt = 0; sig_cnt < node->elements[elem_idx].sig_model_count; sig_cnt++)
                {
                    s_provisioner_model_info_t *sig_model = &node->elements[elem_idx].sig_models[sig_cnt];
                    printf("\t\t(%d) Model id: 0x%04X\n", sig_cnt, sig_model->model_id.sig_model_id);
                }

                printf("\tVendor model count: %d\n", node->elements[elem_idx].vnd_model_count);
                for(int vnd_cnt = 0; vnd_cnt < node->elements[elem_idx].vnd_model_count; vnd_cnt++)
                {
                    s_provisioner_model_info_t *vnd_model = &node->elements[elem_idx].vnd_models[vnd_cnt];
                    printf("\t\t(%d) Model id: 0x%04X\n", vnd_cnt, vnd_model->model_id.vnd_model_id.model_id);
                }
            }
        }
    }

    printf("************************************************************************\n");
}

esp_ble_mesh_unprov_dev_add_t *get_unprov_dev(uint8_t idx)
{
    if (idx >= PROV_MAX_UNPROV_DEV_CNT)
    {
        ESP_LOGE(TAG, "Unprovisioned device index out of range!!");
        return NULL;
    }

    return provisioner_node_info.unprov_dev_info.unprov_device_info[idx];
}

void remove_unprov_dev(uint8_t idx)
{
    if (idx >= PROV_MAX_UNPROV_DEV_CNT)
    {
        ESP_LOGE(TAG, "Unprovisioned device index out of range!!");
        return;
    }

    if (provisioner_node_info.unprov_dev_info.unprov_device_info[idx])
    {
        ESP_LOGI(TAG, "Removing unprov device info at %d", idx);
        free(provisioner_node_info.unprov_dev_info.unprov_device_info[idx]);
        provisioner_node_info.unprov_dev_info.unprov_device_info[idx] = NULL;
    }

    return;
}

int parse_composition_data(struct net_buf_simple *composition_buffer, uint16_t unicast)
{
    if (!composition_buffer)
    {
        ESP_LOGE(TAG, "Invalid net buf param provided");
        return -1;
    }

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast))
    {
        ESP_LOGE(TAG, "Invalid unicast address provided!!");
        return -1;
    }

    uint8_t *composition_data = composition_buffer->data;
    size_t composition_data_len = composition_buffer->len;
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
    ESP_LOGI(TAG, "Company ID: 0x%04X", company_id);
    ESP_LOGI(TAG, "Product ID: 0x%04X", product_id);
    ESP_LOGI(TAG, "Version ID: 0x%04X", version_id);
    ESP_LOGI(TAG, "CRPL ID: 0x%04X", crpl);
    ESP_LOGI(TAG, "Feature ID: 0x%04X", feature);

    s_provisioner_prov_node_info_t *node = NULL;
    for (int i = 0; i < PROV_MAX_PROV_DEV_CNT; i++)
    {
        if (provisioner_node_info.prov_node_info[i])
        {
            if (provisioner_node_info.prov_node_info[i]->unicast_address == unicast)
            {
                node = provisioner_node_info.prov_node_info[i];
                break;
            }
        }
    }

    if (!node)
    {
        ESP_LOGE(TAG, "Node not found");
        return -1;
    }

    node->cid = company_id;
    node->pid = product_id;
    node->vid = version_id;
    node->crpl = crpl;
    node->features = feature;
    node->elements = (s_provisioner_element_info_t *)malloc(node->element_num * sizeof(s_provisioner_element_info_t));
    if (!node->elements)
    {
        ESP_LOGE(TAG, "Unable to allocate memory to store elements info");
        return -1;
    }

    memset(node->elements, 0, node->element_num * sizeof(s_provisioner_element_info_t));
    for (int element_idx = 0; element_idx < node->element_num; element_idx++)
    {
        s_provisioner_element_info_t *current_element = &node->elements[element_idx];

        low_byte = composition_data[composition_offset++];
        high_byte = composition_data[composition_offset++];
        current_element->element_id = (high_byte << 8) | low_byte;

        current_element->sig_model_count = composition_data[composition_offset++];
        current_element->vnd_model_count = composition_data[composition_offset++];
        ESP_LOGI(TAG, "Reading models for element: 0x%04X(SIG count: %d and Vendor count: %d)", current_element->element_id, current_element->sig_model_count, current_element->vnd_model_count);
        if (current_element->sig_model_count > 0)
        {
            current_element->sig_models = (s_provisioner_model_info_t *)malloc(current_element->sig_model_count * sizeof(s_provisioner_model_info_t));
            assert(current_element->sig_models);
            for (int sig_idx = 0; sig_idx < current_element->sig_model_count; sig_idx++)
            {
                low_byte = composition_data[composition_offset++];
                high_byte = composition_data[composition_offset++];
                current_element->sig_models[sig_idx].model_id.sig_model_id = (high_byte << 8) | low_byte;
                ESP_LOGI(TAG, "SIG model %d id is 0x%04X", sig_idx + 1, current_element->sig_models[sig_idx].model_id.sig_model_id);
            }

            if (current_element->vnd_model_count > 0)
            {
                current_element->vnd_models = (s_provisioner_model_info_t *)malloc(current_element->vnd_model_count * sizeof(s_provisioner_model_info_t));
                assert(current_element->vnd_models);
                for (int vnd_idx = 0; vnd_idx < current_element->vnd_model_count; vnd_idx++)
                {
                    low_byte = composition_data[composition_offset++];
                    high_byte = composition_data[composition_offset++];
                    current_element->vnd_models[vnd_idx].model_id.vnd_model_id.model_id = (high_byte << 8) | low_byte;
                    current_element->vnd_models[vnd_idx].model_id.vnd_model_id.company_id = node->cid;
                    ESP_LOGI(TAG, "Vendor model %d id is 0x%04X", vnd_idx + 1, current_element->vnd_models[vnd_idx].model_id.vnd_model_id.model_id);
                }
            }
        }
    }

    return 0;
}
