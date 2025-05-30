/* main.c - Application main entry point */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "sdkconfig.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "BLE_mesh_light_server.h"

#include "ble_mesh_example_init.h"
#include "BLE_mesh_initializer.h"

#define TAG "main"

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    
    // BLE_mesh_lighting_model_intialize();
    
    err = bluetooth_init();
    if (err)
    {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }
    
    /* Initialize the Bluetooth Mesh Subsystem */
    bt_mesh_timer_init();
    BLE_mesh_light_lightness_server_initialize();
    err = ble_mesh_init();
    if (err)
    {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }
}
