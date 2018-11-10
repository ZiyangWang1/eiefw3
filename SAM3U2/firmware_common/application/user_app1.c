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
  AT91C_BASE_PIOA->PIO_PER = PA_09_C1_GND;
  AT91C_BASE_PIOA->PIO_PER = PA_10_C1_VCC;
  AT91C_BASE_PIOA->PIO_PER = PA_11_C2_GND;
  AT91C_BASE_PIOA->PIO_PER = PA_12_C2_VCC;
  AT91C_BASE_PIOA->PIO_PER = PA_14_C2_CE;
  AT91C_BASE_PIOA->PIO_PER = PA_15_C2_CSN;
  AT91C_BASE_PIOB->PIO_PER = PB_05_C2_SCK;
  AT91C_BASE_PIOB->PIO_PER = PB_06_C2_MOSI;
  AT91C_BASE_PIOB->PIO_PER = PB_07_C2_IRQ;
  AT91C_BASE_PIOB->PIO_PER = PB_08_C2_MISO;
  
  AT91C_BASE_PIOA->PIO_OER = PA_04_C1_CSN;
  AT91C_BASE_PIOA->PIO_OER = PA_06_C1_SCK;
  AT91C_BASE_PIOA->PIO_OER = PA_07_C1_MOSI;
  AT91C_BASE_PIOA->PIO_OER = PA_08_C1_CE;
  AT91C_BASE_PIOA->PIO_OER = PA_09_C1_GND;
  AT91C_BASE_PIOA->PIO_OER = PA_10_C1_VCC;
  AT91C_BASE_PIOA->PIO_OER = PA_11_C2_GND;
  AT91C_BASE_PIOA->PIO_OER = PA_12_C2_VCC;
  AT91C_BASE_PIOA->PIO_OER = PA_14_C2_CE;
  AT91C_BASE_PIOA->PIO_OER = PA_15_C2_CSN;
  AT91C_BASE_PIOB->PIO_OER = PB_05_C2_SCK;
  AT91C_BASE_PIOB->PIO_OER = PB_06_C2_MOSI;
  
  AT91C_BASE_PIOB->PIO_PPUER = PA_10_C1_VCC;
  AT91C_BASE_PIOB->PIO_PPUER = PA_12_C2_VCC;
  
  /* Initialize the GPIO ports */
  
  AT91C_BASE_PIOA->PIO_CODR = PA_09_C1_GND;
  AT91C_BASE_PIOA->PIO_SODR = PA_10_C1_VCC;
  AT91C_BASE_PIOA->PIO_CODR = PA_11_C2_GND;
  AT91C_BASE_PIOA->PIO_SODR = PA_12_C2_VCC;
  
  
  Adc12AssignCallback(ADC12_CH2, UserApp_AdcCallback);
  LCDCommand(LCD_CLEAR_CMD);
  
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
��ʱ����
******************************************************************************************/
void inerDelay_us(unsigned char n)
{
  for(;n>0;n--);
}

//****************************************************************************************
//NRF24L01��ʼ��
//***************************************************************************************/
void init_NRF24L01(void)
{
  inerDelay_us(100);
  AT91C_BASE_PIOA->PIO_CODR = PA_08_C1_CE;    // chip enable
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;   // Spi  disable 
  AT91C_BASE_PIOA->PIO_CODR = PA_06_C1_SCK;   // 
  SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
  SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  Ƶ��0�Զ�	ACKӦ������	
  SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
  SPI_RW_Reg(WRITE_REG + RF_CH, 0);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
  SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
  SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB
}

/****************************************************************************************************
������uint SPI_RW(uint uchar)
���ܣ�NRF24L01��SPIдʱ��
****************************************************************************************************/
u8 SPI_RW(u8 uchar)
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
    uchar |= AT91C_BASE_PIOA->PIO_PDSR & PA_03_C1_MISO;// capture current MISO bit
    AT91C_BASE_PIOA->PIO_CODR = PA_06_C1_SCK;// ..then set SCK low again
  }
  return(uchar);           		  // return read uchar
}

