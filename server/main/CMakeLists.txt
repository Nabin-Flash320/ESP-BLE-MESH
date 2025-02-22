set(srcs "main.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  "."
                    REQUIRES nvs_flash BLE_mesh_initializer BLE_mesh_light_lightness_server              
)
