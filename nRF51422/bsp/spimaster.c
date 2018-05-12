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
static u8 SpiMaster_u8CurrentByte = 0;                          /* The current byte */
static bool SpiMaster_bReadTask = false;                        /* The flag of SPI master reading task */
static bool SpiMaster_bSendTask = false;                        /* The flag of SPI master sending task */
static u8 SpiMaster_u8ReadTaskNumber = 0;                       /* The number of bytes of SPI master needs to read */

static u8* SpiMaster_pu8RxBuffer = NULL;                        /* The receive buffer pointer */
static u8** SpiMaster_ppu8RxNextChar = NULL;                    /* A pointer to the receiving next char pointer */
static u8 SpiMaster_u8RxLength = 0;                             /* The receiving buffer size */

static u8 SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE];               /* The transmitting buffer */
static u8* SpiMaster_pu8TxNextChar = SpiMaster_TxBuffer;        /* The transmitting next char pointer */
static u8* SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;        /* The unsend char pointer */

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
  u32 u32Result = NRF_SUCCESS;
  
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
  
  //Configure GPIO output
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
  
  /* Configure receiving and transmitting buffer */ 
  SpiMaster_pu8RxBuffer = p_spi_master_config -> p_rx_buffer;
  SpiMaster_ppu8RxNextChar = p_spi_master_config ->pp_rx_nextchar;
  SpiMaster_u8RxLength = p_spi_master_config -> rx_length;
  
  /* Enable "READY" interrupt */
  NRF_SPI0->INTENSET = (u32)1 << 2;
  u32Result |= sd_nvic_SetPriority(SPI0_TWI0_IRQn, NRF_APP_PRIORITY_LOW);
  u32Result |= sd_nvic_EnableIRQ(SPI0_TWI0_IRQn);

  return (u32Result == NRF_SUCCESS);
} /* end SpiMasterOpen() */

/*----------------------------------------------------------------------------------------------------------------------
Function: SspAssertCS

Description:
Asserts (CLEARS) the CS line on the target SSP peripheral.  

Requires:
  - SPI master has been opened.

Promises:
  - Target's CS line is LOW
*/
void SspAssertCS(void)
{
  NRF_GPIO->OUTCLR = P0_10_SPI_CS;
  nrf_delay_us(100);
  
} /* end SspAssertCS() */

/*----------------------------------------------------------------------------------------------------------------------
Function: SspDeAssertCS

Description:
Deasserts (SETS) the CS line on the target SSP peripheral.  

Requires:
  - SPI master has been opened.

Promises:
  - Target's CS line is HIGH
*/
void SspDeAssertCS(void)
{
  NRF_GPIO->OUTSET = P0_10_SPI_CS;
  nrf_delay_us(100);
  
} /* end SspDessertCS() */

/*----------------------------------------------------------------------------------------------------------------------
Function: CheckSpiTask

Description:
Check if there is any spi tasks

Requires:
  - SPI master has been opened.

Promises:
  - If there is a task, return true and go to main handler
*/
bool CheckSpiTask(void)
{
  if(SpiMaster_bReadTask || SpiMaster_bSendTask)
  {
    SspAssertCS();
    SPI0_TWI0_IRQHandler();
    return true;
  }
  return false;
} /* end CheckSpiTask() */

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
  // If there is already a read or send task, just add this byte to transmiting buffer
  if(SpiMaster_bReadTask || SpiMaster_bSendTask)
  {
    *SpiMaster_pu8UnsendChar = *p_tx_buf;

    // Safely advance the unsend char pointer
    SpiMaster_pu8UnsendChar++;

    if(SpiMaster_pu8UnsendChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
    {
      SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;
    }
    SpiMaster_bSendTask = true;
  }
  // If there is no task, create a send task
  else
  {
    SspAssertCS();
    NRF_SPI0->TXD = *p_tx_buf;
    SpiMaster_bSendTask = true;
  }

} /* end SpiMasterSendByte() */

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterSendData

Description:
Send a data array to the selected slave

Requires: 
  - A pointer to the array
  - The length of the array

