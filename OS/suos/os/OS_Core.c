#include <STC15F2K60S2.H>
#include <OS_Core.h>
#include <OS.h>
#include <stdlib.h>

//	����ջ�ռ�
uint8_t idata SystemStack[StackSize]; //ϵͳջ��ϵͳ����ʹ��
uint8_t idata UserStack[StackSize];		//�û�����ջ���û�����ʹ��,��ת��

//	���ٶѿռ�
uint8_t xdata HeapMemory[HeapSize];

//	����CPU�ֳ����浽����PCB����תվ
uint8_t  PSW_Backups;
uint8_t  R0_Backups;
uint8_t  R1_Backups;
uint8_t  R2_Backups;
uint8_t  R3_Backups;
uint8_t  R4_Backups;
uint8_t  R5_Backups;
uint8_t  R6_Backups;
uint8_t  R7_Backups;
uint8_t  ACC_Backups;
uint8_t  B_Backups;
uint8_t  SP_Backups;
uint8_t  DPH_Backups;
uint8_t  DPL_Backups;

//ϵͳPCB������
struct PCB_Struct *PCB_IndexTable[PCB_MaxNumber];
uint8_t PCB_Number  = 0;	//PCB���� = PCB_Number + 1
uint8_t	PCB_Current = 0;	//��ǰ����PID

uint8_t xdata temp = 0;

void main()
{
	Core_InitSystem();
	Create_Process(SystemProcess);
	PCB_IndexTable[0]->SP = (uint8_t)UserStack-1;//ϵͳ����ֱ�ӿ�ʼ����
	PCB_IndexTable[0]->ProcessState = Running;
	PCB_IndexTable[0]->Function();
	while (1); //����
}

/**	
*		���ܣ���ʼ��ϵͳ
*
*/
void Core_InitSystem(void)
{
	//	��ʼ���ѿռ� λ���ڲ���չRAM ��0x00��ַ��ʼ��400Byte
	init_mempool(HeapMemory,HeapSize);
	//	��ʼ����ʱ�� 2ms�ж�һ�Σ���ʱ��ƬΪ2ms
	AUXR |= 0x80;			//1T ģʽ
	TMOD &= 0xF0;			//16λ�Զ�����
	TL0 = 0x80;				//��ֵװ��
	TH0 = 0x44;				
	TF0 = 0;					//��־���
	TR0 = 1;					//������ʱ
	ET0 = 1;
	//	��ʼ��SPI�жϣ�SPI�ӿڲ���ʹ�ã����жϱ������û�̬�л�����̬
	//SPCTL = SPCTL & 0XBF; //�ر�spi
	//IE2 = IE2 | 0X02;	// OPEN SPI INTERRUPT
	
	EA  = 1;					//���ж�
}

/**
*		���ܣ������½���
*		������������ڵ�ַ
*		����ֵ�����̵�PID��-1��ʾ����ʧ��
*/
int8_t Create_Process(void (*Function)(void))
{
	if (PCB_Number == PCB_MaxNumber-1) return -1; //���������Ѿ�����
	PCB_IndexTable[PCB_Number] = (struct PCB_Struct*)malloc(sizeof(struct PCB_Struct)); //Ϊ��������PCB ����PCB��������
	PCB_IndexTable[PCB_Number]->Function = Function;		//���������ڵ�ַ
	PCB_IndexTable[PCB_Number]->ProcessState = Ready;		//���ý���Ϊ����̬
	PCB_IndexTable[PCB_Number]->BlockEvent = 0;					//�������¼�
	PCB_IndexTable[PCB_Number]->s = SemaphoreNull;			//û���ź�������
	PCB_IndexTable[PCB_Number]->PID = PCB_Number;				//���̱�ʶ������
	PCB_IndexTable[PCB_Number]->SP = (uint8_t)UserStack+1;//����ջָ�룬��ʼʱջ�д�ź�����ڵ�ַ
	PCB_IndexTable[PCB_Number]->ProcessStack[0] = (uint16_t)Function & 0x00ff;	//������ڵ�ַ����8λ
	PCB_IndexTable[PCB_Number]->ProcessStack[1] = ((uint16_t)Function>>8) & 0x00ff;	//��8λ
	PCB_IndexTable[PCB_Number]->PSW = 0;
	PCB_IndexTable[PCB_Number]->ACC = 0;
	PCB_IndexTable[PCB_Number]->B   = 0;
	PCB_IndexTable[PCB_Number]->R0  = 0;
	PCB_IndexTable[PCB_Number]->R1  = 0;
	PCB_IndexTable[PCB_Number]->R2  = 0;
	PCB_IndexTable[PCB_Number]->R3  = 0;
	PCB_IndexTable[PCB_Number]->R4  = 0;
	PCB_IndexTable[PCB_Number]->R5  = 0;
	PCB_IndexTable[PCB_Number]->R6  = 0;
	PCB_IndexTable[PCB_Number]->R7  = 0;
	PCB_IndexTable[PCB_Number]->DPL = 0;
	PCB_IndexTable[PCB_Number]->DPH = 0;
	PCB_Number++;
	return (int8_t)PCB_IndexTable[PCB_Number]->PID;
}

