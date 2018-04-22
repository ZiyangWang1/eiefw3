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
  antttSpiMaster.SPI_CONFIG_CPOL = SPI_CONFIG_CPOL_ActiveHigh;
  antttSpiMaster.SPI_CONFIG_CPHA = SPI_CONFIG_CPHA_Leading;
  antttSpiMaster.p_rx_buffer = NULL;
  antttSpiMaster.rx_length = 0;
 
  NRF_GPIO->OUTSET = P0_28_LED_YELLOW;
  
  if(SpiMasterOpen(&antttSpiMaster))
  {
    NRF_GPIO->OUTCLR = P0_10_SPI_CS;
    NRF_GPIO->OUTCLR = P0_13_SPI_MOSI;
    NRF_GPIO->OUTCLR = P0_11_SPI_SCK;
    Anttt_pfnStateMachine = AntttSM_Idle;
  }
  else
  {
    NRF_GPIO->OUTSET = P0_29_LED_RED;
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
  u8 u8TestByte = 65;
  u8 u8RxBuffer = 0;
  
  if(SpiMasterSendByte(&u8TestByte))
  {
    NRF_GPIO->OUTSET = P0_26_LED_BLUE;
    nrf_delay_us(100000);
    NRF_GPIO->OUTCLR = P0_26_LED_BLUE;
  }
  else
  {
    NRF_GPIO->OUTSET = P0_29_LED_RED;
    nrf_delay_us(100000);
    NRF_GPIO->OUTCLR = P0_29_LED_RED;
  }
  
  if(NRF_SPI0->EVENTS_READY == 1)
  {
    u8RxBuffer = NRF_SPI0->RXD;
    NRF_GPIO->OUTSET = P0_27_LED_GREEN;
    NRF_SPI0->EVENTS_READY = 0;
    nrf_delay_us(100000);
    NRF_GPIO->OUTCLR = P0_27_LED_GREEN;
  }
  nrf_delay_us(1000000);
  
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
