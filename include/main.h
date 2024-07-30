
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"
void Error_Handler(void);

#define RST_Pin GPIO_PIN_0
#define RST_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_1
#define CS_GPIO_Port GPIOA
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
