/******************************************************************************
File: spimaster.h                                                               

Description:
Header file for spimaster.c

DISCLAIMER: THIS CODE IS PROVIDED WITHOUT ANY WARRANTY OR GUARANTEES.  USERS MAY
USE THIS CODE FOR DEVELOPMENT AND EXAMPLE PURPOSES ONLY.  ENGENUICS TECHNOLOGIES
INCORPORATED IS NOT RESPONSIBLE FOR ANY ERRORS, OMISSIONS, OR DAMAGES THAT COULD
RESULT FROM USING THIS FIRMWARE IN WHOLE OR IN PART.

******************************************************************************/

#ifndef __SPIMASTER_H
#define __SPIMASTER_H

#include "configuration.h"

/******************************************************************************
Type Definitions
******************************************************************************/

#define SPI_PIN_DISCONNECTED 0xFFFFFFFF /**< A value used to the PIN deinitialization. */
#define SPI_DEFAULT_TX_BYTE  0x00       /**< Default byte (used to clock transmission
                                             from slave to the master) */
#define SPI_TX_BUFFER_SIZE 128

/**@brief Macro for initializing SPI master by default values. */
#define SPI_MASTER_INIT_DEFAULT                                             \
{                                                                           \
    SPI_FREQUENCY_FREQUENCY_M1, /**< Serial clock frequency 1 Mbps. */      \
    SPI_PIN_DISCONNECTED,       /**< SCK pin DISCONNECTED. */               \
    SPI_PIN_DISCONNECTED,       /**< MISO pin DISCONNECTED. */              \
    SPI_PIN_DISCONNECTED,       /**< MOSI pin DISCONNECTED. */              \
    SPI_PIN_DISCONNECTED,       /**< Slave select pin DISCONNECTED. */      \
    APP_IRQ_PRIORITY_LOW,       /**< Interrupt priority LOW. */             \
    SPI_CONFIG_ORDER_LsbFirst,  /**< Bits order LSB. */                     \
    SPI_CONFIG_CPOL_ActiveHigh, /**< Serial clock polarity ACTIVEHIGH. */   \
    SPI_CONFIG_CPHA_Leading,    /**< Serial clock phase LEADING. */         \
    0                           /**< Don't disable all IRQs. */             \
};

/**@brief A structure containing configuration parameters of the SPI master driver. */
typedef struct
{
    u32 SPI_Freq;          /**< SPI master frequency */
    u32 SPI_Pin_SCK;       /**< SCK pin number. */
    u32 SPI_Pin_MISO;      /**< MISO pin number. */
    u32 SPI_Pin_MOSI;      /**< MOSI pin number .*/
    u32 SPI_Pin_SS;        /**< Slave select pin number. */
    u8 SPI_CONFIG_ORDER;   /**< Bytes order LSB or MSB shifted out first. */
    u8 SPI_CONFIG_CPOL;    /**< Serial clock polarity ACTIVEHIGH or ACTIVELOW. */
    u8 SPI_CONFIG_CPHA;    /**< Serial clock phase LEADING or TRAILING. */
    u8 * p_rx_buffer;      /**< A pointer to RX buffer. */
    u8 ** pp_rx_nextchar;  /**< A pointer to a location where put the next char. */
    u16 rx_length;         /**< A length RX buffer. */
} spi_master_config_t;


/******************************************************************************
* Constants
******************************************************************************/


/******************************************************************************
* Function Declarations
******************************************************************************/
/* Public Functions */
bool SpiMasterOpen(spi_master_config_t * p_spi_master_config);
void SpiMasterSendByte(u8 * p_tx_buf);
bool SpiMasterSendData(u8 * p_tx_buf, u8 u8length);
bool SpiMasterReadByte(void);

/* Protected Functions */
void SpiMasterInitialize(void);
void SpiMasterRunActiveState(void);

/* Private Functions */


/*--------------------------------------------------------------------------------------------------------------------*/
/* SM functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
static void SpiMasterSM_Idle(void);
static void SpiMasterSM_Error(void);

/******************************************************************************
* State Machine Function Prototypes
******************************************************************************/


#endif /* __SPIMASTER_H */
