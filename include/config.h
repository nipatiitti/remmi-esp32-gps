#include <Arduino.h>
#include <mcp_canbus.h>

#define SPI_CS_PIN D7
extern MCP_CAN CAN;

extern uint32_t CAN_SEND_ID;
extern uint32_t CAN_RECEIVE_ID;
