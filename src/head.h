/*
 * head.h
 *
 *  Created on: 2018年8月27日
 *      Author: jiajiefeng
 */

#ifndef HEAD_H_
#define HEAD_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <hiredis/hiredis.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <uuid/uuid.h>
#include <arpa/inet.h>
/**
 * 从redis获取配置信息
 */
//收集参数间隔 单位:秒
#define COLLECT_DEFAULT_DELAY "HGET FCCS_CONFIG COLLECT_DEFAULT_DELAY" //默认间隔
#define COLLECT_FAN_DELAY "HGET FCCS_CONFIG COLLECT_FAN_DELAY"           //检测风扇转速
#define COLLECT_TEMP_DELAY "HGET FCCS_CONFIG COLLECT_TEMP_DELAY"       //检测温度
#define COLLECT_RH_DELAY "HGET FCCS_CONFIG COLLECT_RH_DELAY"           //检测湿度
#define COLLECT_VC_DELAY "HGET FCCS_CONFIG COLLECT_VC_DELAY"            //检测电压电流

//调整间隔参数 单位:秒
#define COOL_DELAY "HGET FCCS_CONFIG COOL_DELAY"           //调整散热操作间隔
#define EXHAUST_DELAY "HGET FCCS_CONFIG EXHAUST_DELAY"       //排氢间隔
#define EXHAUST_TIME "HGET FCCS_CONFIG EXHAUST_TIME"           //单次排氢气持续时长

//心跳检测频率
#define HB_DELAY "HGET FCCS_CONFIG HEART_BEAT_DELAY"

//日志信息
#define LOG_DIR "HGET FCCS_CONFIG LOG_DIR"                               //日志存放目录
#define LOG_DIR_MAX_SIZE "HGET FCCS_CONFIG LOG_DIR_MAX_SIZE"           //日志文目录最大使用空间
#define LOG_FILE_MAX_SIZE "HGET FCCS_CONFIG LOG_FILE_MAX_SIZE"           //单个日志文件最大大小

//UDP服务器地址和端口
#define UDP_ENABLE "HGET FCCS_CONFIG UDP_ENABLE"						//UDP开关 0:关 1:开
#define UDP_ADDR "HGET FCCS_CONFIG UDP_ADDR"                            //UDP地址
#define UDP_PORT "HGET FCCS_CONFIG UDP_PORT"                            //UDP端口

/**
 * 向redis中设置运行状态
 */

//全局事务ID
#define INCR_TID "HINCRBY FCCS_RUN TID 1"

//温度湿度
#define INCR_CTID "HINCRBY FCCS_RUN CTID 1"
#define TTID_TIME "HSET FCCS_RUN TIME %s"
#define STACK_TEMP "HSET FCCS_RUN STACK_TEMP %s"
#define GET_STACK_TEMP "HGET FCCS_RUN STACK_TEMP"
#define IN_TEMP "HSET FCCS_RUN IN_TEMP %s"
#define GET_IN_TEMP "HGET FCCS_RUN IN_TEMP"
#define IN_RH  "HSET FCCS_RUN IN_RH %s"
#define GET_IN_RH  "HGET FCCS_RUN IN_RH"
#define OUT_TEMP "HSET FCCS_RUN OUT_TEMP %s"
#define GET_OUT_TEMP "HGET FCCS_RUN OUT_TEMP"
#define OUT_RH  "HSET FCCS_RUN OUT_RH %s"
#define GET_OUT_RH  "HGET FCCS_RUN OUT_RH"


//电压电流
#define INCR_ETID "HINCRBY FCCS_RUN ETID 1"
#define ETID_TIME "HSET FCCS_RUN ETID_TIME %s"
#define VOLTAGE "HSET FCCS_RUN VOLTAGE %s"
#define GET_VOLTAGE "HGET FCCS_RUN VOLTAGE"
#define CURRENT "HSET FCCS_RUN CURRENT %s"
#define GET_CURRENT "HGET FCCS_RUN CURRENT"

//空压机(水冷型)/风扇转速(空冷型)
#define FAN_RPM "HSET FCCS_RUN FAN_RPM %s"
#define FAN_PWM "HSET FCCS_RUN FAN_PWM %s"
#define GET_FAN_PWM "HGET FCCS_RUN FAN_PWM"

//水泵转速(水冷型)
#define PUMP_RPM "HSET FCCS_RUN PUMP_RPM %s"
#define PUMP_PWM "HSET FCCS_RUN PUMP_PWM %s"
#define GET_PUMP_PWM "HGET FCCS_RUN PUMP_PWM "



