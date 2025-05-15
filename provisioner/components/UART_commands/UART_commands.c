
#include <string.h>

#include "driver/uart.h"
#include "esp_log.h"

#include "BLE_mesh_prov_initializer.h"
#include "UART_commands.h"

#define TAG "CMD-Handler"

#define UART_BUF_SIZE 512
#define UART_NUM UART_NUM_0

static void uart_cmd_handler_init(void);
static void uart_rx_task(void *pvParameters);
static void UART_cmd_handler_process_uart_data(uint8_t *data, size_t len);

// Function prototypes for command handlers
static int handle_system_control(const uint8_t *data, size_t *offset);
static void handle_bluetooth_control(const uint8_t *data, size_t *offset);
static void handle_provisioner_control(const uint8_t *data, size_t *offset);

// Call this in your app_main() function
void UART_cmd_handler_start(void)
{
    uart_cmd_handler_init();
    xTaskCreate(uart_rx_task, "uart_rx_task", 4096, NULL, 10, NULL);
}

static void uart_cmd_handler_init(void)
{
    // Initialize UART if needed
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(TAG, "Command handler initialized");
}

// UART Receiving Task
static void uart_rx_task(void *pvParameters)
{
    size_t temp_data_len = 0;
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM, &temp_data_len));
        if (0 == temp_data_len)
        {
            continue;
        }

        // Read data from UART
        size_t buffer_size = temp_data_len;
        uint8_t *received_data = (uint8_t *)malloc(buffer_size);
        if (!received_data)
        {
            continue;
        }

        memset(received_data, 0, buffer_size);
        int actual_bytes_read = uart_read_bytes(UART_NUM, received_data, buffer_size, 1000 / portTICK_PERIOD_MS);
        if (0 == actual_bytes_read)
        {
            continue;
        }

        printf("Received bytes: %d\n", actual_bytes_read);
        ESP_LOG_BUFFER_HEX(TAG, received_data, actual_bytes_read);

        UART_cmd_handler_process_uart_data(received_data, actual_bytes_read);

        free(received_data);
    }
}

static void UART_cmd_handler_process_uart_data(uint8_t *data, size_t len)
{
    if (!data || len <= 0)
    {
        ESP_LOGE(TAG, "Invalid parameters");
        return;
    }

    // Process each byte
    for (int i = 0; i < len; i++)
    {
        ESP_LOGI(TAG, "Byte[%d]: 0x%X", i, data[i]);
    }

    size_t command_idx = 0;
    uint8_t start_byte = data[command_idx++];
    // uint8_t checksum = data[len - 1];
    if (UART_CMD_START_BYTE != start_byte)
    {
        ESP_LOGI(TAG, "Invalid packet");
        return;
    }

    // size_t checksum_calculated = 0;
    // for (int i = 0; i < len - 1; i++)
    // {
    //     checksum_calculated ^= data[i];
    // }

    // if (checksum_calculated != checksum)
    // {
    //     ESP_LOGE(TAG, "Invalid checksum(got: 0x%x and calculated: 0x%x)", checksum, checksum_calculated);
    //     return;
    // }

    uint8_t command = data[command_idx++];
    switch (command)
    {
    case UART_CMD_SYSTEM_OPERATION_COMMANDS:
    {
        ESP_LOGI(TAG, "System operation command");
        handle_system_control(data, &command_idx);
        break;
    }
    case UART_CMD_BLUETOOTH_OPERATION_COMMANDS:
    {
        ESP_LOGI(TAG, "Bluetooth operation command");
        handle_bluetooth_control(data, &command_idx);
        break;
    }
    case UART_CMD_PROVISIONER_OPERATION_COMMANDS:
    {
        ESP_LOGI(TAG, "BLE provisionser operation command");
        handle_provisioner_control(data, &command_idx);
        break;
    }
    default:
    {
        ESP_LOGE(TAG, "Unable to process the command 0x%X", command);
        break;
    }
    }
}

