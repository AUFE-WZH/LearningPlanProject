#include "OS.h"
#include "Snake.h"
#include "handle.h"
#include "indicator.h"
#include "Pellet.h"
 //�����ź��� ʵ����Ļ�Ļ��⹲�� ����1
Semaphore screen = 1;
uint8_t signal = 0;
//�˺����ڽ��ɽ����豸��ʼ���ʹ������̣���ֹ����������
void main_t()
{
	HandleInit();			//��ʼ���ֱ�
	IndicatorInit();	//��ʼ����Ļ
	Create_Process(UserProcess_Snake);	//����̰���߽���
	Create_Process(UserProcess_Pellet); //�����������
}