//排氢口
#define INCR_H2_TID "HINCRBY FCCS_RUN H2_TID 1"
#define H2_TIME "HSET FCCS_RUN H2_TIME %s"
#define H2_OUT_TEMP "HSET FCCS_RUN H2_OUT_TEMP %s"
#define H2_OUT_RH "HSET FCCS_RUN H2_OUT_RH %s"
//下次排氢
#define H2_OUT_DELAY "HSET FCCS_RUN H2_OUT_DELAY %s"
#define SET_H2_OUT_DUR "HSET FCCS_RUN H2_OUT_DUR %s"
#define GET_H2_OUT_DUR "HGET FCCS_RUN H2_OUT_DUR"

//状态/模式
#define STATUS "HGET FCCS_RUN STATUS"
#define MODE "HGET FCCS_RUN MODE"
#define POP "HGET FCCS_RUN POP"

//设备信息
#define BOARD_UUID "HGET FCCS_DEVICE BOARD_UUID"//设备UUID FC唯一标识
#define SET_BOARD_UUID "HSET FCCS_DEVICE BOARD_UUID %s"
#define HAS_BOARD_UUID "HEXISTS FCCS_DEVICE BOARD_UUID"
#define STACK_MODEL "HGET FCCS_DEVICE STACK_MODEL"//电堆型号
#define BOARD_MODEL "HGET FCCS_DEVICE BOARD_MODEL"//控制板型号
#define SOFT_VERSION "HGET FCCS_DEVICE SOFT_VERSION"//软件版本
#define NN_VERSION "HGET FCCS_DEVICE NN_VERSION"//神经网络版本(neural network)

//对外设置
#define INCR_TUNE_TID "HINCRBY FCCS_RUN TUNE_TID 1"


/**
 * 物理针脚和wringPi针脚对应
 */
#define PIN_38 28
#define PIN_40 29


/**
 * 日志文件结构体
 */
typedef struct _Record
{
	char name[50];
	char path[100];
	FILE *file;
	unsigned long size;
} Record;

typedef struct
{
	char uuid[37]; //设备唯一ID
	unsigned long time; //时间戳(秒)
	long long tid; //事务ID
	long long ttid; //子事务ID
	char fan_pwm[10]; //风扇pwm
	char pump_pwm[10]; //水泵pwm
} OP;

typedef struct
{
	char uuid[37]; //设备唯一ID
	unsigned long time; //时间戳(秒)
	long long tid; //事务ID
	long long htid; //子事务ID
	float h2_out_t; //H2出口温度
	float h2_out_h; //H2出口湿度
	int delay; //下次排氢时间
	int dur; //持续时长

} DISC;

typedef struct
{
	char uuid[37]; //设备唯一ID
	unsigned long time; //时间戳(秒)
	char model[100]; //电堆型号
	char borad[100]; //控制板型号
	char soft[100]; //软件版本
	char nnv[100]; //神经网络版本
} HB;

typedef struct
{
	char uuid[37]; //设备唯一ID
	unsigned long time; //时间戳(秒)
	int level; //等级
	int code; //故障码
	char desc[100]; //故障描述
} WARN;

typedef struct
{
	char uuid[37]; //设备唯一ID
	unsigned long time; //时间戳(秒)
	long long tid; //事务ID
	long long ctid; //子事务ID
	char mode[10]; //运行模式
	char pop[10]; //输出偏好
	char stack_t[10]; //电堆温度
	char in_t[10]; //进气温度
	char in_h[10]; //进气湿度
	char out_t[10]; //出气温度
	char out_h[10]; //出气湿度
	char v[10]; //电压
	char c[10]; //电流
	char fan_rpm[10]; //空压机转速
	char pump_rpm[10]; //水泵转速
	char air_flow[10]; //空气流量(L/m)
} CLCT;

/**
 * 函数声明
 */
char *timestr(int isName);
void logger(char* format, ...);

//redis相关函数
void setv(char *cmd, char *arg);
void getv(char *cmd, char *var);
void seti(char *cmd, long long i);
int geti(char *cmd);
void setf(char *cmd, float f);
long long incr(char *cmd);
int exist(char *cmd);

void writeOut(char *msg);

//控制操作相关函数
void init();
void init_baord();
void control();
void discharge();
void heartbeat();
void warn();

//监控
void monitor();

#endif
