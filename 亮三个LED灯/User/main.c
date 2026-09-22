// 推荐使用标准头文件，代替手动typedef
#include <stdint.h>

//寄存器地址宏定义，彻底移除GPIOC
#define RCC_APB2ENR     *(volatile uint32_t *)0x40021018
#define GPIOA_CRL       *(volatile uint32_t *)0x40010800
#define GPIOA_ODR       *(volatile uint32_t *)0x4001080C

/**
 * @brief 简单软件延时，72MHz系统时钟，粗略延时约1秒
 * @note Keil编译器如果优化等级高，空循环会被优化掉，建议调试时用0优化等级
 */
void delay_1s(void)
{
    volatile uint32_t i,j;  //加上volatile，防止编译器优化删除循环
    for(i = 0; i < 800; i++)
    {
        for(j = 0; j < 12000; j++);
    }
}

/**
 * @brief GPIO初始化
 * PA1、PA3、PA5：面包板外接LED推挽输出2MHz
 * @note 本版本彻底不使用PC13/GPIOC，不开启GPIOC时钟
 */
void LED_GPIO_Init(void)
{
    //只开启GPIOA时钟 bit2；GPIOC完全不用，不再开启bit4
    RCC_APB2ENR |= (1U << 2);

    // ----------------------GPIOA_CRL配置 PA1 PA3 PA5----------------------
    // PA1：bit4~bit7清零，设置为0001 推挽输出2MHz
    GPIOA_CRL &= ~(0x000000F0);
    GPIOA_CRL |=  0x00000010;

    // PA3：bit12~bit15清零，设置为0001
    GPIOA_CRL &= ~(0x0000F000);
    GPIOA_CRL |=  0x00001000;

    // PA5：bit20~bit23清零，设置为0001
    GPIOA_CRL &= ~(0x00F00000);
    GPIOA_CRL |=  0x00100000;

    //PA1 PA3 PA5初始全部熄灭（外接LED低电平点亮）
    GPIOA_ODR &= ~((1U<<1) | (1U<<3) | (1U<<5));
}

int main(void)
{
    LED_GPIO_Init();
    while(1)
    {
        //1、只点亮 PA1，PA3 PA5熄灭
        GPIOA_ODR |=  (1U << 1);
        GPIOA_ODR &= ~((1U << 3) | (1U << 5));
        delay_1s();

        //2、只点亮 PA3
        GPIOA_ODR |=  (1U << 3);
        GPIOA_ODR &= ~((1U << 1) | (1U << 5));
        delay_1s();

        //3、只点亮 PA5
        GPIOA_ODR |=  (1U << 5);
        GPIOA_ODR &= ~((1U << 1) | (1U << 3));
        delay_1s();
    }
}
