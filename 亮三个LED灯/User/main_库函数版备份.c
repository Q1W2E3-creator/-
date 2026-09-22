typedef unsigned int uint32_t;

//寄存器地址宏定义
#define RCC_APB2ENR     *(volatile uint32_t *)0x40021018

#define GPIOA_CRL       *(volatile uint32_t *)0x40010800
#define GPIOA_ODR       *(volatile uint32_t *)0x4001080C

#define GPIOC_CRH       *(volatile uint32_t *)0x40011004
#define GPIOC_ODR       *(volatile uint32_t *)0x4001100C

/**
 * @brief 简单软件延时，72MHz系统时钟，粗略延时约1秒
 */
void delay_1s(void)
{
    uint32_t i,j;
    for(i = 0; i < 800; i++)
    {
        for(j = 0; j < 12000; j++);
    }
}

/**
 * @brief GPIO初始化
 * PA1、PA3、PA5：面包板外接LED推挽输出2MHz
 * PC13：板载LED推挽输出2MHz
 */
void LED_GPIO_Init(void)
{
    //开启GPIOA(bit2)、GPIOC(bit4)时钟
    RCC_APB2ENR |= (1U << 2) | (1U << 4);

    // ----------------------GPIOA_CRL配置 PA1 PA3 PA5----------------------
    // PA1：bit4?bit7清零，设置为0001 推挽输出2MHz
    GPIOA_CRL &= ~(0x000000F0);
    GPIOA_CRL |=  0x00000010;

    // PA3：bit12?bit15清零，设置为0001
    GPIOA_CRL &= ~(0x0000F000);
    GPIOA_CRL |=  0x00001000;

    // PA5：bit20?bit23清零，设置为0001
    GPIOA_CRL &= ~(0x00F00000);
    GPIOA_CRL |=  0x00100000;

    // ----------------------PC13板载LED配置 GPIOC_CRH----------------------
    // PC13对应bit20?bit23，清零，配置0001推挽输出2MHz
    GPIOC_CRH &= ~(0x00F00000);
    GPIOC_CRH |=  0x00100000;

    //全部LED初始状态熄灭
    GPIOA_ODR &= ~((1U<<1) | (1U<<3) | (1U<<5));   // PA1 PA3 PA5输出低，外接LED熄灭
    GPIOC_ODR |= (1U << 13);                       // PC13输出高电平，板载LED熄灭
}

int main(void)
{
    LED_GPIO_Init();

    while(1)
    {
        //1、只点亮 PA1，其余全部熄灭
        GPIOA_ODR |=  (1U << 1);
        GPIOA_ODR &= ~((1U << 3) | (1U << 5));
        GPIOC_ODR |=  (1U <<13);
        delay_1s();

        //2、只点亮 PA3
        GPIOA_ODR |=  (1U << 3);
        GPIOA_ODR &= ~((1U << 1) | (1U << 5));
        GPIOC_ODR |=  (1U <<13);
        delay_1s();

        //3、只点亮 PA5
        GPIOA_ODR |=  (1U << 5);
        GPIOA_ODR &= ~((1U << 1) | (1U << 3));
        GPIOC_ODR |=  (1U <<13);
        delay_1s();

        //4、只点亮板载PC13
        GPIOA_ODR &= ~((1U <<1)|(1U <<3)|(1U <<5));
        GPIOC_ODR &= ~(1U <<13); //PC13输出低电平点亮板载LED
        delay_1s();
    }
}
