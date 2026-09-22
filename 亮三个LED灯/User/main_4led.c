/**
  ******************************************************************************
  * 文件名   : main_4led.c
  * 说明     : STM32F103C8T6 流水灯实验 —— 程序2（四灯版，重编）
  *            在程序1基础上，把板载 PC13 LED（低电平点亮）也加入流水灯。
  *            GPIOA(PA0)、GPIOB(PB0)、GPIOC(PC14) + 板载 PC13，共 4 灯轮流闪烁。
  *            纯寄存器方式：直接对寄存器绝对地址读写，不调用任何库函数。
  *            延时复用工程 System/Delay.c 的 Delay_ms()（SysTick，已按 72MHz 标定）。
  * 目标板   : STM32F103C8T6 最小系统核心板
  * 系统时钟 : 72MHz（HSE 8MHz 经 PLL 9 倍频，由 system_stm32f10x.c 的 SystemInit 配置）
  ******************************************************************************
  */

#include <stdint.h>          /* 提供 uint32_t 类型 */
#include "Delay.h"           /* 复用工程已有的 SysTick 精确延时 Delay_ms() */

/* ======================= 寄存器地址定义 ======================= */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018u)
#define GPIOA_CRL     (*(volatile uint32_t *)0x40010800u)
#define GPIOA_ODR     (*(volatile uint32_t *)0x4001080Cu)
#define GPIOB_CRL     (*(volatile uint32_t *)0x40010C00u)
#define GPIOB_ODR     (*(volatile uint32_t *)0x40010C0Cu)
#define GPIOC_CRH     (*(volatile uint32_t *)0x40011004u)
#define GPIOC_ODR     (*(volatile uint32_t *)0x4001100Cu)

/* ======================= 灯控制宏 ======================= */
/* 外部 LED 高电平点亮；板载 PC13 低电平点亮（LED 阳极端接 3.3V，PC13 拉低才亮） */
#define LED1_ON()     (GPIOA_ODR |=  (1u << 0))   /* PA0  红色 */
#define LED1_OFF()    (GPIOA_ODR &= ~(1u << 0))
#define LED2_ON()     (GPIOB_ODR |=  (1u << 0))   /* PB0  绿色 */
#define LED2_OFF()    (GPIOB_ODR &= ~(1u << 0))
#define LED3_ON()     (GPIOC_ODR |=  (1u << 14))  /* PC14 蓝色 */
#define LED3_OFF()    (GPIOC_ODR &= ~(1u << 14))
#define LED4_ON()     (GPIOC_ODR &= ~(1u << 13))  /* PC13 板载灯：低电平点亮 */
#define LED4_OFF()    (GPIOC_ODR |=  (1u << 13))

/* ======================= GPIO 初始化 ======================= */
void LED_Init(void)
{
    /* 1. 使能 GPIOA、GPIOB、GPIOC 时钟 */
    RCC_APB2ENR |= (1u << 2) | (1u << 3) | (1u << 4);   /* IOPAEN|IOPBEN|IOPCEN */

    /* 2. 配置 4 个引脚为「通用推挽输出、50MHz」
     *    每个引脚占 4 位：CNF[1:0] MODE[1:0] = 00 11 = 0x3 */
    GPIOA_CRL &= ~(0xFu << 0);   GPIOA_CRL |= (0x3u << 0);   /* PA0  */
    GPIOB_CRL &= ~(0xFu << 0);   GPIOB_CRL |= (0x3u << 0);   /* PB0  */
    GPIOC_CRH &= ~(0xFu << 20);  GPIOC_CRH |= (0x3u << 20);  /* PC13 引脚13 -> CRH 位20~23 */
    GPIOC_CRH &= ~(0xFu << 24);  GPIOC_CRH |= (0x3u << 24);  /* PC14 引脚14 -> CRH 位24~27 */

    /* 3. 初始状态：所有 LED 熄灭（PC13 置 1 即熄灭） */
    LED1_OFF();
    LED2_OFF();
    LED3_OFF();
    LED4_OFF();
}

/* ======================= 主函数 ======================= */
int main(void)
{
    LED_Init();

    while (1)
    {
        LED1_ON();  Delay_ms(1000);  LED1_OFF();   /* PA0  亮 1s */
        LED2_ON();  Delay_ms(1000);  LED2_OFF();   /* PB0  亮 1s */
        LED3_ON();  Delay_ms(1000);  LED3_OFF();   /* PC14 亮 1s */
        LED4_ON();  Delay_ms(1000);  LED4_OFF();   /* PC13 板载灯 亮 1s */
    }
}
