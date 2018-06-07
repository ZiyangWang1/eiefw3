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

static UartPeripheralType* UserApp_Uart;
static u8 UserApp_au8RxBuffer[USERAPP_RX_BUFFER_SIZE];
static u8 *UserApp_pu8RxBufferNextChar;  
static u8 *UserApp_pu8RxBufferParser;
static u8 UserApp_au8StartupMsg[] = "\n\n\r*** UserApp UART Ready ***\n\r";

static SspPeripheralType* UserApp_SPI;
static u8 UserApp1_au8RxBuffer[USERAPP_RX_BUFFER_SIZE];
static u8* UserApp1_pu8RxBufferNextChar = &UserApp1_au8RxBuffer[0];
static u8* UserApp1_pu8RxBufferParser = &UserApp1_au8RxBuffer [0];
static u8 UserApp1_au8StartupMsg[] = "\n\n\r*** UserApp SPI Ready ***\n\r";

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
  UartConfigurationType sUartConfig;
  SspConfigurationType UserAppSspConfig;
  
  LCDCommand(LCD_CLEAR_CMD);
  for (u16 i = 0; i < USERAPP_RX_BUFFER_SIZE; i++)
  {
    UserApp_au8RxBuffer[i] = 0;
  }
  
  UserApp_pu8RxBufferParser    = &UserApp_au8RxBuffer[0];
  UserApp_pu8RxBufferNextChar  = &UserApp_au8RxBuffer[0]; 
  
  sUartConfig.UartPeripheral     = UART;
  sUartConfig.pu8RxBufferAddress = &UserApp_au8RxBuffer[0];
  sUartConfig.pu8RxNextByte      = &UserApp_pu8RxBufferNextChar;
  sUartConfig.u16RxBufferSize    = USERAPP_RX_BUFFER_SIZE;
  sUartConfig.fnRxCallback       = UserAppRxCallback;
  
  UserApp_Uart = UartRequest(&sUartConfig);
  
  UserAppSspConfig.SspPeripheral = USART2;
  UserAppSspConfig.pCsGpioAddress = AT91C_BASE_PIOB;
  UserAppSspConfig.u32CsPin = AT91C_PIO_PB22;
  UserAppSspConfig.eSspMode = SPI_SLAVE_FLOW_CONTROL;
  UserAppSspConfig.eBitOrder = LSB_FIRST;
  UserAppSspConfig.fnSlaveTxFlowCallback = SlaveTxFlowCallback;
  UserAppSspConfig.fnSlaveRxFlowCallback = SlaveRxFlowCallback;
  UserAppSspConfig.pu8RxBufferAddress = &UserApp1_au8RxBuffer[0];
  UserAppSspConfig.ppu8RxNextByte = &UserApp1_pu8RxBufferNextChar;
  UserAppSspConfig.u16RxBufferSize = USERAPP_RX_BUFFER_SIZE;
 
  UserApp_SPI = SspRequest(&UserAppSspConfig);
  
 
  /* If good initialization, set state to Idle */
  if( UserApp_Uart != NULL && UserApp_SPI != NULL)
  {
    DebugPrintf(UserApp_au8StartupMsg);
    DebugPrintf(UserApp1_au8StartupMsg);
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

/*--------------------------------------------------------------------------------------------------------------------
Function: SlaveTxFlowCallback

Description:


Requires:
  -

Promises:
  - 
*/
void SlaveTxFlowCallback(void)
{
  
}

/*--------------------------------------------------------------------------------------------------------------------
Function: SlaveRxFlowCallback

Description:
Call back function used when character received.

Requires:
  - None

Promises:
  - Safely advances UserApp1_pu8RxBufferNextChar.
*/
void SlaveRxFlowCallback(void)
{
  /* Safely advances the NextChar pointer */
  UserApp1_pu8RxBufferNextChar++;
  if(UserApp1_pu8RxBufferNextChar == &UserApp1_au8RxBuffer[USERAPP_RX_BUFFER_SIZE])
  {
    UserApp1_pu8RxBufferNextChar = &UserApp1_au8RxBuffer[0];
  }
  
}


/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppRxCallback()

Description:
Call back function used when character received.

Requires:
  - None

Promises:
  - Safely advances UserApp_pu8RxBufferNextChar.
*/
void UserAppRxCallback(void)
{
  /* Safely advance the NextChar pointer */
  UserApp_pu8RxBufferNextChar++;
  if(UserApp_pu8RxBufferNextChar == &UserApp_au8RxBuffer[USERAPP_RX_BUFFER_SIZE])
  {
    UserApp_pu8RxBufferNextChar = &UserApp_au8RxBuffer[0];
  }
  
} /* end DebugRxCallback() */

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

/*--------------------------------------------------------------------------------------------------------------------
Function: HexToChar

Description:
Change a 8-bit-number to Hex-ascii formation

Requires:
  - A 8-bit-number input
  - A pointer to store location

Promises:
  - The store location needs at least 2 bytes
*/
void HexToChar(u8 u8HexInput_,u8* pu8Buffer)
{
  if(u8HexInput_ / 16 < 10)
  {
    *pu8Buffer = u8HexInput_ / 16 + '0';
  }
  else
  {
    *pu8Buffer = u8HexInput_ / 16 - 10 + 'A';
  }
  
  if(u8HexInput_ % 16 < 10)
  {
    *(pu8Buffer+1) = u8HexInput_ % 16 + '0';
  }
  else
  {
    *(pu8Buffer+1) = u8HexInput_ % 16 - 10 + 'A';
  }
  return;
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  u8 au8CurrentByte[] = " ";
  static u8 au8SenserData[11];
  static u8 u8ByteCounter = 0;
  static u8 u8PackageCounter = 0;
  static bool bChecked = TRUE;
  static s32 s32LastPitch = 0;
  static u16 u16Steps = 0;
  u8 au8LcdMessage1[] = " AACC:    .  m/s^2";
  u8 au8LcdMessage2[] = "Steps:    ";
  s32 s32Pitch = 0;
  static u8 au8StepsData[] = {0,0};
  static u8 u8DelayCounter = 0;
  static u16 u16SendCounter = 0;
  static bool bSendSRDY = FALSE;
  
  u16SendCounter++;
  
  if(u16SendCounter == 1000)
  {
    u16SendCounter = 0;
    SspWriteData(UserApp_SPI,2,au8StepsData);
    bSendSRDY = TRUE;
  }
  
  if(bSendSRDY)
  {
    u8DelayCounter++;
    if(u8DelayCounter == 5)
    {
      if(AT91C_BASE_PIOB->PIO_ODSR & PB_24_ANT_SRDY)
      {
       AT91C_BASE_PIOB->PIO_CODR = PB_24_ANT_SRDY;
      }
      else
      {
       AT91C_BASE_PIOB->PIO_SODR = PB_24_ANT_SRDY;
      }
      u8DelayCounter = 0;
      bSendSRDY = FALSE;
    }

  }
  
  /* Parse any new characters that have come in until no more chars */
  while( (UserApp_pu8RxBufferParser != UserApp_pu8RxBufferNextChar) )
  {
    /* Grab a copy of the current byte and echo it back */
    au8CurrentByte[0] = *UserApp_pu8RxBufferParser;
    
    if(au8CurrentByte[0] == 0x52)
    {
      u8PackageCounter++;
    }
    
    if(u8PackageCounter == 20)
    {
      u8PackageCounter = 0;
      bChecked = FALSE;
    }
    
    if(!bChecked)
    {
      au8SenserData[u8ByteCounter] = au8CurrentByte[0];
      u8ByteCounter++;
      if(u8ByteCounter == 11)
      {
        u8ByteCounter = 0;
        bChecked = TRUE;
        s32Pitch = ((s32)((s16)(au8SenserData[4]<<8) | au8SenserData[3])) * 1800 / 32768;
        if((s32LastPitch > 10 && s32Pitch < -10 ) )//|| (s32LastPitch < -10 && s32Pitch > 10))
        {
          u16Steps++;
        }
        s32LastPitch = s32Pitch;
        if(s32Pitch < 0)
        {
          au8LcdMessage1[6] = '-';
          s32Pitch *= -1;
        }
        else
        {
          au8LcdMessage1[6] = '+';
        }
        au8LcdMessage1[7] = s32Pitch / 1000 + '0';
        au8LcdMessage1[8] = s32Pitch % 1000 / 100 + '0';
        au8LcdMessage1[9] = s32Pitch % 100 / 10 + '0';
        au8LcdMessage1[11] = s32Pitch % 10 + '0';
        au8LcdMessage2[6] = u16Steps / 1000 + '0';
        au8LcdMessage2[7] = u16Steps % 1000 / 100 + '0';
        au8LcdMessage2[8] = u16Steps % 100 / 10 + '0';
        au8LcdMessage2[9] = u16Steps % 10 + '0';
        au8StepsData[0] = u16Steps >> 8;
        au8StepsData[1] = u16Steps;
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR,au8LcdMessage1);
        LCDMessage(LINE2_START_ADDR,au8LcdMessage2);
      }
    }
    
    UserApp_pu8RxBufferParser++;
    if(UserApp_pu8RxBufferParser >= &UserApp_au8RxBuffer[USERAPP_RX_BUFFER_SIZE])
    {
      UserApp_pu8RxBufferParser = &UserApp_au8RxBuffer[0];
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
