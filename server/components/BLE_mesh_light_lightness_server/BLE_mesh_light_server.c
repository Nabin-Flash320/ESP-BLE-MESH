
#include "math.h"

#include "esp_log.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_lighting_model_api.h"
#include "esp_ble_mesh_defs.h" // contains all the opcodes for SIG models

#include "BLE_mesh_definitions.h"
#include "BLE_mesh_initializer.h"
#include "BLE_mesh_device_controller.h"

#include "BLE_mesh_light_server.h"

void BLE_mesh_light_lightness_server_initialize()
{

    // Initializes device for working with on/off server
    BLE_mesh_device_controller_init();

    // Register callback for light lightnesss' generic onoff sensor
    void *light_lightness_generic_onoff_server_user_data = BLE_mesh_light_server_get_generic_on_off_server_data();
    ble_mesh_register_sig_generic_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV,
        BLE_mesh_light_server_generic_on_off_model_callback,
        light_lightness_generic_onoff_server_user_data);

    // Register callback for light lightnesss' generic level sensor
    void *light_lightness_generic_level_server_user_data = BLE_mesh_light_server_get_generic_level_server_data();
    ble_mesh_register_sig_generic_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV,
        BLE_mesh_light_lightness_generic_level_model_callback,
        light_lightness_generic_level_server_user_data);

    // Register callback for light lightneess server.
    void *light_lightness_server_setup_user_data = BLE_mesh_light_server_get_lightness_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,
        BLE_mesh_light_lightness_model_setup_server_callback,
        light_lightness_server_setup_user_data);

    // Register callback for light lightness setup server.
    void *light_lightness_server_user_data = BLE_mesh_light_server_get_lightness_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV,
        BLE_mesh_light_server_lightness_server_callback,
        light_lightness_server_user_data);

    // Register callback for hsl setup server
    void *light_hsl_setup_server_user_data = BLE_mesh_light_server_get_hsl_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        2,
        ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV,
        BLE_mesh_light_hsl_model_setup_server_callback,
        light_hsl_setup_server_user_data);

    // Register callback for hsl server
    void *light_hsl_server_user_data = BLE_mesh_light_server_get_hsl_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        2,
        ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV,
        BLE_mesh_light_hsl_server_callback,
        light_hsl_server_user_data);
    
    // Register callback for hsl hue server
    void *light_hsl_hue_server_user_data = BLE_mesh_light_server_get_hsl_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        2,
        ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV,
        BLE_mesh_light_hsl_hue_server_callback,
        light_hsl_hue_server_user_data);
    
    // Register callback for hsl saturation server
    void *light_hsl_saturation_server_user_data = BLE_mesh_light_server_get_hsl_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        2,
        ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV,
        BLE_mesh_light_hsl_saturation_server_callback,
        light_hsl_saturation_server_user_data);

    // Register callback for light ctl setup server
    void *light_ctl_setup_server_user_data = BLE_mesh_light_server_get_light_ctl_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV,
        BLE_mesh_light_ctl_model_setup_server_callback,
        light_ctl_setup_server_user_data);

    // Register callback for light ctl server
    void *light_ctl_server_user_data = BLE_mesh_light_server_get_light_ctl_server_data(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV);
    ble_mesh_register_sig_lighting_model_user_args(
        1,
        ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV,
        BLE_mesh_light_ctl_lightness_server_callback,
        light_ctl_server_user_data);
}
