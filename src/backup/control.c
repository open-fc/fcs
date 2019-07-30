#include "head.h"

/**
 * 采集所有数据
 */
void collect()
{
	while (1)
	{

		//采集温度,湿度 电压 电流 空压机转速,水泵转速,空气流量 TODO
		float stack_t = rand()%100*0.01+ rand() % 11 + 69; //69-79;//56.43; //电堆温度
		float in_t =  rand()%100*0.01+ rand() % 6 + 23;//23-28 //进气温度
		float in_h = rand()%100*0.01+ rand() % 16 + 30; //30-45 56.43; //进气湿度
		float out_t = rand()%100*0.01+ rand() % 11 + 59; //59-69 //出气温度
		float out_h = rand()%100*0.01+ rand() % 8 + 65; //65-82 //出气湿度
		float v = rand()%100*0.01+ rand() % 10 + 36; //36-45 //电压
		float c = rand()%100*0.01+ rand() % 18 + 52; //52-69 //电流
		int fan_rpm = 7000+rand()%500; //7000-7500; //空压机转速
//		int pump_rpm = 400+rand()%30; //400-430  //水泵转速

		CLCT clct;
		getv(BOARD_UUID, clct.uuid); //设备唯一ID
		clct.time = time(NULL); //时间戳
		clct.tid = incr(INCR_TID); //事务ID
		clct.ctid = incr(INCR_CTID); //子事务ID
		getv(MODE, clct.mode); //运行模式
		getv(POP, clct.pop); //输出偏好
		sprintf(clct.stack_t, "%.2f", stack_t); //电堆温度
		sprintf(clct.in_t, "%.2f", in_t); //进气温度
		sprintf(clct.in_h, "%.2f", in_h); //进气湿度
		sprintf(clct.out_t, "%.2f", out_t); //出气温度
		sprintf(clct.out_h, "%.2f", out_h); //出气湿度
		sprintf(clct.v, "%.2f", v); //电压
		sprintf(clct.c, "%.2f", c); //电流
		sprintf(clct.fan_rpm, "%d", fan_rpm); //空压机转速
//		sprintf(clct.pump_rpm, "%d", pump_rpm); //水泵转速

		//类型,报文版本,UUID,时间戳(秒),事务ID,子事务ID,运行模式,输出偏好,电堆温度,进气温度,进气湿度,出气温度,出气湿度,电压,电流,空压机转速,水泵转速,空气流量
		char context[1024];
		sprintf(context,
				"CLCT,10,%s,%lu,%lld,%lld,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
				clct.uuid, clct.time, clct.tid, clct.ctid, clct.mode, clct.pop,
				clct.stack_t, clct.in_t, clct.in_h, clct.out_t, clct.out_h,
				clct.v, clct.c, clct.fan_rpm);
		writeOut(context);

		//向redis存入采集数据
		setv(STACK_TEMP, clct.stack_t); //电堆温度
		setv(IN_TEMP, clct.in_t); //进气温度
		setv(IN_HUM, clct.in_h); //进气湿度
		setv(OUT_TEMP, clct.out_t); //出气温度
		setv(OUT_HUM, clct.out_h); //出气湿度
		setv(VOLTAGE, clct.v); //电压
		setv(CURRENT, clct.c); //电流
		setv(FAN_RPM, clct.fan_rpm); //空压机转速
//		setv(PUMP_RPM, clct.pump_rpm); //水泵转速

		//控制策略计算 TODO

		//对外输出设置:空压机pwm,水泵pwm TODO

		int fan_pwm = rand()%21+70; //70-90;
		int pump_pwm = rand()%21+70; //70-90;

		OP op;
		getv(BOARD_UUID, op.uuid);
		op.time = time(NULL); //时间戳
		op.tid = incr(INCR_TID); //事务ID
		op.ttid = incr(INCR_TUNE_TID); //子事务ID
		sprintf(op.fan_pwm, "%d", fan_pwm); //空压机转速
//		sprintf(op.pump_pwm, "%d", pump_pwm); //水泵转速

		//类型,报文版本,UUID,时间戳(秒),事务ID,子事务ID,空压机pwm
		sprintf(context, "OP,10,%s,%lu,%lld,%lld,%s\n", op.uuid, op.time,
				op.tid, op.ttid, op.fan_pwm);
		writeOut(context);

		//向redis存入采集数据
		setv(FAN_PWM, op.fan_pwm); //空压机PWM
//		setv(PUMP_PWM, op.pump_pwm); //水泵PWM

		sleep(geti(COLLECT_DEFAULT_DELAY));
	}
}

