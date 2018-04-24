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
static SspPeripheralType* UserApp_SPI;
static u8 UserApp1_au8RxBuffer[USERAPP_RX_BUFFER_SIZE];
static u8* UserApp1_pu8RxBufferNextChar = &UserApp1_au8RxBuffer[0];
static u8* UserApp1_pu8RxBufferParser = &UserApp1_au8RxBuffer [0];
static u8 UserApp1_au8StartupMsg[] = "\n\n\r*** UserApp SPI Ready ***\n\r";

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
  SspConfigurationType UserAppSspConfig;
  
  UserAppSspConfig.SspPeripheral = USART2;
  UserAppSspConfig.pCsGpioAddress = AT91C_BASE_PIOB;
  UserAppSspConfig.u32CsPin = AT91C_PIO_PB22;
  UserAppSspConfig.eBitOrder = LSB_FIRST;
  UserAppSspConfig.fnSlaveTxFlowCallback = SlaveTxFlowCallback;
  UserAppSspConfig.fnSlaveRxFlowCallback = SlaveRxFlowCallback;
  UserAppSspConfig.pu8RxBufferAddress = &UserApp1_au8RxBuffer[0];
  UserAppSspConfig.ppu8RxNextByte = &UserApp1_pu8RxBufferNextChar;
  UserAppSspConfig.u16RxBufferSize = USERAPP_RX_BUFFER_SIZE;
 
  UserApp_SPI = SspRequest(&UserAppSspConfig);
  
  /* If good initialization, set state to Idle */
  if( UserApp_SPI != NULL )
  {
    DebugPrintf(UserApp1_au8StartupMsg);
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
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


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  u8 au8CurrentByte[] = " ";
  
  /* Parse any new characters that have come in until no more chars */
  while( (UserApp1_pu8RxBufferParser != UserApp1_pu8RxBufferNextChar))
  {
    /* Grab a copy of the current byte and echo it back */
    au8CurrentByte[0] = *UserApp1_pu8RxBufferParser;
    DebugPrintf(au8CurrentByte);
    UserApp1_pu8RxBufferParser++;
    if(UserApp1_pu8RxBufferParser >= &UserApp1_au8RxBuffer[USERAPP_RX_BUFFER_SIZE])
    {
      UserApp1_pu8RxBufferParser = &UserApp1_au8RxBuffer[0];
    }
    LedToggle(BLUE);
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
