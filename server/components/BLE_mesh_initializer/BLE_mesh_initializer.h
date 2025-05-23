
#ifndef _BLE_MESH_INITIALIZER_H_
#define _BLE_MESH_INITIALIZER_H_

#include "esp_err.h"

#include "BLE_mesh_definitions.h"

esp_err_t ble_mesh_init(void);
int ble_mesh_register_vendor_model_user_args(uint16_t element_id, uint16_t model_id, custom_model_callback model_callback,
                                             model_publication_callback publication_callback, void *args);
int ble_mesh_register_sig_generic_model_user_args(uint16_t element_id, uint16_t model_id, sig_generic_model_callback model_callback, model_publication_callback pub_callback, void *args);
int ble_mesh_register_sig_lighting_model_user_args(uint16_t element_id, uint16_t model_id, sig_lighting_model_callback model_callback, void *args);

#endif // _BLE_MESH_INITIALIZER_H_
