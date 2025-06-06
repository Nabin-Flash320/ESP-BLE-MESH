
#ifndef __UART_COMMANDS_H__
#define __UART_COMMANDS_H__

#define UART_CMD_START_BYTE 0xAA

// Commands
#define UART_CMD_SYSTEM_OPERATION_COMMANDS 0x01
#define UART_CMD_BLUETOOTH_OPERATION_COMMANDS 0x02
#define UART_CMD_PROVISIONER_OPERATION_COMMANDS 0x03

// System command operations
#define UART_CMD_SYSTEM_OPRERATION_PING 0x00
#define UART_CMD_SYSTEM_OPRERATION_RESTART 0x01

// Bluetooth operation
#define UART_CMD_BLUETOOTH_OPRERATION_START_BLE 0x01

// Provisioner operations
#define UART_CMD_PROVISIONER_OPERATION_BEGIN_PROV 0x01
#define UART_CMD_PROVISIONER_OPERATION_LIST_UNPROV_NODES 0x02
#define UART_CMD_PROVISIONER_OPERATION_LIST_PROV_NODES 0x03
#define UART_CMD_PROVISIONER_OPERATION_ADD_NODE_IDX 0x04
#define UART_CMD_PROVISIONER_OPERATION_GET_DEVICE_COMPOSITION 0x05

// Function prototypes
void UART_cmd_handler_start(void);
void UART_cmd_handler_send_response(uint8_t cmd, uint8_t resp_code, const uint8_t *payload, uint8_t length);

#endif // __UART_COMMANDS_H__