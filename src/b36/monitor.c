/*
 * monitor.c
 *
 *  Created on: Nov 27, 2018
 *      Author: Jeffrey Jia
 */

#include <curses.h>
#include <signal.h>

#include "../head.h"

void monitor()
{

	extern int stdo;
	stdo = 0; //日志不输出到屏幕

	/* initialize curses */
	initscr();
	cbreak();
	noecho();
	clear();

	HB hb;
	getv(BOARD_UUID, hb.uuid);    	//UUID
	getv(STACK_MODEL, hb.model);    	//电堆型号
	getv(BOARD_MODEL, hb.borad);    	//控制板型号
	getv(SOFT_VERSION, hb.soft);    	//软件版本
	getv(NN_VERSION, hb.nnv);    	//神经网络版本
	printw("UUID:%s STACK:%s BOARD:%s SOFT:%s NN:%s\n", hb.uuid, hb.model,
			hb.borad, hb.soft, hb.nnv);

	CLCT clct;
	for (;;)
	{
		int fanPwm = geti(GET_FAN_PWM);    	//风扇PWM

		mvaddstr(1, 0, "FAN-PWM: [");
		mvhline(1, 10, '=', fanPwm);
		mvhline(1, 12 + fanPwm, ' ', 100 - fanPwm);
		char fper[50];
		sprintf(fper, "] %d%%", fanPwm);
		mvaddstr(1, 110, fper);
		move(2, 0);


		getv(GET_STACK_TEMP, clct.stack_t); //电堆温度
		getv(GET_IN_TEMP, clct.in_t); //进气温度
		getv(GET_IN_RH, clct.in_h); //进气湿度
		getv(GET_OUT_TEMP, clct.out_t); //出气温度
		getv(GET_OUT_RH, clct.out_h); //出气湿度
		getv(GET_VOLTAGE, clct.v); //电压
		getv(GET_CURRENT, clct.c); //电流

		printw("IN_TEMP  : %s°C\n", clct.in_t);
		printw("IN_RH    : %s %%\n", clct.in_h);
		printw("OUT_TEMP : %s°C\n", clct.out_t);
		printw("OUT_RH   : %s %%\n", clct.out_h);
		printw("VOLTAGE  : %s V\n", clct.v);
		printw("CURRENT  : %s A\n", clct.c);

		refresh();
		sleep(1);
	}

	endwin();
	exit(0);

}
