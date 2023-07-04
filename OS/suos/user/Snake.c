#include <OS.h>
#include <Snake.h>
#include <stdlib.h>
#include <handle.h>
#include "indicator.h"
/*
*	map: 	20*20
*	size:	210*210
*/
#define SnakeMaxSize 200
uint8_t food2str[4];

enum DIR{UP=1,DOWN,LEFT,RIGHT};

struct PointType{
	uint8_t x;
	uint8_t y;
};


struct SnakeType{
	struct PointType body[SnakeMaxSize];
	enum DIR Direction;
	uint8_t SnakeHead;	//��ͷ
	uint8_t SnakeTail;	//��β�����һ���ո�
	uint8_t SnakeSize;
};

sbit HandleSwitch = P1^3;	//��ʼ��
enum{GameReady = 1,GameLoading,Gameing,GameOver} GameStatus = GameReady;	//��Ϸ״̬
struct PointType food;	//ʳ��

/*��ʼ����*/
void SnakeInit(struct SnakeType *Snake)
{
	Snake->SnakeHead = 3;
	Snake->SnakeTail = 1;
	Snake->Direction = RIGHT;
	Snake->body[1].x = 3;
	Snake->body[2].x = 4;
	Snake->body[3].x = 5;
	Snake->body[1].y = 10;
	Snake->body[2].y = 10;
	Snake->body[3].y = 10;
	Snake->SnakeSize = 3;
}

/*�����ȡʳ��λ�ã�α�����*/
void GetFoodRandom(uint8_t x,uint8_t y)
{
	food.x = (x*3+17)%20;
	food.y = (y*3+17)%20;
}

/*��λ��ת��Ϊ�ַ���*/
uint8_t* Number2String(uint16_t n)
{
	food2str[0] = n/100 + '0';
	food2str[1] = (n%100)/10 + '0';
	food2str[2] = n%10 + '0';
	food2str[3] = 0;
	return food2str;
}

/*��ʾʳ��*/
void DisplayFood()
{
	DisplaySendString("cirs ");
	DisplaySendString(Number2String(food.x*10+5));
	DisplaySendString(",");
	DisplaySendString(Number2String(food.y*10+5));
	DisplaySendString(",4,YELLOW");
	DisplaySendEnd();
}

/*��ʾ����*/
void DisplayScore(uint8_t score)
{
	DisplaySendString("t0.txt=\" Score:");
	DisplaySendString(Number2String(score));
	DisplaySendString("\"");
	DisplaySendEnd();
}


/*���ߵĵ�*/
void DisplaySnakeDot(uint8_t x,uint8_t y,uint8_t color)
{
	DisplaySendString("fill ");
	DisplaySendString(Number2String(x*10));
	DisplaySendString(",");
	DisplaySendString(Number2String(y*10));
	DisplaySendString(",10,10,");
	switch(color)
	{
		case 0: DisplaySendString("GREEN");break;
		case 1: DisplaySendString("BROWN");break;
		case 2: DisplaySendString("BLACK");break;
	}
	DisplaySendEnd();
}

/*	������ɵ�ʳ��λ���Ƿ�Ϸ��������������ߵ�����	*/
/*	return	0-�Ϸ� 1-�Ƿ�	*/
uint8_t CheckFood(struct SnakeType *Snake)
{
	uint8_t i=Snake->SnakeTail;
	if (Snake->SnakeTail > Snake->SnakeHead) for(;(i>=Snake->SnakeTail&&i<SnakeMaxSize)||(i<Snake->SnakeHead);)
	{
		if ((food.x==Snake->body[i].x)&&(food.y==Snake->body[i].y)) return 1;
		i=(i+1)%SnakeMaxSize;
	}else for (;i<Snake->SnakeHead;)
	{
		if ((food.x==Snake->body[i].x)&&(food.y==Snake->body[i].y)) return 1;
		i++;
	}
	return 0;
}

/*�ж����Ƿ�Ե����Լ�*/
uint8_t isEatSelf(struct SnakeType *Snake)
{
	uint8_t i=Snake->SnakeTail;
	if (Snake->SnakeTail>Snake->SnakeHead) for(;(i>=Snake->SnakeTail&&i<SnakeMaxSize)||(i<Snake->SnakeHead);)
	{
		if ((Snake->body[Snake->SnakeHead].x==Snake->body[i].x)&&(Snake->body[Snake->SnakeHead].y==Snake->body[i].y)) return 1;
		i=(i+1)%SnakeMaxSize;
	}else for (;i<Snake->SnakeHead;)
	{
		if ((Snake->body[Snake->SnakeHead].x==Snake->body[i].x)&&(Snake->body[Snake->SnakeHead].y==Snake->body[i].y)) return 1;
		i++;
	}
	return 0;
}

/*��ȡ�ֱ��ķ���*/
enum DIR GetHandleDIR(enum DIR old)
{
	uint8_t y=HandleReadX();
	uint8_t x=HandleReadY();
	if (x==0xff) return RIGHT;
	if (x==0x00) return LEFT;
	if (y==0xff) return DOWN;
	if (y==0x00) return UP;
	return old;
}

