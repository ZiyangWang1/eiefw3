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

//***************************************NRF24L01寄存器指令*******************************************************
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
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
