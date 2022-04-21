#pragma once

void initI2C(uint8_t slave_id);
void recv_handler(int len);
void req_handler();
void i2c_send(uint8_t id, uint8_t *msg, int len);
void node_discovery(void);
void msg_handler_parser(uint8_t *msg, int len);
int msg_formatter(uint8_t *msg, uint8_t cmd, uint8_t *data, int data_size);