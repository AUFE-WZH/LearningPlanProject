#ifndef _SUOS_H_
#define _SUOS_H_
#include <OS_TypeDefine.h>
#include <STC15F2K60S2.H>

extern uint8_t xdata temp;
void main_t(void);															//�ڸú����д����û�����
int8_t Create_Process(void (*Function)(void)); 	//ʹ�ô˺��������û�����
STATUS DelayBlock_Proccess(uint16_t time);			//����ǰ��������time���룬���ϴ�
void SemaphoreWait(Semaphore *s);
void SemaphoreSignal(Semaphore *s);
#endif