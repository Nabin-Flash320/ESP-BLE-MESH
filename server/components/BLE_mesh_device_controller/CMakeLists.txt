idf_component_register(
    SRCS "BLE_mesh_device_controller.c"
    INCLUDE_DIRS .
    REQUIRES esp_driver_gpio
)