#ifndef __READ_H
#define __READ_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "string.h"
#include "stdio.h"

#define RX_BUF_SIZE 64
#define ANGLE_RESPONSE_LEN 10	//
#define MAX_SERVO_NUM 6  		// ID：000~005
void test_servo_id0(void);
void servo_init(void);
void request_angle(void);
void request_angle_id(uint8_t id);
void parse_angle(void);
uint8_t get_servo_angle(uint8_t id, uint16_t *angle);

extern uint8_t uart_rx_buf[RX_BUF_SIZE];
extern uint8_t uart_rx_flag;
extern uint16_t rx_index;  // 当前写入位置
extern uint16_t servo_angle;

#endif
