
#ifndef __BLE_MESH_PROV_DEFINITIONS_H__
#define __BLE_MESH_PROV_DEFINITIONS_H__

#include "stdint.h"

#include "esp_ble_mesh_defs.h"

#define BLE_MESH_APP_KEY_COUNT 2
#define BLE_MESH_APP_KEY_SIZE 16
#define BLE_MESH_TOTAL_NODE_COUNT 10

typedef enum e_ble_mesh_enum
{
    BLE_MESH_BOOL_FALSE = 0,
    BLE_MESH_BOOL_TRUE,
} e_ble_mesh_enum_t;

typedef struct s_ble_mesh_node_element_model_info
{
    union
    {
        uint16_t sig_model_id;
        struct
        {
            uint16_t company_id;
            uint16_t model_id;
        } vendor_model_id;
    };
} s_ble_mesh_node_element_model_info_t;

typedef struct s_ble_mesh_node_element_info
{
    uint16_t element_id;
    size_t sig_model_count;
    size_t vendor_model_count;
    s_ble_mesh_node_element_model_info_t *sig_model_info;
    s_ble_mesh_node_element_model_info_t *vendor_model_info;
} s_ble_mesh_node_element_info_t;

typedef struct s_ble_mesh_provisioned_node_info
{
    uint8_t uuid[16];
    uint16_t company_id;
    uint16_t product_id;
    uint16_t version_id;
    uint16_t crpl;
    uint16_t features;
    uint16_t unicast_address;
    size_t element_num;
    s_ble_mesh_node_element_info_t *elements;
    e_ble_mesh_enum_t is_provisioned;
} s_ble_mesh_provisioned_node_info_t;

typedef struct s_ble_mesh_app_key
{
    uint8_t app_key_idx;
    uint8_t app_key[BLE_MESH_APP_KEY_SIZE];
} s_ble_mesh_app_key_t;


typedef struct s_ble_mesh_unprov_dev_info
{
    esp_ble_mesh_unprov_dev_add_t *unprov_device_info[BLE_MESH_TOTAL_NODE_COUNT];
} s_ble_mesh_unprov_dev_info_t;

typedef struct s_ble_mesh_provisioner_node_struct
{
    uint16_t net_idx;
    uint16_t app_idx;
    s_ble_mesh_app_key_t app_keys[BLE_MESH_APP_KEY_COUNT];
    s_ble_mesh_provisioned_node_info_t nodes[BLE_MESH_TOTAL_NODE_COUNT];
    s_ble_mesh_unprov_dev_info_t unprov_devices;
} s_ble_mesh_provisioner_node_struct_t;

#endif // __BLE_MESH_PROV_DEFINITIONS_H__
