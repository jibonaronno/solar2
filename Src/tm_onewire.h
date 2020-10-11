
#ifndef TM_ONEWIRE_H
#define TM_ONEWIRE_H 210

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
	
/* OneWire delay */
#define ONEWIRE_DELAY(x)				HAL_Delay(x)

/* Pin settings */
#define ONEWIRE_LOW(structure)			HAL_GPIO_WritePin((structure)->GPIOx, (structure)->GPIO_Pin, GPIO_PIN_RESET)
#define ONEWIRE_HIGH(structure)			HAL_GPIO_WritePin((structure)->GPIOx, (structure)->GPIO_Pin, GPIO_PIN_SET)
#define ONEWIRE_INPUT(structure)		GpioSetPinAsInput(structure->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_OUTPUT(structure)		GpioSetPinAsOutput(structure->GPIOx, (structure)->GPIO_Pin)

/* OneWire commands */
#define ONEWIRE_CMD_RSCRATCHPAD			0xBE
#define ONEWIRE_CMD_WSCRATCHPAD			0x4E
#define ONEWIRE_CMD_CPYSCRATCHPAD		0x48
#define ONEWIRE_CMD_RECEEPROM			0xB8
#define ONEWIRE_CMD_RPWRSUPPLY			0xB4
#define ONEWIRE_CMD_SEARCHROM			0xF0
#define ONEWIRE_CMD_READROM				0x33
#define ONEWIRE_CMD_MATCHROM			0x55
#define ONEWIRE_CMD_SKIPROM				0xCC

#define ONEWIRE_GPIO							GPIOC
#define ONEWIRE_PIN								GPIO_PIN_3
#define ONEWIRE_GPIO_INISTRUCT 		ONEWIRE_GPIO_InitStruct;

typedef struct {
	GPIO_TypeDef* GPIOx;           /*!< GPIOx port to be used for I/O functions */
	uint16_t GPIO_Pin;             /*!< GPIO Pin to be used for I/O functions */
	uint8_t LastDiscrepancy;       /*!< Search private */
	uint8_t LastFamilyDiscrepancy; /*!< Search private */
	uint8_t LastDeviceFlag;        /*!< Search private */
	uint8_t ROM_NO[8];             /*!< 8-bytes address of last search device */
} TM_OneWire_t;

int  writeSector(uint32_t Address,void * values, uint16_t size);
void eraseSector(uint32_t SectorStartAddress);
void readSector(uint32_t SectorStartAddress, void * values, uint16_t size);

	
/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif