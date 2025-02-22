#ifndef _BLE_MESH_MODEL_PUBLICATIONS_H_
#define _BLE_MESH_MODEL_PUBLICATIONS_H_

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"

#include "BLE_mesh_definitions.h"

#if defined(CONFIG_BLE_MESH_LED_VENDOR_MODEL_ENABLE)
// msg_len calculated by summing up opcode len and message size.
// For vendor model, opcode len is 3 bytes and for our vendor model opcode msg len size is 1 byte in len so it can be set as 3+1 or directly 4
ESP_BLE_MESH_MODEL_PUB_DEFINE(led_state_publication, 3 + 2, ROLE_NODE);
#endif // if defined(CONFIG_BLE_MESH_LED_VENDOR_MODEL_ENABLE)

#if defined(CONFIG_BLE_MESH_BME280_VENDOR_MODEL_ENABLE)
ESP_BLE_MESH_MODEL_PUB_DEFINE(bme280_publication, 3 + 13 + 1, ROLE_NODE);
#endif //  if defined(CONFIG_BLE_MESH_BME280_VENDOR_MODEL_ENABLE)

#endif // _BLE_MESH_MODEL_PUBLICATIONS_H_
