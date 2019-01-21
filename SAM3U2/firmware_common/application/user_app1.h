/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
#define USERAPP_RX_BUFFER_SIZE 128

/* Pin definition */
#define PA_03_C1_MISO    PA_03_HSMCI_MCCK 
#define PA_04_C1_CSN     PA_04_HSMCI_MCCDA 
#define PA_05_C1_IRQ     PA_05_HSMCI_MCDA0 
#define PA_06_C1_SCK     PA_06_HSMCI_MCDA1
#define PA_07_C1_MOSI    PA_07_HSMCI_MCDA2 
#define PA_08_C1_CE      PA_08_SD_CS_MCDA3 

//#define PA_09_C1_GND     PA_08_SD_CS_MCDA3 
//#define PA_10_C1_VCC     PA_04_HSMCI_MCCDA

#define PA_11_C2_GND     PA_11_BLADE_UPIMO
#define PA_12_C2_VCC     PA_12_BLADE_UPOMI 
#define PA_14_C2_CE      PA_14_BLADE_MOSI
#define PA_15_C2_CSN     PA_15_BLADE_SCK 
#define PB_05_C2_SCK     PB_05_TP56 
#define PB_06_C2_MOSI    PB_06_TP58
#define PB_07_C2_IRQ     PB_07_TP60 
#define PB_08_C2_MISO    PB_08_TP62

//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  20  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  20  	// 20 uints TX payload

//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define READ_REG        0x00  	// ���Ĵ���ָ��
#define WRITE_REG       0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//**************************************************************************************

//******************************************************************************************
#define	RX_DR   (sta & 0x40)
#define	TX_DS	(sta & 0x20)
#define	MAX_RT	(sta & 0x10)

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);
void UserApp_AdcCallback(u16 u16Result_);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void inerDelay_us(unsigned char n);
void UserApp1_init_NRF24L01(void);
u8 UserApp1_SPI_RW(u8 uchar);
u8 UserApp1_SPI_Read(u8 reg);
u8 UserApp1_SPI_RW_Reg(u8 reg, u8 value);
u8 UserApp1_SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars);
u8 UserApp1_SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars);
void UserApp1_nRF24L01_TxPacket(unsigned char * tx_buf);

void SPI_Write(u8 uchar);
void SPI_Write_Buf(u8 *pBuf, u8 uchars);
/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_Idle(void);    
static void UserApp1SM_Error(void);         



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/


#endif /* __USER_APP1_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
