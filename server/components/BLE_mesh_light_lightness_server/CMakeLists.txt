

idf_component_register(
    SRCS "BLE_mesh_light_server_light_ctl_server.c" "BLE_mesh_light_server_hsl_server.c" "BLE_mesh_light_server_generic_level_server.c" "BLE_mesh_light_server_lightness_server.c" "BLE_mesh_light_server.c" "BLE_mesh_light_server_generic_on_off_server.c"
    INCLUDE_DIRS .
    REQUIRES bt BLE_mesh_initializer
)