/**
*		���ܣ����̿��ƺ���
					ɨ����̶��У��ͷ�����ֹ���̵�PCB�ռ䣬���ѷ�����������������
*					���ý����л�����
*		��������
*		����ֵ����
*/
void Control_Process(void)
{
	EA = 0;
	Refresh_Process();
	Switch_Process();
	EA = 1;
}

/**
*		���ܣ������л����� �ɶ�ʱ��0�жϷ��������� ʵ��ʱ��Ƭ��ת
*		��������
*		����ֵ����
*/
void Switch_Process(void)
{
	uint8_t i;
	//	0.�ж��Ƿ���Ҫ�л����̣���û�д��ھ���̬�Ľ����������л�
	bit Unwanted = 1;			//	����Ҫ�л����̱�־ Ϊ1ʱ���л�
	for (i = 0;i<=PCB_Number;i++) 
		if (PCB_IndexTable[i]->ProcessState == Ready)
			Unwanted = 0;
	if (Unwanted) return;
	//	1.���û�ջ�ռ临�Ƶ��Լ���PCB,�ڳ�������һ������
	for (i=0;i<StackSize;i++)
		PCB_IndexTable[PCB_Current]->ProcessStack[i] = UserStack[i];
	// 	2.��CPU�������Ƶ�PCB,�ڳ�������һ������
	PCB_IndexTable[PCB_Current]->PSW = PSW_Backups;
	PCB_IndexTable[PCB_Current]->ACC = ACC_Backups;
	PCB_IndexTable[PCB_Current]->B   = B_Backups;
	PCB_IndexTable[PCB_Current]->R0  = R0_Backups;
	PCB_IndexTable[PCB_Current]->R1  = R1_Backups;
	PCB_IndexTable[PCB_Current]->R2  = R2_Backups;
	PCB_IndexTable[PCB_Current]->R3  = R3_Backups;
	PCB_IndexTable[PCB_Current]->R4  = R4_Backups;
	PCB_IndexTable[PCB_Current]->R5  = R5_Backups;
	PCB_IndexTable[PCB_Current]->R6  = R6_Backups;
	PCB_IndexTable[PCB_Current]->R7  = R7_Backups;
	PCB_IndexTable[PCB_Current]->DPL = DPL_Backups;
	PCB_IndexTable[PCB_Current]->DPH = DPH_Backups;
	PCB_IndexTable[PCB_Current]->SP  = SP_Backups;
	//  3.ѡ����һ�������Ľ��̲��л�
	//
	i = PCB_Current;
	while(1)   /*���н��̶���������ô�죿*/
	{
		if (i==PCB_Number) 
			i=0;																				//	ѭ������
		if (PCB_IndexTable[i]->ProcessState == Ready)	
			break; 																			//	�ҵ���һ�������Ľ����ˣ��˳�Ѱ��
		i++;																		
	} 
	if (PCB_IndexTable[PCB_Current]->ProcessState == Running) //�����ǰ����Ϊ����̬
		PCB_IndexTable[PCB_Current]->ProcessState = Ready;	//��ǰ��������Ϊ����
	PCB_Current = i;
	PCB_IndexTable[PCB_Current]->ProcessState = Running;//�ҵ��Ľ�������Ϊ����״̬
	//	4.���ҵ��Ľ���CPU���ֳ���Ϣ���Ƶ���תվ
	PSW_Backups = PCB_IndexTable[PCB_Current]->PSW;
	ACC_Backups = PCB_IndexTable[PCB_Current]->ACC;
	B_Backups   = PCB_IndexTable[PCB_Current]->B;
	R0_Backups  = PCB_IndexTable[PCB_Current]->R0;
	R1_Backups  = PCB_IndexTable[PCB_Current]->R1;
	R2_Backups  = PCB_IndexTable[PCB_Current]->R2;
	R3_Backups  = PCB_IndexTable[PCB_Current]->R3;
	R4_Backups  = PCB_IndexTable[PCB_Current]->R4;
	R5_Backups  = PCB_IndexTable[PCB_Current]->R5;
	R6_Backups  = PCB_IndexTable[PCB_Current]->R6;
	R7_Backups  = PCB_IndexTable[PCB_Current]->R7;
	DPL_Backups = PCB_IndexTable[PCB_Current]->DPL;
	DPH_Backups = PCB_IndexTable[PCB_Current]->DPH;
	SP_Backups  = PCB_IndexTable[PCB_Current]->SP;
	//	5.��ջ�ռ��PCB���Ƶ��û�ջ
	for (i=0;i<StackSize;i++)
		UserStack[i] = PCB_IndexTable[PCB_Current]->ProcessStack[i];
}