/****************************************************************************************************
������uchar SPI_Read(uchar reg)
���ܣ�NRF24L01��SPIʱ��
****************************************************************************************************/
u8 SPI_Read(u8 reg)
{
  u8 reg_val;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;// CSN low, initialize SPI communication...
  SPI_RW(reg);            // Select register to read from..
  reg_val = SPI_RW(0);    // ..then read registervalue
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;// CSN high, terminate SPI communication
  
  return(reg_val);        // return register value
}

/****************************************************************************************************/
/*���ܣ�NRF24L01��д�Ĵ�������*/
/****************************************************************************************************/
u8 SPI_RW_Reg(u8 reg, u8 value)
{
  u8 status;
  
  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;// CSN low, init SPI transaction
  status = SPI_RW(reg);      // select register
  SPI_RW(value);             // ..and write value to it..
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;// CSN high again
  
  return(status);            // return nRF24L01 status uchar
}

/****************************************************************************************************/
/*������uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)*/
/*����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���*/
/****************************************************************************************************/
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
  u8 status,uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;// Set CSN low, init SPI tranaction
  status = SPI_RW(reg);       		// Select register to write to and read status uchar

  for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
    pBuf[uchar_ctr] = SPI_RW(0);    // 

  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;                          

  return(status);                    // return nRF24L01 status uchar
}

/*********************************************************************************************************/
/*������uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)*/
/*����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���*/
/*********************************************************************************************************/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
  u8 status,uchar_ctr;

  AT91C_BASE_PIOA->PIO_CODR = PA_04_C1_CSN;//SPIʹ��       
  status = SPI_RW(reg);   
  for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
    SPI_RW(*pBuf++);
  AT91C_BASE_PIOA->PIO_SODR = PA_04_C1_CSN;//�ر�SPI
  return(status);    // 
}

/****************************************************************************************************/
/*������void SetRX_Mode(void)*/
/*���ܣ����ݽ������� */
/****************************************************************************************************/
void SetRX_Mode(void)
{
  AT91C_BASE_PIOA->PIO_CODR = PA_08_C1_CE;
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);// IRQ�շ�����ж���Ӧ��16λCRC	��������
  AT91C_BASE_PIOA->PIO_SODR = PA_08_C1_CE;
  inerDelay_us(130);
}

/******************************************************************************************************/
/*������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)*/
/*���ܣ����ݶ�ȡ�����rx_buf���ջ�������*/
/******************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
  unsigned char revale=0;
  sta=SPI_Read(STATUS);	// ��ȡ״̬�Ĵ������ж����ݽ���״��
  if(RX_DR)				// �ж��Ƿ���յ�����
  {
    AT91C_BASE_PIOA->PIO_CODR = PA_08_C1_CE;//SPIʹ��
    SPI_Read_Buf(RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
    revale =1;			//��ȡ������ɱ�־
  }
  SPI_RW_Reg(WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
  return revale;
}

/***********************************************************************************************************/
/*������void nRF24L01_TxPacket(unsigned char * tx_buf)*/
/*���ܣ����� tx_buf������*/
/**********************************************************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
  AT91C_BASE_PIOA->PIO_CODR = PA_08_C1_CE;//StandBy Iģʽ	
  SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
  SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // װ������	
  SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e); // IRQ�շ�����ж���Ӧ��16λCRC��������
  AT91C_BASE_PIOA->PIO_SODR = PA_08_C1_CE;//�ø�CE���������ݷ���
  inerDelay_us(10);
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u8 au8ADCData[21] = {0};
  static u8 u8Counter=0;
  static u16 u16Timer=0;
  static bool bSend=FALSE;
  
  if(IsButtonPressed(BUTTON0))
  {
    bSend=TRUE;
  }
  
  if(bSend)
  {
    u16Timer++;
    Adc12StartConversion(ADC12_CH2);

    if(G_bUserApp1ADCFlag)
    {
      G_bUserApp1ADCFlag=FALSE;
      au8ADCData[u8Counter]= (u8)(G_u16UserApp1ADCResult>>4)+1;
      u8Counter++;
      
      if(u8Counter==20)
      {
        DebugPrintf(au8ADCData);
        u8Counter=0;
      }
    }
    
    if(u16Timer==40000)
    {
      bSend=FALSE;
      u16Timer=0;
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
