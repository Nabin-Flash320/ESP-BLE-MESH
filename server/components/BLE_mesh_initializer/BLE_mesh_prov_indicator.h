

#ifndef _BLE_MESH_PROV_INDICATOR_H_
#define _BLE_MESH_PROV_INDICATOR_H_

int BLE_mesh_indicator_init(void);
void BLE_mesh_prov_link_open_indicate(void);
void BLE_mesh_prov_complete_indicate(void);
void BLE_mesh_blink_indicator(void);

#endif // _BLE_MESH_PROV_INDICATOR_H_
