/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */

bool G_bUserApp1ADCFlag=FALSE;                /* Global ADC flag signal */
u16 G_u16UserApp1ADCResult=0;                 /* Global ADC result */
u8 sta;
u8 TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};//本地地址
u8 RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};//接收地址

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  
    /* Enable PIO and configurate */
  AT91C_BASE_PIOA->PIO_PER = PA_03_C1_MISO;
  AT91C_BASE_PIOA->PIO_PER = PA_04_C1_CSN;
  AT91C_BASE_PIOA->PIO_PER = PA_05_C1_IRQ;
  AT91C_BASE_PIOA->PIO_PER = PA_06_C1_SCK;
  AT91C_BASE_PIOA->PIO_PER = PA_07_C1_MOSI;
  AT91C_BASE_PIOA->PIO_PER = PA_08_C1_CE;
  //AT91C_BASE_PIOA->PIO_PER = PA_09_C1_GND;
  //AT91C_BASE_PIOA->PIO_PER = PA_10_C1_VCC;
  AT91C_BASE_PIOA->PIO_PER = PA_11_C2_GND;
  AT91C_BASE_PIOA->PIO_PER = PA_12_C2_VCC;
  AT91C_BASE_PIOA->PIO_PER = PA_14_C2_CE;
  AT91C_BASE_PIOA->PIO_PER = PA_15_C2_CSN;
  AT91C_BASE_PIOB->PIO_PER = PB_05_C2_SCK;
  AT91C_BASE_PIOB->PIO_PER = PB_06_C2_MOSI;
  AT91C_BASE_PIOB->PIO_PER = PB_07_C2_IRQ;
  AT91C_BASE_PIOB->PIO_PER = PB_08_C2_MISO;
  
  AT91C_BASE_PIOA->PIO_ODR = PA_03_C1_MISO;
  AT91C_BASE_PIOB->PIO_ODR = PB_08_C2_MISO;
  AT91C_BASE_PIOA->PIO_OER = PA_04_C1_CSN;
  AT91C_BASE_PIOA->PIO_OER = PA_06_C1_SCK;
  AT91C_BASE_PIOA->PIO_OER = PA_07_C1_MOSI;
  AT91C_BASE_PIOA->PIO_OER = PA_08_C1_CE;
  //AT91C_BASE_PIOA->PIO_OER = PA_09_C1_GND;
  //AT91C_BASE_PIOA->PIO_OER = PA_10_C1_VCC;
  AT91C_BASE_PIOA->PIO_OER = PA_11_C2_GND;
  AT91C_BASE_PIOA->PIO_OER = PA_12_C2_VCC;
  AT91C_BASE_PIOA->PIO_OER = PA_14_C2_CE;
  AT91C_BASE_PIOA->PIO_OER = PA_15_C2_CSN;
  AT91C_BASE_PIOB->PIO_OER = PB_05_C2_SCK;
  AT91C_BASE_PIOB->PIO_OER = PB_06_C2_MOSI;
  
  //AT91C_BASE_PIOB->PIO_PPUER = PA_10_C1_VCC;
  //AT91C_BASE_PIOB->PIO_PPUER = PA_12_C2_VCC;
  
  /*AT91C_BASE_PIOA->PIO_IER = PA_05_C1_IRQ;
  AT91C_BASE_PIOB->PIO_IER = PB_07_C2_IRQ;
  
  AT91C_BASE_PIOA->PIO_AIMER = PA_05_C1_IRQ;
  AT91C_BASE_PIOB->PIO_AIMER = PB_07_C2_IRQ;
  
  AT91C_BASE_PIOA->PIO_ESR = PA_05_C1_IRQ;
  AT91C_BASE_PIOB->PIO_ESR = PB_07_C2_IRQ;
  
  AT91C_BASE_PIOA->PIO_FELLSR = PA_05_C1_IRQ;
  AT91C_BASE_PIOB->PIO_FELLSR = PB_07_C2_IRQ;
  
  NVIC_ClearPendingIRQ( IRQn_PIOA );
  NVIC_EnableIRQ( IRQn_PIOA );
  
  NVIC_ClearPendingIRQ( IRQn_PIOB );
  NVIC_EnableIRQ( IRQn_PIOB );
  */

  /* Initialize the GPIO ports */
  
  //AT91C_BASE_PIOA->PIO_CODR = PA_09_C1_GND;
  //AT91C_BASE_PIOA->PIO_SODR = PA_10_C1_VCC;
  AT91C_BASE_PIOA->PIO_CODR = PA_11_C2_GND;
  AT91C_BASE_PIOA->PIO_SODR = PA_12_C2_VCC;
  
  AT91C_BASE_PIOB->PIO_CODR = PB_06_C2_MOSI;
  
  Adc12AssignCallback(ADC12_CH2, UserApp_AdcCallback);
  LCDCommand(LCD_CLEAR_CMD);
  UserApp1_init_NRF24L01();
  
  /* If good initialization, set state to Idle */
  if(1)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

