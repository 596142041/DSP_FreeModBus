/*
 * main.c
 *
 *  Created on: 2016年9月11日
 *      Author: lpr
 */
#include "main.h"
eMBErrorCode Code, error_code;
void Modbus_Data_Init(void);
int main(void)
{
	InitSysCtrl();
	Modbus_Data_Init();
	DINT;
	DRTM;
	InitPieCtrl();
	IER = 0x00;
	IFR = 0x00;
	InitPieVectTable();
#if DEBUG_FLASH
	MemCopy(&RamfuncsLoadStart,&RamfuncsLoadEnd,&RamfuncsRunStart);
	InitFlash();
#endif
	EALLOW;
	PieVectTable.SCIRXINTC = &serial_Rx_isr;
	PieVectTable.SCITXINTC = &serial_Tx_isr;
	PieVectTable.TINT0 = &cpu_timer2_isr;
	EDIS;
	SPIFLASH_Init();
	//-----------------------------------------------
	//以下函数是自己添加的自定义功能函数
	//主要是对保持寄存器的相关操作
	error_code = eMBRegisterCB(0x43, Fun_ReadHoldingRegister);
	error_code = eMBRegisterCB(0x41, Fun_WriteHoldingRegister);
	error_code = eMBRegisterCB(0x42, Fun_WriteMultipleHoldingRegister);
	//-------------END-------------------------------------
	eMBInit(MB_RTU, 0x0A, 1, 9600, MB_PAR_NONE);
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;	//使能CPU定时器0的中断
	IER |= M_INT8;
	IER |= M_INT1;
	EINT;
	ERTM;
	eMBEnable();
	while (1)
	{
		Code = eMBPoll();
		//Code = eMBPoll();
	}
}
void Modbus_Data_Init(void)
{
	int cnt;
	for (cnt = 0; cnt < REG_INPUT_NREGS; cnt++)
	{
		usRegInputBuf[cnt] = cnt;
	}
	for (cnt = 0; cnt < REG_Coils_NREGS / 8; cnt++)
	{
		CoilsReg_Buf[cnt] = 0;
	}
	for (cnt = 0; cnt < REG_HOLDING_NREGS; cnt++)
	{
		usRegHoldingBuf[cnt] = cnt * 10;
	}
	for (cnt = 0; cnt < REG_Discrete_NREG / 8; cnt++)
	{
		DiscreteReg_Buf[cnt] = 0;
	}
}
