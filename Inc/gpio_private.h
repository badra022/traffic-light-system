/*
 * gpio_private.h
 *
 *  Created on: Jun 1, 2022
 *      Author: Badra
 */

#ifndef INC_GPIO_PRIVATE_H_
#define INC_GPIO_PRIVATE_H_

static volatile u32 gpio_port[11] = {GPIOA_ADDRESS, GPIOB_ADDRESS, GPIOC_ADDRESS, GPIOD_ADDRESS, GPIOE_ADDRESS, GPIOF_ADDRESS,
					GPIOG_ADDRESS,  GPIOH_ADDRESS, GPIOI_ADDRESS, GPIOJ_ADDRESS, GPIOK_ADDRESS};


#define GPIO_REG(PORT_ADDR, REG_OFFSET)		(*(unsigned int*)(PORT_ADDR + REG_OFFSET))

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

#endif /* INC_GPIO_PRIVATE_H_ */
