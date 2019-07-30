/*
 * b36.h
 *
 *  Created on: Apr 2, 2019
 *      Author: Jeffrey
 */

#ifndef SRC_B36_B36_H_
#define SRC_B36_B36_H_

#define  FAN_1234 23
#define  FAN_56 26

#define	OPEN  1
#define	CLOSE 0

typedef struct
{
	float v; //电压
	float c; //电流
} Vc;

typedef struct
{
	float in_t; //进气温度
	float in_rh; //进气湿度
	float out_t; //出气温度
	float out_rh; //出气湿度

} Trh;

void b36_init();
void pwm(int fan, int duty); //设置pwm占空比 0--100
void detect_vc(Vc *vc); //检测电压电流
void detect_trh(Trh *trh); //检测温度湿度
void valve(int opt); //电磁阀

#endif /* SRC_B36_B36_H_ */
