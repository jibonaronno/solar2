
#include "tm_onewire.h"

GPIO_InitTypeDef ONEWIRE_GPIO_InitStruct;

void GpioSetPinAsInput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	ONEWIRE_GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOx, &ONEWIRE_GPIO_InitStruct);
}

void GpioSetPinAsOutput(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	ONEWIRE_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOx, &ONEWIRE_GPIO_InitStruct);
}

int  writeSector(uint32_t Address,void * values, uint16_t size)
{              
    uint16_t *AddressPtr;
    uint16_t *valuePtr;
    AddressPtr = (uint16_t *)Address;
    valuePtr=(uint16_t *)values;
    size = size / 2;  // incoming value is expressed in bytes, not 16 bit words
    while(size) {        
        // unlock the flash 
        // Key 1 : 0x45670123
        // Key 2 : 0xCDEF89AB
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
        FLASH->CR &= ~(1 << 1); // ensure PER is low
        FLASH->CR |= (1 << 0);  // set the PG bit        
        *(AddressPtr) = *(valuePtr);
        while(FLASH->SR & (1 << 0)); // wait while busy
        if (FLASH->SR & (1<<2))
            return -1; // flash not erased to begin with
        if (FLASH->SR & (1<<4))
            return -2; // write protect error
        AddressPtr++;
        valuePtr++;
        size--;
    }    
    return 0;    
}
void eraseSector(uint32_t SectorStartAddress)
{
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
    FLASH->CR &= ~(1<<0);  // Ensure PG bit is low
    FLASH->CR |= (1<<1); // set the PER bit
    FLASH->AR = SectorStartAddress;
    FLASH->CR |= (1<<6); // set the start bit 
    while(FLASH->SR & (1<<0)); // wait while busy
}
void readSector(uint32_t SectorStartAddress, void * values, uint16_t size)
{
    uint16_t *AddressPtr;
    uint16_t *valuePtr;
    AddressPtr = (uint16_t *)SectorStartAddress;
    valuePtr=(uint16_t *)values;
    size = size/2; // incoming value is expressed in bytes, not 16 bit words
    while(size)
    {
        *((uint16_t *)valuePtr)=*((uint16_t *)AddressPtr);
        valuePtr++;
        AddressPtr++;
        size--;
    }
}

/*
__STATIC_INLINE void DelayMicro(__IO uint32_t micros)
{
	micros *= (SystemCoreClock / 1000000) / 9;
	while (micros--) ;
}
*/

void tm_OneWire_Init(TM_OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) 
{

	/* Init GPIO pin */
	//TM_GPIO_Init(GPIOx, GPIO_Pin, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	
	ONEWIRE_GPIO_InitStruct.Pin = ONEWIRE_PIN;
  ONEWIRE_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  ONEWIRE_GPIO_InitStruct.Pull = GPIO_PULLUP;
  ONEWIRE_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOx, &ONEWIRE_GPIO_InitStruct);
	
	/* Save settings */
	OneWireStruct->GPIOx = GPIOx;
	OneWireStruct->GPIO_Pin = GPIO_Pin;
}

uint8_t tm_OneWire_Reset(TM_OneWire_t* OneWireStruct) 
{
	uint8_t i;
	
	/* Line low, and wait 480us */
	ONEWIRE_LOW(OneWireStruct);
	ONEWIRE_OUTPUT(OneWireStruct);
	ONEWIRE_DELAY(480);
	
	/* Release line and wait for 70us */
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(70);
	
	/* Check bit value */
	//i = TM_GPIO_GetInputPinValue(OneWireStruct->GPIOx, OneWireStruct->GPIO_Pin);
	
	i = HAL_GPIO_ReadPin(OneWireStruct->GPIOx, OneWireStruct->GPIO_Pin);
	
	/* Delay for 410 us */
	ONEWIRE_DELAY(410);
	
	/* Return value of presence pulse, 0 = OK, 1 = ERROR */
	return i;
}

void tm_OneWire_WriteBit(TM_OneWire_t* OneWireStruct, uint8_t bit) {
	if (bit) {
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		ONEWIRE_DELAY(10);
		
		/* Bit high */
		ONEWIRE_INPUT(OneWireStruct);
		
		/* Wait for 55 us and release the line */
		ONEWIRE_DELAY(55);
		ONEWIRE_INPUT(OneWireStruct);
	} else {
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		ONEWIRE_DELAY(65);
		
		/* Bit high */
		ONEWIRE_INPUT(OneWireStruct);
		
		/* Wait for 5 us and release the line */
		ONEWIRE_DELAY(5);
		ONEWIRE_INPUT(OneWireStruct);
	}

}
