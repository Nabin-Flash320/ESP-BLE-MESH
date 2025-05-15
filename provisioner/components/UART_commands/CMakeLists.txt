

idf_component_register(
    SRCS "UART_commands.c"
    INCLUDE_DIRS .
    REQUIRES driver BLE_mesh_provisioner
)