/*----------------------------------------------------------------------------------------------------------------------
Function UserApp_AdcCallback(u16 u16Result_)
Description:
The callback function for ADC of CH1
*/
void UserApp_AdcCallback(u16 u16Result_)
{
  G_bUserApp1ADCFlag=TRUE;
  G_u16UserApp1ADCResult=u16Result_;
}

/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/******************************************************************************************
延时函数
******************************************************************************************/
void inerDelay_us(unsigned char n)
{
  for(;n>0;n--);
}

/****************************************************************************************************
函数：void SPI_Write(uint uchar)
功能：SPI写时序
****************************************************************************************************/
void SPI_Write(u8 uchar)
{
  u8 bit_ctr;
  for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
  {
    if(uchar & 0x80)
    {
      AT91C_BASE_PIOB->PIO_SODR = PB_06_C2_MOSI;
    }
    else
    {
      AT91C_BASE_PIOB->PIO_CODR = PB_06_C2_MOSI;
    }
    
    uchar = (uchar << 1);           // shift next bit into MSB..
    AT91C_BASE_PIOB->PIO_SODR = PB_05_C2_SCK;// Set SCK high..
    inerDelay_us(5);
    AT91C_BASE_PIOB->PIO_CODR = PB_05_C2_SCK;// ..then set SCK low again
    inerDelay_us(5);
  }
}

/*********************************************************************************************************/
/*函数：void SPI_Write_Buf(uchar *pBuf, uchar uchars)*/
/*功能: 用于写数据：pBuf：为待写入数据地址，uchars：写入数据的个数*/
/*********************************************************************************************************/
void SPI_Write_Buf(u8 *pBuf, u8 uchars)
{
  u8 uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_15_C2_CSN;//SPI使能       
  inerDelay_us(20);
  for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
    SPI_Write(*pBuf++);
  AT91C_BASE_PIOA->PIO_SODR = PA_15_C2_CSN;//关闭SPI
}

/******************************************************************************************
测试函数
******************************************************************************************/
u8 NRF24L01_Check(void)
{
  u8 buf=0x0E;    
  UserApp1_SPI_RW_Reg(WRITE_REG+CONFIG ,buf);
  buf=UserApp1_SPI_Read(CONFIG);  
  
  if(buf!=0x0E)             
    return 0;        //失败
  else 
    return 1;         //成功
}

//****************************************************************************************
//NRF24L01初始化
//***************************************************************************************/
void UserApp1_init_NRF24L01(void)
{
  inerDelay_us(100);
  AT91C_BASE_PIOA->PIO_CODR = PA_08_C1_CE;    // chip enable
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;   // Spi  disable 
  AT91C_BASE_PIOA->PIO_CODR = PA_06_C1_SCK;   // 
  UserApp1_SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址
  UserApp1_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
  UserApp1_SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      //  频道0自动	ACK应答允许	
  UserApp1_SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
  UserApp1_SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x00); // 500us + 86us, 10 retrans...
  UserApp1_SPI_RW_Reg(WRITE_REG + RF_CH, 40);        //   设置信道工作为2.4GHZ，收发必须一致
  UserApp1_SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
  UserApp1_SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB
  UserApp1_SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);
  AT91C_BASE_PIOA->PIO_SODR = PA_08_C1_CE;
}

/****************************************************************************************************
函数：uint UserApp1_SPI_RW(uint uchar)
功能：NRF24L01的SPI写时序
****************************************************************************************************/
u8 UserApp1_SPI_RW(u8 uchar)
{
  u8 bit_ctr;
  for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
  {
    if(uchar & 0x80)
    {
      AT91C_BASE_PIOA->PIO_SODR = PA_07_C1_MOSI;
    }
    else
    {
      AT91C_BASE_PIOA->PIO_CODR = PA_07_C1_MOSI;
    }
    
    uchar = (uchar << 1);           // shift next bit into MSB..
    AT91C_BASE_PIOA->PIO_SODR = PA_06_C1_SCK;// Set SCK high..
    if(AT91C_BASE_PIOA->PIO_PDSR & PA_03_C1_MISO)
    {
      uchar |= 0x01;
    }
    else
    {
      uchar &= 0xFE;
    }
  // capture current MISO bit
    AT91C_BASE_PIOA->PIO_CODR = PA_06_C1_SCK;// ..then set SCK low again
  }
  return(uchar);           		  // return read uchar
}

