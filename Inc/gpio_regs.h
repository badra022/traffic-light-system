/************************************************************/
/*********** Author		: Ahmed Mohamed Badra	*************/
/*********** Date		: 5/1/2021				*************/
/*********** Version	: V01					*************/
/************************************************************/
#ifndef INC_GPIO_REGS_H_
#define INC_GPIO_REGS_H_


#define GPIOA_ADDRESS		0x40020000
#define GPIOB_ADDRESS		0x40020400
#define GPIOC_ADDRESS		0x40020800
#define GPIOD_ADDRESS		0x40020C00
#define GPIOE_ADDRESS		0x40021000
#define GPIOF_ADDRESS		0x40021400
#define GPIOG_ADDRESS		0x40021800
#define GPIOH_ADDRESS		0x40021C00
#define GPIOI_ADDRESS		0x40022000
#define GPIOJ_ADDRESS		0x40022400
#define GPIOK_ADDRESS		0x40022800

static unsigned int gpio_port[] = {GPIOA_ADDRESS, GPIOB_ADDRESS, GPIOC_ADDRESS, GPIOD_ADDRESS, GPIOE_ADDRESS, GPIOF_ADDRESS,
					GPIOG_ADDRESS,  GPIOH_ADDRESS, GPIOI_ADDRESS, GPIOJ_ADDRESS, GPIOK_ADDRESS};

#define GPIO_REG(PORT_ADDR, REG_OFFSET)		(*(unsigned int*)(PORT_ADDR + REG_OFFSET))

#define RCC_AHB1ENR				GPIO_REG(0x40023800, 0x30)
#define GPIO_MODER(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x00)
#define GPIO_OTYPER(PORT_ID)	GPIO_REG(gpio_port[(u32)PORT_ID], 0x04)
#define GPIO_OSPEEDR(PORT_ID)	GPIO_REG(gpio_port[(u32)PORT_ID], 0x08)
#define GPIO_PUPDR(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x0C)
#define GPIO_IDR(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x10)
#define GPIO_ODR(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x14)
#define GPIO_BSRR(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x18)
#define GPIO_LCKR(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x1C)
#define GPIO_AFRL(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x20)
#define GPIO_AFRH(PORT_ID)		GPIO_REG(gpio_port[(u32)PORT_ID], 0x24)

#endif /* INC_GPIO_REGS_H_ */
