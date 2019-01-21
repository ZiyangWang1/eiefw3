/*!*********************************************************************************************************************
@file user_app2.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app2 as a template:
 1. Copy both user_app2.c and user_app2.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app2" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp2" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp2Initialize(void)
- void UserApp2RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp2"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */

extern volatile u8 sta;
extern  u8 TX_ADDRESS[TX_ADR_WIDTH];//本地地址
extern  u8 RX_ADDRESS[RX_ADR_WIDTH];//接收地址

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp2_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp2Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp2Initialize(void)
{
  //UserApp2_init_NRF24L01();
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp2_pfStateMachine = UserApp2SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_pfStateMachine = UserApp2SM_Error;
  }

} /* end UserApp2Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp2RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp2RunActiveState(void)
{
  UserApp2_pfStateMachine();

} /* end UserApp2RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
//****************************************************************************************
//NRF24L01初始化
//***************************************************************************************/
void UserApp2_init_NRF24L01(void)
{
  inerDelay_us(100);
  AT91C_BASE_PIOA->PIO_CODR = PA_14_C2_CE;    // chip enable
  AT91C_BASE_PIOA->PIO_SODR = PA_15_C2_CSN;   // Spi  disable 
  AT91C_BASE_PIOB->PIO_CODR = PB_05_C2_SCK;   // 
  UserApp2_SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址
  UserApp2_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
  UserApp2_SPI_RW_Reg(WRITE_REG + EN_AA, 0x00);      //  频道0自动	ACK应答允许	
  UserApp2_SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
  UserApp2_SPI_RW_Reg(WRITE_REG + RF_CH, 40);        //   设置信道工作为2.4GHZ，收发必须一致
  UserApp2_SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
  UserApp2_SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB
  UserApp2_SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);// IRQ收发完成中断响应，16位CRC	，主接收
  AT91C_BASE_PIOA->PIO_SODR = PA_14_C2_CE;    // chip enable
}

/****************************************************************************************************
函数：uint UserApp2_SPI_RW(uint uchar)
功能：NRF24L01的SPI写时序
****************************************************************************************************/
u8 UserApp2_SPI_RW(u8 uchar)
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
    if(AT91C_BASE_PIOB->PIO_PDSR & PB_08_C2_MISO)
    {
      uchar |= 0x01;
    }
    else
    {
      uchar &= 0xFE;
    }// capture current MISO bit
    AT91C_BASE_PIOB->PIO_CODR = PB_05_C2_SCK;// ..then set SCK low again
  }
  return(uchar);           		  // return read uchar
}

/****************************************************************************************************
函数：uchar SPI_Read(uchar reg)
功能：NRF24L01的SPI时序
****************************************************************************************************/
u8 UserApp2_SPI_Read(u8 reg)
{
  u8 reg_val;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_15_C2_CSN;// CSN low, initialize SPI communication...
  UserApp2_SPI_RW(reg);            // Select register to read from..
  reg_val = UserApp2_SPI_RW(0);    // ..then read registervalue
  AT91C_BASE_PIOA->PIO_SODR = PA_15_C2_CSN;// CSN high, terminate SPI communication
  
  return(reg_val);        // return register value
}

/****************************************************************************************************/
/*功能：NRF24L01读写寄存器函数*/
/****************************************************************************************************/
u8 UserApp2_SPI_RW_Reg(u8 reg, u8 value)
{
  u8 status;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_15_C2_CSN;// CSN low, init SPI transaction
  status = UserApp2_SPI_RW(reg);      // select register
  UserApp2_SPI_RW(value);             // ..and write value to it..
  AT91C_BASE_PIOA->PIO_SODR = PA_15_C2_CSN;// CSN high again
  
  return(status);            // return nRF24L01 status uchar
}

/****************************************************************************************************/
/*函数：uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)*/
/*功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数*/
/****************************************************************************************************/
u8 UserApp2_SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
  u8 status,uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_15_C2_CSN;// Set CSN low, init SPI tranaction
  status = UserApp2_SPI_RW(reg);       		// Select register to write to and read status uchar

  for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
    pBuf[uchar_ctr] = UserApp2_SPI_RW(0);    // 

  AT91C_BASE_PIOA->PIO_SODR = PA_15_C2_CSN;                          

  return(status);                    // return nRF24L01 status uchar
}

/*********************************************************************************************************/
/*函数：uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)*/
/*功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数*/
/*********************************************************************************************************/
u8 UserApp2_SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
  u8 status,uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_15_C2_CSN;//SPI使能       
  status = UserApp2_SPI_RW(reg);   
  for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
    UserApp2_SPI_RW(*pBuf++);
  AT91C_BASE_PIOA->PIO_SODR = PA_15_C2_CSN;//关闭SPI
  return(status);    // 
}

/******************************************************************************************************/
/*函数：unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)*/
/*功能：数据读取后放如rx_buf接收缓冲区中*/
/******************************************************************************************************/
unsigned char UserApp2_nRF24L01_RxPacket(unsigned char* rx_buf)
{
  unsigned char revale=0;
  sta=UserApp2_SPI_Read(STATUS);	// 读取状态寄存其来判断数据接收状况
  if(RX_DR)				// 判断是否接收到数据
  {
    AT91C_BASE_PIOA->PIO_CODR = PA_14_C2_CE;//SPI使能
    UserApp2_SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
    revale =1;			//读取数据完成标志
    UserApp2_SPI_RW(FLUSH_RX);
  }
  UserApp2_SPI_RW_Reg(WRITE_REG+STATUS,0xFF);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
  return revale;
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp2SM_Idle(void)
{
  /*static u8 au8ADCData[21] = {0};
  static u8 u8LastDataNum=0;
  static u16 u16Timer=0;
  static bool bSend=FALSE;
  
  if(WasButtonPressed(BUTTON1))
  {
    bSend=TRUE;
    ButtonAcknowledge(BUTTON1);
  }
  
  if(bSend)
  {
    u16Timer++;
    
    if(u16Timer==3000)
    {
      DebugPrintNumber(UserApp2_SPI_Read(STATUS));
      DebugPrintNumber(UserApp2_SPI_Read(FIFO_STATUS));
    }
    
    if(u16Timer==6000)
    {
      UserApp2_nRF24L01_RxPacket(au8ADCData);
      if(u8LastDataNum!=au8ADCData[0])
      {
        u8LastDataNum=au8ADCData[0];
        DebugPrintf(au8ADCData);
      }
    }
    
    if(u16Timer==9000)
    {
      DebugPrintNumber(UserApp2_SPI_Read(STATUS));
      DebugPrintNumber(UserApp2_SPI_Read(FIFO_STATUS));
      u16Timer=0;
    }
    
    
  }
  
  

   */ 
} /* end UserApp2SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{
  
} /* end UserApp2SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