/****************************************************************************************************
函数：uchar SPI_Read(uchar reg)
功能：NRF24L01的SPI时序
****************************************************************************************************/
u8 UserApp1_SPI_Read(u8 reg)
{
  u8 reg_val;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;// CSN low, initialize SPI communication...
  UserApp1_SPI_RW(reg);            // Select register to read from..
  reg_val = UserApp1_SPI_RW(0);    // ..then read registervalue
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;// CSN high, terminate SPI communication
  
  return(reg_val);        // return register value
}

/****************************************************************************************************/
/*功能：NRF24L01读写寄存器函数*/
/****************************************************************************************************/
u8 UserApp1_SPI_RW_Reg(u8 reg, u8 value)
{
  u8 status;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;// CSN low, init SPI transaction
  status = UserApp1_SPI_RW(reg);      // select register
  UserApp1_SPI_RW(value);             // ..and write value to it..
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;// CSN high again
  
  return(status);            // return nRF24L01 status uchar
}

/****************************************************************************************************/
/*函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)*/
/*功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数*/
/****************************************************************************************************/
u8 UserApp1_SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
  u8 status,uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;// Set CSN low, init SPI tranaction
  status = UserApp1_SPI_RW(reg);       		// Select register to write to and read status uchar

  for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
    pBuf[uchar_ctr] = UserApp1_SPI_RW(0);    // 

  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;                          

  return(status);                    // return nRF24L01 status uchar
}

/*********************************************************************************************************/
/*函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)*/
/*功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数*/
/*********************************************************************************************************/
u8 UserApp1_SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
  u8 status,uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;//SPI使能       
  status = UserApp1_SPI_RW(reg);   
  for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
    UserApp1_SPI_RW(*pBuf++);
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;//关闭SPI
  return(status);    // 
}

/***********************************************************************************************************/
/*函数：void nRF24L01_TxPacket(unsigned char * tx_buf)*/
/*功能：发送 tx_buf中数据*/
/**********************************************************************************************************/
void UserApp1_nRF24L01_TxPacket(unsigned char * tx_buf)
{
  AT91C_BASE_PIOA->PIO_CODR = PA_08_C1_CE;//StandBy I模式
  UserApp1_SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // 装载数据	
  AT91C_BASE_PIOA->PIO_SODR = PA_08_C1_CE;//置高CE，激发数据发送
  inerDelay_us(10);
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u8 au8ADCData[41] = {0};
  static u8 u8Counter=0;
  static u16 u16Timer=0;
  static bool bSend=FALSE;
  static u8 sta=0;
  
  if(WasButtonPressed(BUTTON0))
  {
    AT91C_BASE_PIOB->PIO_CODR = PB_06_C2_MOSI;
    LedOff(BLUE);
    ButtonAcknowledge(BUTTON0);
  }
  
  if(WasButtonPressed(BUTTON3))
  {
    bSend=TRUE;
    LedOn(YELLOW);
    ButtonAcknowledge(BUTTON3);
  }
  
  if(bSend)
  {
    u16Timer++;
    Adc12StartConversion(ADC12_CH2);

    if(G_bUserApp1ADCFlag)
    {
      G_bUserApp1ADCFlag=FALSE;
      au8ADCData[u8Counter]= (u8)(G_u16UserApp1ADCResult>>4)+1-(u16Timer/3000);
      
      /*if(au8ADCData[u8Counter]<0x88&&au8ADCData[u8Counter]>0x80)
      {
        au8ADCData[u8Counter]=0x84;
      }*/
      
      u8Counter++;
      
      if(u8Counter==40)
      {
        //SPI_Write_Buf(au8ADCData,20);
        DebugPrintf(au8ADCData);
        u8Counter=0;
      }
    }
    
    if(u16Timer==4000)
    {
      //UserApp1_nRF24L01_TxPacket(au8ADCData);
    }
    
    if(u16Timer==40000)
    {
      bSend=FALSE;
      sta=UserApp1_SPI_Read(STATUS);
      //DebugPrintNumber(sta);
      //DebugPrintNumber(UserApp1_SPI_Read(FIFO_STATUS));
      UserApp1_SPI_RW_Reg(WRITE_REG + STATUS, sta);
      UserApp1_SPI_Write_Buf(WR_TX_PLOAD, au8ADCData, TX_PLOAD_WIDTH);
      AT91C_BASE_PIOB->PIO_SODR = PB_06_C2_MOSI;
      u16Timer=0;
      LedOff(YELLOW);
      LedOn(BLUE);
    }
  }

} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