static int handle_system_control(const uint8_t *data, size_t *offset)
{
    if (!data)
    {
        return -1;
    }

    uint8_t required_response = data[(*offset)++];
    uint8_t operation = data[(*offset)++];
    uint8_t payload_len = data[(*offset)++];

    uint8_t *payload = NULL;
    if (payload_len > 0)
    {
        payload = (uint8_t *)malloc(payload_len * sizeof(uint8_t));
        if (!payload)
        {
            return -1;
        }

        memset(payload, 0, payload_len * sizeof(uint8_t));
        memcpy(payload, data + *offset, payload_len);
    }

    ESP_LOGI(TAG, "Rsp req: 0x%X, Operation: 0x%X, payload len: 0x%X", required_response, operation, payload_len);
    switch (operation)
    {
    case UART_CMD_SYSTEM_OPRERATION_PING:
    {
        break;
    }
    case UART_CMD_SYSTEM_OPRERATION_RESTART:
    {
        esp_restart();
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Unable to process command: 0x%X", operation);
        break;
    }
    }

    return 0;
}

static void handle_bluetooth_control(const uint8_t *data, size_t *offset)
{
    if (!data)
    {
        return;
    }

    uint8_t required_response = data[(*offset)++];
    uint8_t operation = data[(*offset)++];
    uint8_t payload_len = data[(*offset)++];

    uint8_t *payload = NULL;
    if (payload_len > 0)
    {
        payload = (uint8_t *)malloc(payload_len * sizeof(uint8_t));
        if (!payload)
        {
            return;
        }

        memset(payload, 0, payload_len * sizeof(uint8_t));
        memcpy(payload, data + *offset, payload_len);
    }

    ESP_LOGI(TAG, "Rsp req: 0x%X, Operation: 0x%X, payload len: 0x%X", required_response, operation, payload_len);
    switch (operation)
    {
    case UART_CMD_BLUETOOTH_OPRERATION_START_BLE:
    {
        esp_err_t err = bluetooth_init();
        if (err)
        {
            ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        }
        break;
    }
    case UART_CMD_BLUETOOTH_OPRERATION_START_PROVISIONER:
    {
        ble_mesh_prov_init();
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Unable to process bluetooth operation: 0x%X", operation);
        break;
    }
    }
}

static void handle_provisioner_control(const uint8_t *data, size_t *offset)
{
    if (!data)
    {
        return;
    }

    uint8_t required_response = data[(*offset)++];
    uint8_t operation = data[(*offset)++];
    uint8_t payload_len = data[(*offset)++];

    uint8_t *payload = NULL;
    if (payload_len > 0)
    {
        payload = (uint8_t *)malloc(payload_len * sizeof(uint8_t));
        if (!payload)
        {
            return;
        }

        memset(payload, 0, payload_len * sizeof(uint8_t));
        memcpy(payload, data + *offset, payload_len);
    }

    ESP_LOGI(TAG, "Rsp req: 0x%X, Operation: 0x%X, payload len: 0x%X", required_response, operation, payload_len);
    if (UART_CMD_PROVISIONER_OPERATION_BEGIN_PROV == operation)
    {
        ble_mesh_provisioning_begin();
    }
    else if (UART_CMD_PROVISIONER_OPERATION_LIST_UNPROV_NODES == operation)
    {
        ble_mesh_prov_list_unprov_devices();
    }
    else if (UART_CMD_PROVISIONER_OPERATION_LIST_PROV_NODES == operation)
    {
        ble_mesh_prov_list_prov_devices();
    }
    else if (UART_CMD_PROVISIONER_OPERATION_ADD_NODE_IDX == operation)
    {
        ESP_LOGI(TAG, "Provisioning device at idx %d", (uint16_t)payload[0]);
        ble_mesh_provision_start_dev_prov_with_idx((uint16_t)payload[0]);
    }
    else if(UART_CMD_PROVISIONER_OPERATION_GET_DEVICE_COMPOSITION == operation)
    {
        ble_mesh_provision_get_node_composition_data((uint16_t)payload[0]);
    }
    else
    {
        ESP_LOGI(TAG, "Unable to process provisioner operation: 0x%X", operation);
    }

    free(payload);
}
