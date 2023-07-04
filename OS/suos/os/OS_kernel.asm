;
;	��Ȩ������(suxin1412@qq.com)��2019
;	
;	�ļ���Core.asm
;	�汾��VER 1.0
;	ʱ�䣺2019/03/04
;	������ϵͳ�ںˣ�ʵ�ֽ��̵��ȹ���
;

	PUBLIC	?C_STARTUP        	;���ⲿ������ROM������ʼλ��
	
;	�����ⲿ����
	EXTRN	IDATA(SystemStack)	;ϵͳջ��ַ
	EXTRN	IDATA(UserStack)	;�û�����ջ��ַ
	EXTRN	CODE(?C_START)		;main������ڵ�ַ
	EXTRN	CODE(Control_Process,BlockSwitch_Process,UartTI)	;ϵͳ����
	EXTRN	DATA(SP_Backups,PSW_Backups,R0_Backups,R1_Backups,R2_Backups,R3_Backups)	
	EXTRN	DATA(ACC_Backups,B_Backups,R4_Backups,R5_Backups,R6_Backups,R7_Backups)
	EXTRN	DATA(DPH_Backups,DPL_Backups)
	
	;PUBLIC FXC
	;EXTRN CODE(xdxd)
	
;	PCָ�����
?C_STARTUP :
	ORG 0X00
	JMP System_Reset

	;�ⲿ0�ж�����
	;ORG		0x03
	;JMP		Handle_External_0_Interrupt
	
	;��ʱ��0�ж�����
	ORG		0x0B
	JMP		Timer0_ServiceFunction
	
	;SPI �ж�����
	ORG 	0X4B
	JMP		UserBlock_ServiceFunction
	
	;UART �ж�����
	ORG 	0X23
	JMP		Uart_ServiceFunction

; ϵͳ��λ
System_Reset:
	MOV 	SP, #(UserStack - 1)	;��ʼ��ϵͳջ
	JMP		?C_START
RET

;����CPU�ֳ���Ϣ����������
Save_CPU_Context:
	MOV 	PSW_Backups, PSW
	MOV 	ACC_Backups, A
	MOV 	B_Backups, B
	MOV		R0_Backups, R0
	MOV		R1_Backups, R1
	MOV		R2_Backups, R2
	MOV		R3_Backups, R3
	MOV		R4_Backups, R4
	MOV		R5_Backups, R5
	MOV		R6_Backups, R6
	MOV		R7_Backups, R7
	MOV		DPL_Backups, DPL
	MOV		DPH_Backups, DPH
RET

;�ָ�CPU�ֳ���Ϣ���л�����
Recovery_CPU_Context:
	MOV 	PSW, PSW_Backups
	MOV 	A, ACC_Backups
	MOV 	B, B_Backups
	MOV		R0, R0_Backups
	MOV		R1, R1_Backups
	MOV		R2, R2_Backups
	MOV		R3, R3_Backups
	MOV		R4, R4_Backups
	MOV		R5, R5_Backups
	MOV		R6, R6_Backups
	MOV		R7, R7_Backups
	MOV		DPL, DPL_Backups
	MOV		DPH, DPH_Backups
RET

;��ʱ��0�жϷ����� ���̵����ڴ�ʵ�� 
Timer0_ServiceFunction:
	MOV 	SP_Backups, SP			;����ǰ�û���SPָ�뱣��
	MOV 	SP, #(SystemStack - 1)	;��ϵͳջ����Ϊ��ǰջ
	LCALL 	Save_CPU_Context		;�����ֳ�����
	LCALL 	Control_Process			;���ý��̵��Ⱥ��� ���ú�ϵͳ���л��ֳ���Ϣ�Ͷϵ�
	LCALL 	Recovery_CPU_Context	;�ָ��ֳ���Ϣ
	MOV 	SP, SP_Backups			;�ָ�ջָ��
RETI

UserBlock_ServiceFunction:
	MOV 	SP_Backups, SP			;����ǰ�û���SPָ�뱣��
	MOV 	SP, #(SystemStack - 1)	;��ϵͳջ����Ϊ��ǰջ
	LCALL 	Save_CPU_Context		;�����ֳ�����
	LCALL 	BlockSwitch_Process		;���ý��̵��Ⱥ��� ���ú�ϵͳ���л��ֳ���Ϣ�Ͷϵ�
	LCALL 	Recovery_CPU_Context	;�ָ��ֳ���Ϣ
	MOV 	SP, SP_Backups			;�ָ�ջָ��
RETI

Uart_ServiceFunction:
	LCALL UartTI
RETI

END