#include "read.h"
#include "main.h"
#include "string.h"
#include "stdio.h"

extern UART_HandleTypeDef huart6; // 舵机连接的串口

uint8_t uart_rx_buf[RX_BUF_SIZE];
uint8_t uart_rx_flag = 0;
uint16_t rx_index = 0; // 写入位置
uint16_t read_index = 0; // 解析读取位置

// ====== 这里补回了缺失的 ServoInfo 结构体定义 ======
typedef struct
{
    uint16_t angle;
    uint8_t updated; // 角度更新标志
} ServoInfo;

ServoInfo servos[MAX_SERVO_NUM];
// ====================================================

void servo_init(void)
{
    uint8_t cmd[] = "#255PULK!";
    HAL_UART_Transmit(&huart6, cmd, strlen((char *)cmd), 100); // 改为huart6
    HAL_Delay(100);
}

void request_angle_id(uint8_t id)
{
    if (id >= MAX_SERVO_NUM) return;
    char cmd[15];
    snprintf(cmd, sizeof(cmd), "#%03dPRAD!", id); 
    HAL_UART_Transmit(&huart6, (uint8_t *)cmd, strlen(cmd), 100); // 改为huart6
}

// 基于环形缓冲区的单字节状态机解析
void parse_angle(void)
{
    static uint8_t frame[ANGLE_RESPONSE_LEN];
    static uint8_t frame_pos = 0;

    // 当读取位置不等于写入位置时，说明有新数据
    while (read_index != rx_index) 
    {
        uint8_t byte = uart_rx_buf[read_index];
        read_index = (read_index + 1) % RX_BUF_SIZE; // 移动读取指针

        // 帧头检测
        if (byte == '#')
        {
            frame_pos = 0;
            frame[frame_pos++] = byte;
            continue;
        }

        // 收集有效数据
        if (frame_pos > 0 && frame_pos < ANGLE_RESPONSE_LEN)
        {
            frame[frame_pos++] = byte;
            
            // 完整帧检测
            if (byte == '!' && frame_pos == ANGLE_RESPONSE_LEN)
            {
                if (frame[4] == 'P')
                {
                    uint8_t id = (frame[1] - '0') * 100 + (frame[2] - '0') * 10 + (frame[3] - '0');
                    if (id < MAX_SERVO_NUM)
                    {
                        servos[id].angle = (frame[5] - '0') * 1000 +
                                           (frame[6] - '0') * 100 +
                                           (frame[7] - '0') * 10 +
                                           (frame[8] - '0');
                        servos[id].updated = 1;
                    }
                }
                frame_pos = 0; // 一帧解析完毕，重置状态
            }
        }
    }
}

uint8_t get_servo_angle(uint8_t id, uint16_t *angle)
{
    if (id >= MAX_SERVO_NUM || angle == NULL) 
    {
        return 0;
    }

    if (servos[id].updated)
    {
        if(servos[id].angle >= 500 && servos[id].angle <= 2500)
        {
            *angle = servos[id].angle;
        }
        servos[id].updated = 0;
        return 1;
    }
    return 0; 
}

// 修正后的中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    if (huart->Instance == USART6) 
    {
        rx_index = (rx_index + 1) % RX_BUF_SIZE; 
        uart_rx_flag = 1; // 通知主循环开始解析
        
        // 重新开启下一次单字节接收
        HAL_UART_Receive_IT(&huart6, &uart_rx_buf[rx_index], 1); 
    }
}
