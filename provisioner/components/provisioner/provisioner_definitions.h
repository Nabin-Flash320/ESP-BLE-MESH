
#ifndef __PROVISIONER_DEFINITIONS_H__
#define __PROVISIONER_DEFINITIONS_H__

#include <stdint.h>
#include <string.h>

#include "esp_ble_mesh_defs.h"

#define PROV_TOTAL_DEV_CNT 10
#define PROV_MAX_UNPROV_DEV_CNT PROV_TOTAL_DEV_CNT
#define PROV_MAX_PROV_DEV_CNT PROV_TOTAL_DEV_CNT

typedef enum e_provisioner_boolean
{
    PROV_BOOLEAN_FALE = 0,
    PROV_BOOLEAN_TRUE,
}e_provisioner_boolean_t;

typedef struct s_provisioner_model_info
{
    union {
        uint16_t sig_model_id;
        struct {
            uint16_t company_id;
            uint16_t model_id;
        }vnd_model_id;
    }model_id;
}s_provisioner_model_info_t;

typedef struct s_provisioner_element_info
{
    uint16_t element_id;
    size_t sig_model_count;
    size_t vnd_model_count;
    s_provisioner_model_info_t *sig_models;
    s_provisioner_model_info_t *vnd_models;
}s_provisioner_element_info_t;

typedef struct s_provisioner_unprov_dev_info
{
    esp_ble_mesh_unprov_dev_add_t *unprov_device_info[PROV_MAX_UNPROV_DEV_CNT];
}s_provisioner_unprov_dev_info_t;

typedef struct s_provisioner_prov_node_info
{
    char name[32];
    uint8_t uuid[16];
    uint16_t cid;
    uint16_t pid;
    uint16_t vid;
    uint16_t crpl;
    uint16_t features;
    uint16_t unicast_address;
    size_t element_num;
    s_provisioner_element_info_t *elements;
}s_provisioner_prov_node_info_t;

#endif // __PROVISIONER_DEFINITIONS_H__

