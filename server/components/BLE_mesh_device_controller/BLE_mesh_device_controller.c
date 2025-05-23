
#include "esp_log.h"
#include "driver/gpio.h"

#include "BLE_mesh_device_controller.h"

#define TAG "Device-Controller"

#define GPIO_LED_RED GPIO_NUM_18
#define GPIO_LED_GREEN GPIO_NUM_4
#define GPIO_LED_BLUE GPIO_NUM_2

int BLE_mesh_device_controller_init()
{
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << GPIO_LED_GREEN) | (1ULL << GPIO_LED_GREEN) | (1ULL << GPIO_LED_BLUE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_LED_RED, 1));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_LED_GREEN, 1));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_LED_BLUE, 1));

    return 0;
}

int BLE_mesh_device_controllet_change_onoff_state(uint8_t state)
{
    if (state != 0 && state != 1)
    {
        ESP_LOGE(TAG, "Invalid device on off state(%d)", state);
        return -1;
    }

    ESP_ERROR_CHECK(gpio_set_level(GPIO_LED_RED, (uint32_t)state));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_LED_GREEN, (uint32_t)state));
    ESP_ERROR_CHECK(gpio_set_level(GPIO_LED_BLUE, (uint32_t)state));

    return 0;
}