/**
*		���ܣ����������л����� ���û����̽��Լ�����ʱ�л�����
*		��������
*		����ֵ����
*/
void BlockSwitch_Process(void)
{
	uint8_t i;
	//	0.�ж��Ƿ���Ҫ�л����̣���û�д��ھ���̬�Ľ����������л�
	bit Unwanted = 1;			//	����Ҫ�л����̱�־ Ϊ1ʱ���л�
	for (i = 0;i<=PCB_Number;i++) 
		if (PCB_IndexTable[i]->ProcessState == Ready)
			Unwanted = 0;
	if (Unwanted) return;
	//	1.���û�ջ�ռ临�Ƶ��Լ���PCB,�ڳ�������һ������
	for (i=0;i<StackSize;i++)
		PCB_IndexTable[PCB_Current]->ProcessStack[i] = UserStack[i];
	// 	2.��CPU�������Ƶ�PCB,�ڳ�������һ������
	PCB_IndexTable[PCB_Current]->PSW = PSW_Backups;
	PCB_IndexTable[PCB_Current]->ACC = ACC_Backups;
	PCB_IndexTable[PCB_Current]->B   = B_Backups;
	PCB_IndexTable[PCB_Current]->R0  = R0_Backups;
	PCB_IndexTable[PCB_Current]->R1  = R1_Backups;
	PCB_IndexTable[PCB_Current]->R2  = R2_Backups;
	PCB_IndexTable[PCB_Current]->R3  = R3_Backups;
	PCB_IndexTable[PCB_Current]->R4  = R4_Backups;
	PCB_IndexTable[PCB_Current]->R5  = R5_Backups;
	PCB_IndexTable[PCB_Current]->R6  = R6_Backups;
	PCB_IndexTable[PCB_Current]->R7  = R7_Backups;
	PCB_IndexTable[PCB_Current]->DPL = DPL_Backups;
	PCB_IndexTable[PCB_Current]->DPH = DPH_Backups;
	PCB_IndexTable[PCB_Current]->SP  = SP_Backups;
	//  3.ѡ����һ�������Ľ��̲��л�
	//
	i = PCB_Current;
	while(1)   /*���н��̶���������ô�죿*/
	{
		if (i==PCB_Number) 
			i=0;																				//	ѭ������
		if (PCB_IndexTable[i]->ProcessState == Ready)	
			break; 																			//	�ҵ���һ�������Ľ����ˣ��˳�Ѱ��
		i++;																		
	} 
	if (PCB_IndexTable[PCB_Current]->ProcessState == Running) //�����ǰ����Ϊ����̬
		PCB_IndexTable[PCB_Current]->ProcessState = Ready;	//��ǰ��������Ϊ����
	PCB_Current = i;
	PCB_IndexTable[PCB_Current]->ProcessState = Running;//�ҵ��Ľ�������Ϊ����״̬
	//	4.���ҵ��Ľ���CPU���ֳ���Ϣ���Ƶ���תվ
	PSW_Backups = PCB_IndexTable[PCB_Current]->PSW;
	ACC_Backups = PCB_IndexTable[PCB_Current]->ACC;
	B_Backups   = PCB_IndexTable[PCB_Current]->B;
	R0_Backups  = PCB_IndexTable[PCB_Current]->R0;
	R1_Backups  = PCB_IndexTable[PCB_Current]->R1;
	R2_Backups  = PCB_IndexTable[PCB_Current]->R2;
	R3_Backups  = PCB_IndexTable[PCB_Current]->R3;
	R4_Backups  = PCB_IndexTable[PCB_Current]->R4;
	R5_Backups  = PCB_IndexTable[PCB_Current]->R5;
	R6_Backups  = PCB_IndexTable[PCB_Current]->R6;
	R7_Backups  = PCB_IndexTable[PCB_Current]->R7;
	DPL_Backups = PCB_IndexTable[PCB_Current]->DPL;
	DPH_Backups = PCB_IndexTable[PCB_Current]->DPH;
	SP_Backups  = PCB_IndexTable[PCB_Current]->SP;
	//	5.��ջ�ռ��PCB���Ƶ��û�ջ
	for (i=0;i<StackSize;i++)
		UserStack[i] = PCB_IndexTable[PCB_Current]->ProcessStack[i];
}
/**
*		���ܣ����н���
*		��������
*		����ֵ����
*/
//void Run_Process(uint8_t PCB_ID)
//{
//	
//}

