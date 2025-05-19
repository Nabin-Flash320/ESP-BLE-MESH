idf_component_register(
    SRCS "provisioner.c" "provisioner_info.c"
    INCLUDE_DIRS .
    REQUIRES bt example_init
)