Promises:
*/
bool SpiMasterSendData(u8 * p_tx_buf, u8 u8length)
{
  
  // Return false when the data is larger than my transmitting buffer
  if(u8length > SPI_TX_BUFFER_SIZE)
  {
    return false;
  }
  
  // If there is already a read or send task, just add this data to transmiting buffer
  if(SpiMaster_bReadTask || SpiMaster_bSendTask)
  {
    for(int i = 0;i<u8length;i++)
    {
      *SpiMaster_pu8UnsendChar = p_tx_buf[i];
      
      // Safely advance the unsend char pointer
      SpiMaster_pu8UnsendChar++;
      
      if(SpiMaster_pu8UnsendChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
      {
        SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;
      }
    }
    SpiMaster_bSendTask = true;
  }
  // If there is no task, create a send task by the first byte
  // And add the rest bytes to the transmiting buffer
  else
  {
    for(int i = 1;i<u8length;i++)
    {
      *SpiMaster_pu8UnsendChar = p_tx_buf[i];
      
      // Safely advance the unsend char pointer
      SpiMaster_pu8UnsendChar++;
      
      if(SpiMaster_pu8UnsendChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
      {
        SpiMaster_pu8UnsendChar = SpiMaster_TxBuffer;
      }
    }
    SspAssertCS();
    NRF_SPI0->TXD = p_tx_buf[0];
    SpiMaster_bSendTask = true;
  }

  return true;
} /* end SpiMasterSendData() */

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
  // Return false when there is already a read task
  if(SpiMaster_bReadTask)
  {
    return false;
  }
  
  // If there is no read task, create one and set its bytes need to be read
  SpiMaster_bReadTask = true;
  SpiMaster_u8ReadTaskNumber = 1;
  
  if(!SpiMaster_bSendTask)
  {
    // If there is no sending task, send a dummy byte
    // The receiving byte will be in the receiving buffer
    SspAssertCS();
    NRF_SPI0->TXD = SPI_DEFAULT_TX_BYTE;
  }
  return true;
} /* end SpiMasterReadByte() */

/*--------------------------------------------------------------------------------------------------------------------
Function: SpiMasterReadData

Description:
Read a data array from the selected slave

Requires: 
  - The length of the array

Promises:
*/
bool SpiMasterReadData(u8 u8length)
{
  
  // Return false when the data is larger than my receiving buffer
  if(u8length > SpiMaster_u8RxLength)
  {
    return false;
  }
  
  // Return false when there is already a read task
  if(SpiMaster_bReadTask)
  {
    return false;
  }
  
  // If there is no read task, create one and set its bytes need to be read
  SpiMaster_bReadTask = true;
  SpiMaster_u8ReadTaskNumber = u8length;
  
  if(!SpiMaster_bSendTask)
  {
    // If there is no sending task, send a dummy byte
    // The receiving byte will be in the receiving buffer
    SspAssertCS();
    NRF_SPI0->TXD = SPI_DEFAULT_TX_BYTE;
  }
  return true;
} /* end SpiMasterReadData() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Interrupt handler functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Interrupt handler: SPI0_TWI0_IRQHandler

Description:
Processes SPI0 Events 

Requires:
  - Enabled via sd_nvic_XXX

Promises:
  - Handles the SPI0 events. 
*/
void SPI0_TWI0_IRQHandler(void)
{
  // When READY interrupt comes, read RXD register
  SpiMaster_u8CurrentByte = NRF_SPI0->RXD;
  
  // Clear READY interrupt and event
  NRF_SPI0->EVENTS_READY = 0;
  sd_nvic_ClearPendingIRQ(SPI0_TWI0_IRQn);
  
  // Check against dummy bytes
  if(SpiMaster_u8CurrentByte != 0x00 && SpiMaster_u8CurrentByte != 0xFF && SpiMaster_bReadTask)
  {
    // Save to the next char location
    **SpiMaster_ppu8RxNextChar = SpiMaster_u8CurrentByte;
    
    // Safely advance the receiving next char pointer
    (*SpiMaster_ppu8RxNextChar)++;
    
    if(*SpiMaster_ppu8RxNextChar == (SpiMaster_pu8RxBuffer + SpiMaster_u8RxLength))
    {
      *SpiMaster_ppu8RxNextChar = SpiMaster_pu8RxBuffer;
    }
    
    // Process the byte
    switch(SpiMaster_u8CurrentByte)
    {
    case 0x51: LedToggle(BLUE);break;
    case 0x52: LedToggle(GREEN);break;
    case 0x53: LedToggle(YELLOW);break;
    case 0x54: LedToggle(RED);break;
    default:SpiMaster_u8ReadTaskNumber++;
    }
    
    // The 0 read task number means the read task is completed
    if(--SpiMaster_u8ReadTaskNumber == 0)
    {
      SspDeAssertCS();
      SpiMaster_bReadTask = false;
    }
  }
  
  // If there is nothing to send but you need to read something, send a dummy byte
  if(SpiMaster_bReadTask && (!SpiMaster_bSendTask))
  {
    NRF_SPI0->TXD = SPI_DEFAULT_TX_BYTE;
  }
  else
  {
    // Send all unsend chars
    if(SpiMaster_pu8TxNextChar != SpiMaster_pu8UnsendChar)
    {
      NRF_SPI0->TXD = *SpiMaster_pu8TxNextChar;
      
      // Safely advance the transmitting next char pointer
      SpiMaster_pu8TxNextChar++;
      
      if(SpiMaster_pu8TxNextChar == &SpiMaster_TxBuffer[SPI_TX_BUFFER_SIZE])
      {
        SpiMaster_pu8TxNextChar = SpiMaster_TxBuffer;
      }
    }
    else
    {
      // Nothing to send means send task is completed
      SspDeAssertCS();
      SpiMaster_bSendTask = false;
    }
  }
  

} /* end GPIOTE_IRQHandler() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


