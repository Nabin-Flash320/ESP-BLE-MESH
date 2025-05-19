
#ifndef __PROVISIONER_H__
#define __PROVISIONER_H__

#include "esp_ble_mesh_defs.h"

int provisioner_init();
int provisioner_enable();
int provisioner_add_dev_to_node(uint8_t idx);

int provisioner_add_unprov_dev_info(esp_ble_mesh_unprov_dev_add_t *unprov_device_info);
int provisioner_add_prov_dev_info(const uint8_t uuid[16], uint16_t unicast, uint8_t elem_num);
void provisioner_log_unprov_dev_info();
void provisioner_log_prov_dev_info();
esp_ble_mesh_unprov_dev_add_t *get_unprov_dev(uint8_t idx);
void remove_unprov_dev(uint8_t idx);
int parse_composition_data(struct net_buf_simple *composition_buffer, uint16_t unicast);

#endif // __PROVISIONER_H__
