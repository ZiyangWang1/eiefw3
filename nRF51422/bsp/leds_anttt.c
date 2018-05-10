/***********************************************************************************************************************
File: leds_anttt.c                                                                

Description:
***********************************************************************************************************************/

#include "configuration.h"


/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_xxLed"
***********************************************************************************************************************/
/*--------------------------------------------------------------------------------------------------------------------*/
/* New variables (all shall start with G_xxLed*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* External global variables defined in other files (must indicate which file they are defined in) */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern volatile u32 G_u32ApplicationFlags;             /* From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Led_" and be declared as static.
***********************************************************************************************************************/

 

/***********************************************************************************************************************
* Function Definitions
***********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------

void LedOn(LedNumberType eLED_)

Turn the specified LED on.  

The function works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedOn(BLUE);


Requires:
- Definitions in Leds_asLedArray[eLED_] are correct

eLED_ is a valid LED index

Promises:
- eLED_ is turned on 

*/
void LedOn(LedNumberType eLED_)
{
  u32 u32SetAddress;
  
  u32SetAddress = 1 << (26 + eLED_);
  NRF_GPIO->OUTSET = u32SetAddress;
} /* end LedOn() */

/*----------------------------------------------------------------------------------------------------------------------
void LedOff(LedNumberType eLED_)

Turn the specified LED off.

It works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedOff(BLUE);


Requires:
- Definitions in Leds_asLedArray[eLED_] are correct

eLED_ is a valid LED index

Promises:
- eLED_ is turned off 

*/
void LedOff(LedNumberType eLED_)
{
  u32 u32SetAddress;
  
  u32SetAddress = 1 << (26 + eLED_);
  NRF_GPIO->OUTCLR = u32SetAddress;
} /* end LedOn() */

/*----------------------------------------------------------------------------------------------------------------------
void LedToggle(LedNumberType eLED_)

Toggles the specified LED from on to off or vise-versa.

It works immediately (it does not require the main application
loop to be running). 

Currently it only supports one LED at a time.

Example:

LedToggle(BLUE);


Requires:

eLED_ is a valid LED index

Promises:
- eLED_ is toggled

*/
void LedToggle(LedNumberType eLED_)
{
  u32 u32SetAddress;
  
  u32SetAddress = 1 << (26 + eLED_);
  NRF_GPIO->OUT ^= u32SetAddress;
} /* end LedToggle() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions */
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: LedsInitialize

Description:
Initializes the LEDs.

Requires:

Promises:
*/
void LedsInitialize(void)
{
  u8 u8Leds = 0;
  u8 u8CirTimes = 0;
  NRF_GPIO->PIN_CNF[P0_29_INDEX] = P0_29_LED_RED_CNF;
  NRF_GPIO->PIN_CNF[P0_28_INDEX] = P0_28_LED_YELLOW_CNF;
  NRF_GPIO->PIN_CNF[P0_27_INDEX] = P0_27_LED_GREEN_CNF;
  NRF_GPIO->PIN_CNF[P0_26_INDEX] = P0_26_LED_BLUE_CNF;
  
  for(u8CirTimes=0;u8CirTimes<5;u8CirTimes++)
  {
    for(u8Leds=0;u8Leds<4;u8Leds++)
    {
      LedToggle((LedNumberType)u8Leds);
    }
    nrf_delay_us(500000);
  }
  
  for(u8Leds=0;u8Leds<4;u8Leds++)
  {
    LedOff((LedNumberType)u8Leds);
  }
  
} /* end LedsInitialize() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File */
/*--------------------------------------------------------------------------------------------------------------------*/


