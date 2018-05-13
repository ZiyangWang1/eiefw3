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
static fnCode_type Anttt_pfnStateMachine;                       /* The application state machine function pointer */
static u8 Anttt_au8RxBuffer[ANTTT_RX_BUFFER_SIZE];              /* The receiving buffer */
static u8 * Anttt_pu8RxNextChar = Anttt_au8RxBuffer;            /* The receiving next char pointer */
u8 * Anttt_pu8RxParser = Anttt_au8RxBuffer;              /* The receiving parser pointer */
static u8 ** Anttt_ppu8RxNextCharPos = &Anttt_pu8RxNextChar;    /* A pointer to the receiving next char pointer */

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

  // Configure GPIOTE and enable the interrupt
  nrf_gpiote_event_config(0,P0_09_INDEX,NRF_GPIOTE_POLARITY_TOGGLE);
  sd_nvic_SetPriority(GPIOTE_IRQn, NRF_APP_PRIORITY_LOW);
  sd_nvic_EnableIRQ(GPIOTE_IRQn);
  NRF_GPIOTE->INTENSET = (u32)1 << 0;
  
  // Clear receiving buffer
  for(int i = 0;i<ANTTT_RX_BUFFER_SIZE;i++)
  {
    Anttt_au8RxBuffer[i] = 0;
  }
  
  // Configure SPI Master
  antttSpiMaster.SPI_Freq = SPI_FREQUENCY_FREQUENCY_M1;
  antttSpiMaster.SPI_Pin_SCK = P0_11_INDEX;
  antttSpiMaster.SPI_Pin_MISO = P0_12_INDEX;
  antttSpiMaster.SPI_Pin_MOSI = P0_13_INDEX;
  antttSpiMaster.SPI_Pin_SS = P0_10_INDEX;
  antttSpiMaster.SPI_CONFIG_ORDER = SPI_CONFIG_ORDER_LsbFirst;
  antttSpiMaster.SPI_CONFIG_CPOL = SPI_CONFIG_CPOL_ActiveLow;
  antttSpiMaster.SPI_CONFIG_CPHA = SPI_CONFIG_CPHA_Trailing;
  antttSpiMaster.p_rx_buffer = Anttt_au8RxBuffer;
  antttSpiMaster.pp_rx_nextchar = Anttt_ppu8RxNextCharPos;
  antttSpiMaster.rx_length = ANTTT_RX_BUFFER_SIZE;
  
  // If configure successfully, set the state machine to idle
  if(SpiMasterOpen(&antttSpiMaster))
  {
    SspDeAssertCS();
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
  // Check receiving buffer for new bytes
  if(Anttt_pu8RxParser != Anttt_pu8RxNextChar)
  {
    Anttt_pu8RxParser++;
    
    if(Anttt_pu8RxParser == &Anttt_au8RxBuffer[ANTTT_RX_BUFFER_SIZE])
    {
      Anttt_pu8RxParser = Anttt_au8RxBuffer;
    }
    
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
