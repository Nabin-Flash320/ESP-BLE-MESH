#ifndef __BLE_MESH_DEIVCE_CONTROLLER_H__
#define __BLE_MESH_DEIVCE_CONTROLLER_H__

int BLE_mesh_device_controller_init();
int BLE_mesh_device_controllet_change_onoff_state(uint8_t state);
uint8_t BLE_mesh_device_get_onoff_state();

#endif // __BLE_MESH_DEIVCE_CONTROLLER_H__