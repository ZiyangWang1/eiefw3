/***********************************************************************************************************************
File: spimaster.c                                                                

Description:
***********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_xxLed"
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern volatile u32 G_u32ApplicationFlags;             /* From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Spi_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type SpiMaster_pfnStateMachine;              /* The application state machine function pointer */
u8 SpiMaster_u8CurrentByte = 0;

static u8* SpiMaster_pu8RxBuffer = NULL;
static u8** SpiMaster_ppu8RxNextChar = NULL;
static u8 SpiMaster_u8RxLength = 0;

static u8 SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE];
static u8* SpiMaster_pu8TxNextChar = SpiMaster_TxBuffer;
static u8* SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;

/***********************************************************************************************************************
* Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterOpen

Description:
Request a spi master peripherals

Requires: 
  - A pointer to a spi_master_config_t type

Promises:
*/
bool SpiMasterOpen(spi_master_config_t * p_spi_master_config)
{
  /* Check against null */
  if (p_spi_master_config == NULL)
  {
      return false;
  }
  
  //A Slave select must be set as high before setting it as output,
  //because during connect it to the pin it causes glitches.
  nrf_gpio_pin_set(p_spi_master_config->SPI_Pin_SS);
  nrf_gpio_cfg_output(p_spi_master_config->SPI_Pin_SS);
  nrf_gpio_pin_set(p_spi_master_config->SPI_Pin_SS);
  
  //Configure GPIO
  nrf_gpio_cfg_output(p_spi_master_config->SPI_Pin_SCK);
  nrf_gpio_cfg_output(p_spi_master_config->SPI_Pin_MOSI);
  nrf_gpio_cfg_input(p_spi_master_config->SPI_Pin_MISO, NRF_GPIO_PIN_NOPULL);
  
  switch(p_spi_master_config->SPI_CONFIG_CPOL)
  {
  case SPI_CONFIG_CPOL_ActiveHigh: NRF_GPIO->OUTCLR = P0_11_SPI_SCK;break;
  case SPI_CONFIG_CPOL_ActiveLow: NRF_GPIO->OUTSET = P0_11_SPI_SCK;break;
  default: return false;
  }
  
  NRF_GPIO->OUTCLR = P0_13_SPI_MOSI;
  /* Configure SPI hardware */
  NRF_SPI0->PSELSCK  = p_spi_master_config->SPI_Pin_SCK;
  NRF_SPI0->PSELMOSI = p_spi_master_config->SPI_Pin_MOSI;
  NRF_SPI0->PSELMISO = p_spi_master_config->SPI_Pin_MISO;

  NRF_SPI0->FREQUENCY = p_spi_master_config->SPI_Freq;

  NRF_SPI0->CONFIG =
      (u32)(p_spi_master_config->SPI_CONFIG_CPHA << SPI_CONFIG_CPHA_Pos) |
      (p_spi_master_config->SPI_CONFIG_CPOL << SPI_CONFIG_CPOL_Pos) |
      (p_spi_master_config->SPI_CONFIG_ORDER << SPI_CONFIG_ORDER_Pos);
  
  /* Clear waiting interrupts and events */
  NRF_SPI0->EVENTS_READY = 0;
  
  /* Enable SPI hardware */
  NRF_SPI0->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
  
  SpiMaster_pu8RxBuffer = p_spi_master_config -> p_rx_buffer;
  SpiMaster_ppu8RxNextChar = p_spi_master_config ->pp_rx_nextchar;
  SpiMaster_u8RxLength = p_spi_master_config -> rx_length;
  return true;
}

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterSendByte

Description:
Send a single byte to the selected slave

Requires: 
  - A pointer to the byte

Promises:
*/
void SpiMasterSendByte(u8 * p_tx_buf)
{
    *SpiMaster_pu8UnsendChar = *p_tx_buf;
    SpiMaster_pu8UnsendChar++;
    
    if(SpiMaster_pu8UnsendChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
    {
      SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;
    }
}

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterSendData

Description:
Send an data array to the selected slave

Requires: 
  - A pointer to the array
  - The length of the array

Promises:
*/
bool SpiMasterSendData(u8 * p_tx_buf, u8 u8length)
{
  if(u8length > SPI_TX_BUFFER_SIZE)
  {
    return false;
  }
  
  for(int i = 0;i<u8length;i++)
  {
    *SpiMaster_pu8UnsendChar = p_tx_buf[i];
    SpiMaster_pu8UnsendChar++;
    
    if(SpiMaster_pu8UnsendChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
    {
      SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;
    }
  }
  return true;
}

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterReadByte

Description:
Read a single byte from the selected slave

Requires: 

Promises:
  - 
*/
bool SpiMasterReadByte(void)
{
  return false;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterInitialize

Description:
Initializes the State Machine and its variables.

Requires:

Promises:
*/
void SpiMasterInitialize(void)
{
  for(int i = 0;i<SPI_TX_BUFFER_SIZE;i++)
  {
    SpiMaster_TxBuffer[i] = 0;
  }
  
  if(1)
  {
    SpiMaster_pfnStateMachine = SpiMasterSM_Idle;
  }
  else
  {
    SpiMaster_pfnStateMachine = SpiMasterSM_Error;
  }
} /* end SpiMasterInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function SpiMasterRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void SpiMasterRunActiveState(void)
{
  SpiMaster_pfnStateMachine();

} /* end SpiMasterRunActiveState */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/

  
/*--------------------------------------------------------------------------------------------------------------------*/
/* State Machine definitions                                                                                          */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
State: SpiMasterSM_Idle
*/
static void SpiMasterSM_Idle(void)
{
  if(NRF_SPI0->EVENTS_READY == 1)
  {
    SpiMaster_u8CurrentByte = NRF_SPI0->RXD;
    
    if(SpiMaster_u8CurrentByte != 0)
    {
      **SpiMaster_ppu8RxNextChar = SpiMaster_u8CurrentByte;
      (*SpiMaster_ppu8RxNextChar)++;
      
      if(*SpiMaster_ppu8RxNextChar == (SpiMaster_pu8RxBuffer + SpiMaster_u8RxLength))
      {
        *SpiMaster_ppu8RxNextChar = SpiMaster_pu8RxBuffer;
      }
    }
    NRF_SPI0->EVENTS_READY = 0;
  }

  if(SpiMaster_pu8TxNextChar != SpiMaster_pu8UnsendChar)
  {
    NRF_SPI0->TXD = *SpiMaster_pu8TxNextChar;
    SpiMaster_pu8TxNextChar++;
    
    if(SpiMaster_pu8TxNextChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
    {
      SpiMaster_pu8TxNextChar = SpiMaster_TxBuffer;
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------------
State: SpiMasterSM_Error
*/
static void SpiMasterSM_Error(void)
{
    
} 

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


