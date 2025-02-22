
#include "esp_err.h"
#include "led_strip.h"
#include "led_strip_types.h"
#include "led_strip_rmt.h"
#include "esp_log.h"

#include "BLE_mesh_prov_indicator.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define INDICATOR_LED 48
#endif // CONFIG_IDF_TARGET_ESP32S3

#if defined(CONFIG_IDF_TARGET_ESP32S3)
static led_strip_handle_t indicator_led_strip_handle = NULL;

static led_strip_config_t indicator_led_strip_config = {
    .strip_gpio_num = INDICATOR_LED,
    .max_leds = 1,
    .led_pixel_format = LED_PIXEL_FORMAT_GRB,
    .led_model = LED_MODEL_WS2812,
    .flags.invert_out = false,
};

static led_strip_rmt_config_t indicator_led_rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 10 * 1000 * 1000, // 10MHz
    .flags.with_dma = false,
};
#endif // CONFIG_IDF_TARGET_ESP32S3

int BLE_mesh_indicator_init(void)
{
    int ret = -1;

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    if (0 == led_strip_new_rmt_device(&indicator_led_strip_config, &indicator_led_rmt_config, &indicator_led_strip_handle))
    {
        printf("Indicator initialized...\n");
        ESP_ERROR_CHECK(led_strip_clear(indicator_led_strip_handle));
        ESP_ERROR_CHECK(led_strip_set_pixel(indicator_led_strip_handle, 0, 255, 0, 0));
        ESP_ERROR_CHECK(led_strip_refresh(indicator_led_strip_handle));
        ret = 0;
    }
#else
    ESP_LOGE(__FILE__, "No LED indicator available for %s", CONFIG_IDF_TARGET);
#endif // CONFIG_IDF_TARGET_ESP32S3
    return ret;
}

void BLE_mesh_prov_link_open_indicate(void)
{
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    if (indicator_led_strip_handle)
    {
        ESP_ERROR_CHECK(led_strip_clear(indicator_led_strip_handle));
        ESP_ERROR_CHECK(led_strip_set_pixel(indicator_led_strip_handle, 0, 0, 0, 255));
        ESP_ERROR_CHECK(led_strip_refresh(indicator_led_strip_handle));
    }
#else
    ESP_LOGE(__FILE__, "No LED indicator available for %s", CONFIG_IDF_TARGET);
#endif // CONFIG_IDF_TARGET_ESP32S3
}

void BLE_mesh_prov_complete_indicate(void)
{
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    if (indicator_led_strip_handle)
    {
        ESP_ERROR_CHECK(led_strip_clear(indicator_led_strip_handle));
        ESP_ERROR_CHECK(led_strip_set_pixel(indicator_led_strip_handle, 0, 0, 255, 0));
        ESP_ERROR_CHECK(led_strip_refresh(indicator_led_strip_handle));
    }
#else
    ESP_LOGE(__FILE__, "No LED indicator available for %s", CONFIG_IDF_TARGET);
#endif // CONFIG_IDF_TARGET_ESP32S3
}
