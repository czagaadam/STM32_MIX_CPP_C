/*
 * GPIObase.h v2.0
 *
 *  Author: Adam Czaga czagaadam@gmail.com
 *
 *  GPIO parent class and Din,Doutt inherited classes
 *
 */

#ifndef GPIOBASE_H_
#define GPIOBASE_H_

#include "stm32f4xx_hal.h"
//#include "gpio.h"

typedef uint16_t gpio_num_t;

/*typedef enum
{
    GPIO_Pin_State_Off = 0,
    GPIO_Pin_State_On = 1
} GPIO_PinState_t;*/

typedef enum GPIO_InvertedLogic
{
    GPIO_Inverted_Logic_Off = 0,
    GPIO_Inverted_Logic_On = 1
} GPIO_InvertedLogic_t;

typedef enum{
    STATE_HOME,
    STATE_SETUP,
} SSTATE;

extern SSTATE sstate;

class GPIObase
{
	public:

    constexpr GPIObase(
    const GPIO_InitTypeDef &gpiox,
    GPIO_TypeDef* port,
    const gpio_num_t gpio_pin,
    const GPIO_InvertedLogic_t inverted_logic = GPIO_Inverted_Logic_Off,
	GPIO_PinState state = GPIO_PIN_RESET
    ) :
    _GPIOx{gpiox},//todo rename to init struct
	_Port{port},
    _GPIO_Pin{gpio_pin},
    _Inverted_Logic{inverted_logic},
    _State{state}{};
    /*[[nodiscard]]*/ HAL_StatusTypeDef init(void);
    //GPIO_PinState_t read(void);
    //virtual GPIO_PinState write(void) =0;
    gpio_num_t get_pin(void);

	protected:
    HAL_StatusTypeDef InitErrorCheck(const GPIO_TypeDef* gpiox, const gpio_num_t gpio_pin);
    GPIO_InitTypeDef _GPIOx;
    GPIO_TypeDef* _Port;	//HAL_GPIO_Init
    const gpio_num_t _GPIO_Pin;
    const GPIO_InvertedLogic_t _Inverted_Logic = GPIO_Inverted_Logic_Off;
    GPIO_PinState _State = GPIO_PIN_RESET;
    bool isInit = false;

	private:

	
};


class Dout : public GPIObase
{
	public:

	constexpr Dout(
	GPIO_TypeDef* port,
	const gpio_num_t gpio_pin,
	const GPIO_InvertedLogic_t inverted_logic = GPIO_Inverted_Logic_Off,
	GPIO_PinState state = GPIO_PIN_RESET
	) :
	GPIObase{GPIO_InitTypeDef{
		  .Pin = gpio_pin,
		  .Mode = GPIO_MODE_OUTPUT_PP,
		  .Pull = GPIO_NOPULL,
		  .Speed = GPIO_SPEED_FREQ_LOW,
	},
	port,
	gpio_pin,
	inverted_logic,
	state
	}
	{
	};

	/*[[nodiscard]]*/ HAL_StatusTypeDef init(void);
	HAL_StatusTypeDef write(GPIO_PinState state);
	HAL_StatusTypeDef toggle(void);

	protected:
	private:

};


class Din : public GPIObase
{
	public:

	constexpr Din(
	GPIO_TypeDef* port,
	const gpio_num_t gpio_pin,
    const GPIO_InvertedLogic_t inverted_logic = GPIO_Inverted_Logic_Off) :
    GPIObase{GPIO_InitTypeDef{
    		.Pin = gpio_pin,
    		.Mode = GPIO_MODE_IT_FALLING,
    		.Pull = GPIO_PULLUP,
    },
	port,
    gpio_pin,
    inverted_logic,
	GPIO_PIN_RESET
    }
    {

    };

    /*[[nodiscard]]*/ HAL_StatusTypeDef init(void);
    GPIO_PinState read(void);
    void enable_interrupt(void);
    void disable_interrupt(void);
	protected:
	private:
};


class I2Cpin : public GPIObase
{
	public:

	constexpr I2Cpin(
	GPIO_TypeDef* port,
	const gpio_num_t gpio_pin,
    const GPIO_InvertedLogic_t inverted_logic = GPIO_Inverted_Logic_Off) :
    GPIObase{GPIO_InitTypeDef{
        .Pin = gpio_pin,
        .Mode = GPIO_MODE_AF_OD,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Alternate = GPIO_AF4_I2C3
    },
	port,
    gpio_pin,
    inverted_logic,
	GPIO_PIN_RESET
    }
    {

    };

    /*[[nodiscard]]*/ HAL_StatusTypeDef init(void);
    GPIO_PinState read(void);
    void enable_interrupt(void);
    void disable_interrupt(void);
	protected:
	private:
};










#endif /* GPIOBASE_H_ */
