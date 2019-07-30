/*
 ============================================================================
 Name        : b36.c
 Author      : jiajiefeng
 Version     : 1.0
 Copyright   : OpenFC
 Description : Fuel Cell Control System(FCCS)
 ============================================================================
 */
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include "ads1115.h"
#include "b36.h"

#define  TCA9548_ADDR 0x70
#define  ADS1115_ADDR 0x48
#define  SHT30_ADDR 0x44
#define PIN_BASE 200

int tca9548_fd = 0;
int sht30_fd = 0;

//LED pin脚
int LED1 = 2; //D8 十位
int LED0 = 0; //D5 个位

int n1 = 0, n2 = 0; //状态码

void led();

void b36_init()
{
	wiringPiSetup();

	//pwm
	pwmSetClock(3200); //6khz
//	pwmSetRange(100);//0--100 x%
//	pwmSetMode(PWM_MODE_MS);
	pinMode(24, PWM_OUTPUT); //BCM 19 pin_35 FAN12 PWM1
	pinMode(23, PWM_OUTPUT); //BCM 13 pin_33 FAN34 PWM1 alt0
	pinMode(26, PWM_OUTPUT); //BCM 12 pin_32 FAN56 PWM0 alt0

//	pinMode(1, PWM_OUTPUT); //BCM 18 pin_12  FAN78 PwM0-- 不用作风扇输出
	pinMode(1, OUTPUT); //电磁阀

	//led灯
	pinMode(LED0, OUTPUT);
	pinMode(LED1, OUTPUT);

	tca9548_fd = wiringPiI2CSetup(TCA9548_ADDR);
	sht30_fd = wiringPiI2CSetup(SHT30_ADDR);
	ads1115Setup(PIN_BASE, ADS1115_ADDR);

	wiringPiI2CWrite(tca9548_fd, 0x7E); //切换通道 01111110=0x7E
	char config[2] =
	{ 0x23, 0x34 };
	write(sht30_fd, config, 2); //开启sht30温度检测

	pthread_t led_t;
	pthread_create(&led_t, NULL, (void *) led, NULL);
}

/**
 * 获取电压电流
 */
void detect_vc(Vc *vc)
{
	wiringPiI2CWrite(tca9548_fd, 0x80); //切换通道 10000000=0x80

	int a2 = analogRead(PIN_BASE + 2);
	float v = 187.5 * a2 * 21 / 1000 / 1000;
//	printf("[Voltage]: AIN2-GND = %d, %.2f V \n", a2, v);

	int a0 = analogRead(PIN_BASE);
	int a1 = analogRead(PIN_BASE + 1);
	float c = 187.5 * (a0 - a1 / 2) / 40 / 1000;
//	printf("[Current]: AIN0 = %d,AIN1= %d,I= %.2f A \n", a0, a1, c);
	vc->v = v;
	vc->c = c;
}

/**
 * 温度和湿度
 */
void detect_trh(Trh *trh)
{

	wiringPiI2CWrite(tca9548_fd, 0x02);
	char cmd[2] =
	{ 0xe0, 0x00 };
	write(sht30_fd, cmd, 2);
	char data[6] =
	{ 0 };
	if (read(sht30_fd, data, 6) != 6)
	{
		printf("[===错误===] : Input/output Erorr \n");
	}
	else
	{
		// Convert the data
		int temp = (data[0] * 256 + data[1]);
		float cTemp = -45 + (175 * temp / 65535.0);
		float humidity = 100 * (data[3] * 256 + data[4]) / 65535.0;

		// Output data to screen
//		printf("湿度 Relative Humidity : %.2f RH \n", humidity);
//		printf("温度 Temperature in Celsius : %.2f C \n", cTemp);

		trh->in_t = cTemp;
		trh->in_rh = humidity;
		trh->out_t = cTemp; //TODO
		trh->out_rh = humidity; //TODO
	}
}

/**
 * 设置风扇转速
 * duty 占空比 0--100
 */
void pwm(int fan, int duty)
{
	duty = 1024 - duty * 10;
	pwmWrite(fan, duty);
}

/**
 * 设置电磁阀
 */
void valve(int opt)
{
	digitalWrite(1, opt); // On
}

/**
 * LED报警灯
 */
void led()
{
	while (1)
	{
		if (n1 + n2 > 0)
		{
			for (int i = 0; i < 9; i++)
			{
				if (i < n1)
				{
					digitalWrite(LED0, HIGH); // On
				}
				if (i < n2)
				{
					digitalWrite(LED1, HIGH); // On
				}
				delay(200);	// ms
				if (i < n1)
				{
					digitalWrite(LED0, LOW);	// Off
				}
				if (i < n2)
				{
					digitalWrite(LED1, LOW);	// Off
				}

				delay(200);

			}
		}
		else
		{
			digitalWrite(LED0, HIGH); // On
			delay(100);
			digitalWrite(LED0, LOW);	// Off
			delay(100);
			digitalWrite(LED1, HIGH); // On
			delay(100);
			digitalWrite(LED1, LOW);	// Off

		}
		delay(2000);
	}

}

