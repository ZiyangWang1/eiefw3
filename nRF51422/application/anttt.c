/**********************************************************************************************************************
File: anttt.c                                                                

Description:
Implements TIC-TAC-TOE using data input from ANT or BLE.



**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_xxAnttt"
***********************************************************************************************************************/
/* New variables */
u32 G_u32AntttFlags;                                     /* Global state flags */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */
extern volatile u32 G_u32BPEngenuicsFlags;             /* From bleperipheral_engenuics.c  */
extern volatile u8 SpiMaster_u8RxBuffer;

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Anttt_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Anttt_pfnStateMachine;              /* The application state machine function pointer */



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
Function: AntttInitialize

Description:
Initializes the State Machine and its variables.

Requires:

Promises:
*/
void AntttInitialize(void)
{
  spi_master_config_t antttSpiMaster;

  antttSpiMaster.SPI_Freq = SPI_FREQUENCY_FREQUENCY_M1;
  antttSpiMaster.SPI_Pin_SCK = P0_11_INDEX;
  antttSpiMaster.SPI_Pin_MISO = P0_12_INDEX;
  antttSpiMaster.SPI_Pin_MOSI = P0_13_INDEX;
  antttSpiMaster.SPI_Pin_SS = P0_10_INDEX;
  antttSpiMaster.SPI_CONFIG_ORDER = SPI_CONFIG_ORDER_LsbFirst;
  antttSpiMaster.SPI_CONFIG_CPOL = SPI_CONFIG_CPOL_ActiveLow;
  antttSpiMaster.SPI_CONFIG_CPHA = SPI_CONFIG_CPHA_Trailing;
  antttSpiMaster.p_rx_buffer = NULL;
  antttSpiMaster.rx_length = 0;
  
  if(SpiMasterOpen(&antttSpiMaster))
  {
    NRF_GPIO->OUTCLR = P0_10_SPI_CS;
    Anttt_pfnStateMachine = AntttSM_Idle;
  }
  else
  {
    LedOn(RED);
    Anttt_pfnStateMachine = AntttSM_Error;
  }
} /* end AntttInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function AntttRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void AntttRunActiveState(void)
{
  Anttt_pfnStateMachine();

} /* end AntttRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/* State Machine definitions                                                                                          */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
State: AntttSM_Idle
*/
static void AntttSM_Idle(void)
{
  static u8 u8Status = 0x30;
  
  SpiMasterSendByte(&u8Status);
  nrf_delay_us(50000);
  switch(SpiMaster_u8RxBuffer)
  {
  case 0x51: LedToggle(BLUE);u8Status = 0x31;break;
  case 0x52: LedToggle(GREEN);u8Status = 0x32;break;
  case 0x53: LedToggle(YELLOW);u8Status = 0x33;break;
  case 0x54: LedToggle(RED);u8Status = 0x34;break;
  default: ;
  }
  
} 

     
/*--------------------------------------------------------------------------------------------------------------------
State: AntttSM_Error
*/
static void AntttSM_Error(void)
{
    
} 


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
