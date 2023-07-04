/* ���û������ֹ������ͷ�ļ� */
#ifndef _CORE_H_
#define _CORE_H_

#include <OS_TypeDefine.h>

//	��������������
#define PCB_MaxNumber 10

//	����ջ�ռ��С
#define StackSize 10

//	�ѿռ��С
#define HeapSize 2000

/*	PCB�ṹ����
*		�����¼���ѡ�
*						BlockEvent <= 60000 ʱ
*									����Ϊ��ʱ������ÿ��ˢ�½��̱�ʱִ�� --BlockEvent
*									BlockEvent == 0 ʱ �����Զ�����
***/
struct PCB_Struct
{
	uint8_t ProcessStack[StackSize];											//����ջ��������̶ϵ�
	enum {Ready=1,Block=2,Running=3} ProcessState;				//����״̬
	uint8_t PID;																					//���̱�ʶ��
	uint8_t PSW,R0,R1,R2,R3,R4,R5,R6,R7,ACC,B,DPH,DPL,SP; //�����ֳ�
	void (*Function)(void);																//���̺������
	uint16_t BlockEvent;																	//�����¼�
	Semaphore *s;
};


//	����ջ�ռ�
extern uint8_t idata SystemStack[StackSize]; 	//ϵͳջ��ϵͳ����ʹ��
extern uint8_t idata UserStack[StackSize];		//�û�����ջ���û�����ʹ��,��ת��

//	��������CPU�ֳ����浽����PCB����תվ
extern uint8_t  PSW_Backups;
extern uint8_t  R0_Backups;
extern uint8_t  R1_Backups;
extern uint8_t  R2_Backups;
extern uint8_t  R3_Backups;
extern uint8_t  R4_Backups;
extern uint8_t  R5_Backups;
extern uint8_t  R6_Backups;
extern uint8_t  R7_Backups;
extern uint8_t  ACC_Backups;
extern uint8_t  B_Backups;
extern uint8_t  SP_Backups;
extern uint8_t  DPH_Backups;
extern uint8_t  DPL_Backups;

//�������� ϵͳ�������û����ɵ���
void Core_InitSystem(void);
void SystemProcess(void);
void Switch_Process(void);
void Control_Process(void);
void Refresh_Process(void);
void BlockSwitch_Process(void);//Ŀǰ����


#endif
