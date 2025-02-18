/*
 * GPIObase.cpp v2.0
 *
 *  Author: Adam Czaga czagaadam@gmail.com
 *
 *  GPIO parent class and Din,Doutt inherited classes
 *
 */

#include "GPIObase.h"

/*[[nodiscard]]*/ HAL_StatusTypeDef GPIObase::init(void)
{
	//if (InitErrorCheck(_Port, _GPIO_Pin) == HAL_ERROR) return HAL_ERROR;
	HAL_GPIO_Init(_Port, &_GPIOx);
	return HAL_OK;
}

HAL_StatusTypeDef GPIObase::InitErrorCheck(const GPIO_TypeDef* gpiox, gpio_num_t gpio_pin)
{
	 /* assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
	  assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
	  assert_param(IS_GPIO_MODE(GPIO_Init->Mode));*/
	HAL_StatusTypeDef err = HAL_OK;
	// TODO Not working
	if(gpiox == nullptr){return HAL_ERROR;}
	for(uint16_t pin = 0x8000; pin >= 0x0001; pin >>= 1)
	{
		if(gpio_pin == pin)
		{
			err = HAL_ERROR;
			break;
		}
		else
		{
			err = HAL_OK;
		}
	}
	return err;
}

uint16_t GPIObase::get_pin(void)
{
	return _GPIO_Pin;
}

/*[[nodiscard]]*/ HAL_StatusTypeDef Dout::init(void) {
	GPIObase::init();
	//todo: set level etc.
}

HAL_StatusTypeDef Dout::write(GPIO_PinState state)
{
	HAL_GPIO_WritePin(_Port, _GPIO_Pin, state);
	return HAL_OK;
}

HAL_StatusTypeDef Dout::toggle(void){
	HAL_GPIO_TogglePin(_Port, _GPIO_Pin);
	return HAL_OK;
}



/*[[nodiscard]]*/ HAL_StatusTypeDef Din::init(void) {
	GPIObase::init();
	//todo: read level
}

GPIO_PinState Din::read(void)
{
	return HAL_GPIO_ReadPin(_Port, _GPIO_Pin);
}


void Din::enable_interrupt(void)
{
	//#todo:
}

void Din::disable_interrupt(void)
{
	//#todo:
}