/**
*		���ܣ����½���״̬ 
*					��Ҫ�Ǽ�������������ȴ����¼��Ƿ�����
*		��������
*		����ֵ����
*/
void Refresh_Process(void)
{
	uint8_t i;
	for (i = 0;i<=PCB_Number;i++)
	{
		if (PCB_IndexTable[i]!=(void*)0 && PCB_IndexTable[i]->ProcessState == Block) ////PCB�ǿ� ���̱�����
			{
				///*�ź����������������������*/
				if (PCB_IndexTable[i]->BlockEvent == 0 && PCB_IndexTable[i]->s == SemaphoreNull)
					PCB_IndexTable[i]->ProcessState = Ready;
				else if (PCB_IndexTable[i]->BlockEvent>0)
				{
					--PCB_IndexTable[i]->BlockEvent;
					if (PCB_IndexTable[i]->BlockEvent == 0) PCB_IndexTable[i]->ProcessState = Ready;
				}
					/*
				if (PCB_IndexTable[i]->BlockEvent==0&&*(PCB_IndexTable[i]->s)>0)//��ʱ��Ϊ0���ź���>0 ����
					{
						PCB_IndexTable[i]->ProcessState = Ready;
						continue;
					}
				if (PCB_IndexTable[i]->BlockEvent<=60000) 
					--PCB_IndexTable[i]->BlockEvent;
				else ;//����ӵ���������¼�
				if (PCB_IndexTable[i]->BlockEvent==0&&*(PCB_IndexTable[i]->s)>0)//��ʱ��Ϊ0���ź���>0 ����
					{PCB_IndexTable[i]->ProcessState = Ready;continue;}
				*/
			}//if end
	}//for end
}

/**
*		���ܣ��������̣���ʱtime�������
*		������time ��Ҫ��ʱ��ʱ����msΪ��λ��ȡֵ��Χ��2-120000 
*		����ֵ�����óɹ�SUCCESS��ʧ��FAIL
*/
STATUS DelayBlock_Proccess(uint16_t time)
{
	if (time>60000 || time < 2) return FAIL;
	PCB_IndexTable[PCB_Current]->BlockEvent = time / 2;
	PCB_IndexTable[PCB_Current]->ProcessState = Block;
	//�л�����	��ʱûʵ��
	//Ŀǰ��ä��	��ʱ��Ƭ�����Զ��л�
	while (PCB_IndexTable[PCB_Current]->ProcessState==Block);
	return SUCCESS;
}

/**
*		���ܣ��ź���P����
*		�������ź�����Semaphore������
*		���أ���
*/
void SemaphoreWait(Semaphore *s)
{
	EA = 0;//���жϣ�ʵ��ԭ�Ӳ���
	*s = (*s) - 1;
	if ((*s)<0) 
	{
		PCB_IndexTable[PCB_Current]->s = s;
		PCB_IndexTable[PCB_Current]->ProcessState = Block;
	}
	EA = 1;
	//Ŀǰ��ä��	��ʱ��Ƭ�����Զ��л�
	while (PCB_IndexTable[PCB_Current]->ProcessState==Block);
}
/**
*		���ܣ��ź���V����
*		�������ź�����Semaphore������
*		���أ���
*/
void SemaphoreSignal(Semaphore *s)
{
	EA = 0;
	++(*s); 	/*���Ѳ�����ô�㣿�ź����������ͻ���������*/
	if ((*s)<=0) 
	{
		uint8_t i = 0;
		for (;i<=PCB_Number;i++)
			if (PCB_IndexTable[i]->ProcessState==Block&&PCB_IndexTable[i]->s==s)
			{
				PCB_IndexTable[i]->ProcessState=Ready;
				PCB_IndexTable[i]->s = SemaphoreNull;
				break;
			}
	}
	EA = 1;	
}

/**
*		���ܣ�ϵͳ���̣�ϵͳ��ʼ����ĵ�һ�����̣�
*					�����û�����Ϊ�ý��̵��ӽ��̡�
*		��������
*		����ֵ����
*/
void SystemProcess(void)
{
	main_t();
	while(1) temp += 1;
}