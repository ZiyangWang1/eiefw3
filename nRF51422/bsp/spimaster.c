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

 

/***********************************************************************************************************************
* Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions */
/*--------------------------------------------------------------------------------------------------------------------*/

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
  
  return true;
}

bool SpiMasterSendByte(u8 * p_tx_buf)
{
  if(NRF_SPI0->EVENTS_READY == 1)
  {
    return false;
  }
  else
  {
    NRF_SPI0->TXD = *p_tx_buf;
    return true;
  }
}



/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions */
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/

  


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


