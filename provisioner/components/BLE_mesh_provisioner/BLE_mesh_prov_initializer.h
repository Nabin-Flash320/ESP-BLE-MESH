
#ifndef __BLE_MESH_PROV_INITIALIZER_H__
#define __BLE_MESH_PROV_INITIALIZER_H__

#include "esp_ble_mesh_config_model_api.h"
#include "BLE_mesh_prov_definitions.h"

int bluetooth_init(void);
int ble_mesh_prov_init();
int ble_mesh_provisioning_begin();

int ble_mesh_prov_set_node_info(uint16_t node_idx, uint8_t uuid[16], uint16_t unicast_address, uint8_t elemet_num);
uint16_t ble_mesh_get_own_address();
int ble_mesh_prov_add_unprov_device_info(esp_ble_mesh_unprov_dev_add_t *unprov_dev_info);
void ble_mesh_prov_list_unprov_devices();
void ble_mesh_provision_start_dev_prov_with_idx(uint16_t idx);
void ble_mesh_prov_list_prov_devices();
void ble_mesh_provision_get_node_composition_data(uint16_t idx);
s_ble_mesh_provisioned_node_info_t *ble_mesh_prov_get_node_info_unicast_addr(uint16_t unicast_addr);
s_ble_mesh_provisioned_node_info_t *ble_mesh_prov_get_node_info_idx(uint16_t idx);
int ble_mesh_prov_request_client_state(
    s_ble_mesh_provisioned_node_info_t *node_info,
    esp_ble_mesh_cfg_client_get_state_t *get_state,
    uint32_t opcode);

#endif // __BLE_MESH_PROV_INITIALIZER_H__
