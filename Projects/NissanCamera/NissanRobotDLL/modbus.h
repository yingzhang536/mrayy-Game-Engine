

/********************************************************************
created:	2013/12/04
created:	7:26:2014   14:33
filename: 	C:\Development\mrayEngine\Projects\TelubeeRobotAgent\RobotSerialPort.h
file path:	C:\Development\mrayEngine\Projects\TelubeeRobotAgent
file base:	RobotSerialPort
file ext:	h
author:		Charith Fernando

purpose:
*********************************************************************/

#ifndef __ModBus__
#define __ModBus__

// axis definition
#define X_AXIS				0x01
#define Y_AXIS				0x02

#define ON					0xFF00
#define OFF					0x0000

// Function Commands
#define READ_DO				0x01
#define READ_INPUT_STATUS	0x02
#define READ_HOLD_REG		0x03
#define READ_INPUT_REG		0x04
#define WRITE_DO			0x05
#define WRITE_HOLD_REG		0x06
#define READ_EXCEPTION		0x07
#define WRITE_MULT_DO		0x0F
#define WRITE_HOLD_REG		0x10
#define QUERY_SLAVE_ID		0x11
#define READ_WRITE_REG		0x17


// operation commands
#define SAFETY				0x0401
#define SERVO				0x0403
#define ALARM_RST			0x0407
#define BREAK_REL			0x0408

#define PAUSE				0x040A
#define HOME				0x040B
#define START_POS			0x040C
#define JOG_MODE			0x0411
#define TEACH_MODE			0x0414
#define POS_LOAD			0x0415
#define JOG_FW				0x0416
#define JOG_RW				0x0417

#define START_POS7			0x0418
#define START_POS6			0x0419
#define START_POS5			0x041A
#define START_POS4			0x041B
#define START_POS3			0x041C
#define START_POS2			0x041D
#define START_POS1			0x041E
#define START_POS1			0x041F

#define LOAD_CELL			0x0426
#define MODBUS_MODE			0x0427
#define STOP				0x042C

#define PCMD				0x9900


#endif


