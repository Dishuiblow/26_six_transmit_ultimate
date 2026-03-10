#include "Key.h"

/*—-------------------------按键按下读取检测------------------------—*/

uint8_t Key_GetNum(void)//调用此函数可返回按下按键的键码
{
	uint8_t KeyNum = 0;//按键键码默认为0，若未按下则返回0
	if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1)//按键按下，1代表读取到高电平
	{
		HAL_Delay(20);//延时20毫秒，消除按键按下时的抖动
		while(HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == 1);//如果按键一直按下就进入死循环，直到松手
		HAL_Delay(20);//消除松手时的抖动
		KeyNum = 1;//表示按键KEY0按下	
	}
	return KeyNum;
}