/**
 * 排氢操作,排水操作
 */
void discharge()
{
	while (1)
	{

		//排氢操作
		int dur = geti(GET_H2_OUT_DUR); //持续时长(单位:ms)
		//TODO 打开排氢口
		sleep(dur / 1000);    	//排氢口打开时长
		//TODO 关闭排氢口
		//采集出氢口温度,湿度 TODO
		float tmep = rand()%100*0.01+ rand() % 11 + 69; //69-79 //温度
		float hum = rand()%100*0.01+ rand() % 13 + 74; //74-86  	//湿度

		//计算设置下次排氢时间和持续时长
		dur =  rand() %1000 + 2000;//2000-3000;    	//TODO
		int delay = rand() %70 + 50;//50-120  //睡眠时间//TODO

		DISC disc;
		getv(BOARD_UUID, disc.uuid);
		disc.time = time(NULL);    	//时间戳
		disc.tid = incr(INCR_TID);    	//事务ID
		disc.htid = incr(INCR_H2_TID);    	//子事务ID
		disc.h2_out_t = tmep;    	//H2出口温度
		disc.h2_out_h = hum;    	//H2出口湿度
		disc.delay = delay;    	//下次排氢时间(休眠多长时间 单位:s)
		disc.dur = dur;    	//持续时长(单位:ms)

		//类型,报文版本,UUID,时间戳(秒),事务ID,子事务ID,氢气温度,氢气湿度,下次(s),时长(ms)
		char context[1024];
		sprintf(context, "DISC,10,%s,%lu,%lld,%lld,%.2f,%.2f,%d,%d\n",
				disc.uuid, disc.time, disc.tid, disc.htid, disc.h2_out_t,
				disc.h2_out_h, disc.delay, disc.dur);
		writeOut(context);

		//向redis存入数据
		seti(H2_TIME, disc.time);
		setf(H2_OUT_TEMP, tmep);    	//H2出口温度
		setf(H2_OUT_HUM, hum);    	//H2出口湿度
		seti(H2_OUT_DELAY, delay);    	//下次排氢时间(休眠多长时间 单位:s)
		seti(SET_H2_OUT_DUR, dur);    	//持续时长(单位:ms)
		sleep(delay);

	}
}

/**
 * 心跳检测
 */
void hb()
{
	while (1)
	{

		HB hb;
		getv(BOARD_UUID, hb.uuid);    	//UUID
		hb.time = time(NULL);    	//时间戳(秒)
		getv(STACK_MODEL, hb.model);    	//电堆型号
		getv(BOARD_MODEL, hb.borad);    	//控制板型号
		getv(SOFT_VERSION, hb.soft);    	//软件版本
		getv(NN_VERSION, hb.nnv);    	//神络版本

		//类型,报文版本,UUID,时间戳(秒),电堆型号,控制板型号,软件版本,神络版本
		char context[1024];
		sprintf(context, "HB,10,%s,%lu,%s,%s,%s,%s\n", hb.uuid, hb.time,
				hb.model, hb.borad, hb.soft, hb.nnv);
		writeOut(context);

		sleep(geti(HB_DELAY));
	}

}

/**
 * 故障报警
 */
void warn()
{
// gpio  28 29
	//类型,报文版本,UUID,时间戳(秒),子事务ID,故障等级,故障码,故障信息
	char context[1024];

	writeOut(context);

}