void UserProcess_Snake()
{
	struct SnakeType *Snake = (struct SnakeType*)malloc(sizeof(struct SnakeType));
	enum DIR ReadDir = RIGHT;
	uint8_t tempx,tempy;
	
	while(1)
	{
		switch (GameStatus)
		{
			case	GameReady://��Ϸ׼������
				while(HandleSwitch); //���¿���ǰ�ȴ�
				signal = 1;
				GameStatus = GameLoading;
				DisplaySendCommand("page 1"); //�л����� ����Ҫ�ٽ���
				/*�����ڶ�������*/
				break;
			case	GameLoading:
				SnakeInit(Snake); //��ʼ����
				GetFoodRandom(Snake->body[Snake->SnakeTail].x,Snake->body[Snake->SnakeTail].y);	//��ȡʳ�����λ��
				while (CheckFood(Snake)) food.x++;
				SemaphoreWait(&screen);
				DisplaySendCommand("fill 0,0,210,210,BLACK");
				DisplaySendCommand("fill 30,100,30,10,GREEN");	//����
				DisplaySendCommand("fill 50,100,10,10,BROWN");	//����
				DisplayFood();																//��ʳ��
				DisplayScore(Snake->SnakeSize-3);
				SemaphoreSignal(&screen);
				GameStatus = Gameing;
				break;
			case	Gameing:
				ReadDir = GetHandleDIR(Snake->Direction);
				if ((ReadDir==UP)&&(Snake->Direction==DOWN)) ReadDir = DOWN;
				if ((ReadDir==DOWN)&&(Snake->Direction==UP)) ReadDir = UP;
				if ((ReadDir==LEFT)&&(Snake->Direction==RIGHT)) ReadDir = RIGHT;
				if ((ReadDir==RIGHT)&&(Snake->Direction==LEFT)) ReadDir = LEFT;
				Snake->Direction = ReadDir;
				tempx = Snake->body[Snake->SnakeHead].x;
				tempy = Snake->body[Snake->SnakeHead].y;
				SemaphoreWait(&screen);
				DisplaySnakeDot(Snake->body[Snake->SnakeHead].x,Snake->body[Snake->SnakeHead].y,0); //���߲���
				SemaphoreSignal(&screen);
				Snake->SnakeHead = (Snake->SnakeHead+1)%SnakeMaxSize;
				switch (Snake->Direction)
				{
					case UP:
						Snake->body[Snake->SnakeHead].x = tempx;
						Snake->body[Snake->SnakeHead].y = tempy - 1;
						if (tempy == 0) Snake->body[Snake->SnakeHead].y = 20;
						break;
					case DOWN:
						Snake->body[Snake->SnakeHead].x = tempx;
						Snake->body[Snake->SnakeHead].y = tempy + 1;
						if (tempy == 20) Snake->body[Snake->SnakeHead].y = 0;
						break;
					case LEFT:
						Snake->body[Snake->SnakeHead].x = tempx - 1;
						Snake->body[Snake->SnakeHead].y = tempy;
						if (tempx == 0) Snake->body[Snake->SnakeHead].x = 20;
						break;
					case RIGHT:
						Snake->body[Snake->SnakeHead].x = tempx + 1;
						Snake->body[Snake->SnakeHead].y = tempy;
						if (tempx == 20) Snake->body[Snake->SnakeHead].x = 0;
						break;
				}
				
				SemaphoreWait(&screen);
				DisplaySnakeDot(Snake->body[Snake->SnakeHead].x,Snake->body[Snake->SnakeHead].y,1);//����ͷ
				DisplaySnakeDot(Snake->body[Snake->SnakeTail].x,Snake->body[Snake->SnakeTail].y,2);//Ĩȥ��β��
				SemaphoreSignal(&screen);
				DelayBlock_Proccess(100);
				//�ж��Ƿ�Ե���ʵ��
				if ((Snake->body[Snake->SnakeHead].x==food.x)&&(food.y==Snake->body[Snake->SnakeHead].y))
				{
					Snake->SnakeSize++;
					GetFoodRandom(Snake->body[Snake->SnakeTail].x,Snake->body[Snake->SnakeTail].y);
					while (CheckFood(Snake)) food.x++;
					SemaphoreWait(&screen);
					DisplayFood();	//��ʳ��
					DisplayScore(Snake->SnakeSize-3);
					SemaphoreSignal(&screen);
				}else if (isEatSelf(Snake)) //�ж��Ƿ�Ե����Լ�
				{
					GameStatus = GameOver;
				}else{ //��βԪ�س���
					Snake->SnakeTail = (Snake->SnakeTail+1)%SnakeMaxSize;
				}
				break;
			case	GameOver:
				while(HandleSwitch); //���¿���ǰ�ȴ�
				GameStatus = GameLoading;
				break;
		}
	}
}





