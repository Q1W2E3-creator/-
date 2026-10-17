# 定时器&PWM应用编程

1.使用STM32F103的 Tim2~Tim5其一定时器的某一个通道pin（与GPIOx管脚复用），连接一个LED，用定时器计数方式，控制LED以2s的频率周期性地亮-灭。

2.接上，采用定时器PWM模式，让 LED 以呼吸灯方式渐亮渐灭，周期为1~2秒，自己调整占空比变化到一个满意效果；使用Keil虚拟示波器，观察 PWM输出波形。

## 一、实验原理

### 1. STM32通用定时器概述

STM32F103C8T6属于中等容量产品，内置4个通用定时器（TIM2~TIM5）。每个通用定时器包含一个16位自动装载计数器，由可编程预分频器驱动，具有4个独立通道，可用于输入捕获、输出比较、PWM生成和单脉冲模式输出。

通用定时器的时钟源来自APB1总线。当APB1预分频系数为1时，定时器时钟等于APB1时钟频率；否则定时器时钟为APB1时钟的2倍。在本实验的72MHz系统时钟配置下，APB1总线频率为36MHz，但由于APB1预分频系数不为1，定时器时钟为72MHz。

### 2 .定时器中断工作原理

定时器中断的触发流程如下：
1. 预分频器（PSC）对定时器时钟进行分频，得到计数器的实际工作时钟CK_CNT
2. 计数器（CNT）在每个CK_CNT周期递增（或递减）计数
3. 当CNT值达到自动重装载寄存器（ARR）的设定值时，产生更新事件（UEV）
4. 硬件置位状态寄存器（TIMx_SR）中的更新中断标志位（UIF）
5. 如果中断使能寄存器（TIMx_DIER）中的UIE位为1，则向NVIC发出中断请求
6. CPU响应中断，跳转到对应的中断服务函数（TIMx_IRQHandler）执行
7. 在中断服务函数中必须手动清除UIF标志位
定时周期的计算公式为：
$$
T_{out} = \frac{(ARR+1) \times (PSC+1)}{f_{clk}}
$$
其中TIMx_CLK为定时器输入时钟频率，ARR为自动重装载值，PSC为预分频系数

### 3 .PWM工作原理

PWM（脉冲宽度调制）通过改变输出脉冲的占空比来控制输出信号的平均电压。占空比定义为：

占空比=高电平时间周期×100%占空比=周期高电平时间×100%

STM32的PWM输出基于定时器的比较功能实现。每个通道都有一个捕获/比较寄存器（CCRx），当计数器的值CNT与CCRx匹配时，输出电平发生翻转：

- **PWM模式1**：向上计数时，CNT < CCRx时输出有效电平，否则输出无效电平
- **PWM模式2**：向上计数时，CNT < CCRx时输出无效电平，否则输出有效电平

PWM周期由ARR决定，占空比由CCRx决定：
$$
占空比 = \frac{\text{ARR+1} - \text{CCRx}}{\text{ARR}+1} \times 100\%
$$
例如，PSC=71，ARR=499时，PWM周期 = 500 × 72 / 72,000,000 = 0.5ms，频率为2kHz。

### 4 .引脚映射

本实验使用TIM3的通道1（PA6引脚）输出PWM信号驱动LED。STM32F103C8T6的TIM3通道映射如下：

| 定时器通道 | 引脚 | 说明                  |
| :--------- | :--- | :-------------------- |
| TIM2_CH1   | PA0  | PWM输出               |
| TIM2_CH2   | PA1  | PWM输出               |
| TIM3_CH1   | PA6  | PWM输出（本实验使用） |
| TIM3_CH2   | PA7  | PWM输出               |
| TIM3_CH3   | PB0  | PWM输出               |
| TIM3_CH4   | PB1  | PWM输出               |

## 二、硬件连接

### 1.定时器

![674dabd219fa06e1fa85216b327d6468](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\674dabd219fa06e1fa85216b327d6468.jpg)

### 2.定时器PWM

![694de155a8f2ce9ab34d29b252f56b5e](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\694de155a8f2ce9ab34d29b252f56b5e.jpg)

## 三、用定时器计数方式，控制LED以2s的频率周期性地亮-灭

### 1.创建工程

![8055f9d14f6dae218de60fa7b74762a7](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\8055f9d14f6dae218de60fa7b74762a7.png)

![69e8607ab2b334954c0d24fed0b863d0](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\69e8607ab2b334954c0d24fed0b863d0.png)

![b27901c8b405a07394c2fca1244778da](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\b27901c8b405a07394c2fca1244778da.png)

![667765c981144d5eaa2e52edbb949974](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\667765c981144d5eaa2e52edbb949974.png)

![3c31518f54f305a12a61dfff3b2ffb1d](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\3c31518f54f305a12a61dfff3b2ffb1d.png)

![23473789aa78d5eac6695a25df768012](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\23473789aa78d5eac6695a25df768012.png)

![0ca9b0a458d3a753620cdbc7d4132d9c](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\0ca9b0a458d3a753620cdbc7d4132d9c.png)

![fd74697674fb2188953afb8ff4091c89](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\fd74697674fb2188953afb8ff4091c89.png)

### 2.代码撰写

（1）在`/* USER CODE BEGIN PV */`之间添加：

```c
uint8_t tim2_count = 0;  // 定时器中断计数变量
```

（2）在 `/* USER CODE BEGIN 2 */` 之间添加：

```c
HAL_TIM_Base_Start_IT(&htim2);  // 启动TIM2定时器并使能中断
```

（3）在 `/* USER CODE BEGIN 4 */` 之间添加回调函数：

```c
/* 定时器更新中断回调函数 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    tim2_count++;
    if (tim2_count >= 2)  // 累计2次中断（即2秒）
    {
      tim2_count = 0;
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);  // 翻转PA5引脚
    }
  }
}
```

（4）完整代码

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
uint8_t tim2_count = 0;  // 新增：用于累计定时器中断次数
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // 新增：启动 TIM2 定时器并使能更新中断
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999; // 修改：原为65535，改为9999才能实现1秒定时
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  // 新增：使能 TIM2 的 NVIC 中断（如果你在CubeMX里没勾选中断，这里必须加上）
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  定时器更新中断回调函数
  * @param  htim: 定时器句柄
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    tim2_count++;
    if (tim2_count >= 2)  // 累计2次中断，即2秒
    {
      tim2_count = 0;     // 清零计数器
      // 翻转 PA5 引脚电平
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```

### 3.编译运行

![dc43d5a866d4e9ba63b571e36c6fefaa](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\dc43d5a866d4e9ba63b571e36c6fefaa.png)

![e5c452937183c75732f00ff5a8a987bd](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\e5c452937183c75732f00ff5a8a987bd.png)

![d7a14ac5587d1cf14534bb1ee0b643f7](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\d7a14ac5587d1cf14534bb1ee0b643f7.png)

![5ffd882df6117998aeb41471387a91f9](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\5ffd882df6117998aeb41471387a91f9.png)

![959c99424e146a03567a65f552d40a5a](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\959c99424e146a03567a65f552d40a5a.png)

![a87cb33a99abebe82b3e5fa045c75e14](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\a87cb33a99abebe82b3e5fa045c75e14.png)

### 4.实验结果

<video src="C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Video\2026-10\Ori\8837dce86637585124fb0f03917f42bb.mp4" controls=""></video>

## 四、PWM模式实现呼吸灯

### 1.创建工程

![e69cbf581c77c51be2cdd141ec15bfd3](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\e69cbf581c77c51be2cdd141ec15bfd3.png)

![ba4f3d6027ab6cc18c1580043697ab63](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\ba4f3d6027ab6cc18c1580043697ab63.png)

![8d5798d8f60a663790150cf04de944e4](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\8d5798d8f60a663790150cf04de944e4.png)

![ba632cd8db0bbca0cd474945d06523d9](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\ba632cd8db0bbca0cd474945d06523d9.png)

![366fe245dc1e22da21c7a0c04220cdd2](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\366fe245dc1e22da21c7a0c04220cdd2.png)

![0ddc0f3c33c600ddf339c9445c8aef8c](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\0ddc0f3c33c600ddf339c9445c8aef8c.png)

![ca63ed22284b8d75db9c6021e0347cf0](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\ca63ed22284b8d75db9c6021e0347cf0.png)

![8a3a2f6916ae53035f3a780bff58e2f5](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\8a3a2f6916ae53035f3a780bff58e2f5.png)

![0a2b63f7fa2b4f5962c78a44ee30b8ba](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\0a2b63f7fa2b4f5962c78a44ee30b8ba.png)

### 2.代码撰写

（1） 在 `/* USER CODE BEGIN PV */` 之间添加：

```c
uint16_t pwm_val = 0;    // 当前占空比值 (0~500)
uint8_t direction = 1;   // 1=渐亮，0=渐暗
```

（2）在 `/* USER CODE BEGIN 2 */` 之间添加：

```c
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  // 启动TIM3通道1 PWM输出
```

（3）在 `/* USER CODE BEGIN WHILE */` 和 `/* USER CODE END WHILE */` 之间添加：

```c
    // 呼吸速度调节：觉得快就把 5 改成 10 或 15
    HAL_Delay(5);

    if (direction)
    {
      pwm_val++;
      if (pwm_val >= 500) direction = 0; // 到达最亮转暗
    }
    else
    {
      pwm_val--;
      if (pwm_val == 0) direction = 1;   // 到达最暗转亮
    }
    
    // 动态修改比较寄存器（CCR1）的值，改变PWM占空比
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, pwm_val);
```

（4）完整代码

```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
uint16_t pwm_val = 0;    // 锟斤拷前占锟秸憋拷值 (锟斤拷围 0 ~ 500)
uint8_t direction = 1;   // 锟戒化锟斤拷锟斤拷1=锟斤拷锟斤拷(锟斤拷锟斤拷)锟斤拷0=锟斤拷锟斤拷(锟捷硷拷)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // 锟斤拷锟斤拷 TIM3 通锟斤拷1 锟斤拷 PWM 锟斤拷锟斤拷锟斤拷锟接?PA6 锟斤拷锟脚ｏ拷
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 锟斤拷锟斤拷锟斤拷锟劫度碉拷锟节关硷拷锟姐】
    // 锟斤拷锟斤拷锟斤拷煤锟斤拷锟教拷欤拷锟?5 锟侥筹拷 10 锟斤拷 15锟斤拷锟斤拷锟斤拷越锟斤拷锟斤拷锟皆斤拷锟?
    // 之前锟斤拷 1 锟斤拷 2 锟结导锟斤拷锟今急达拷锟斤拷烁锟斤拷5 锟饺较接斤拷锟剿碉拷锟斤拷锟斤拷锟斤拷锟斤拷锟?
    HAL_Delay(5); 

    if (direction)
    {
      pwm_val++; // 占锟秸憋拷锟斤拷锟接ｏ拷LED 锟金渐憋拷锟斤拷
      if (pwm_val >= 500) 
      {
        direction = 0; // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫伙拷为锟斤拷锟斤拷
      }
    }
    else
    {
      pwm_val--; // 占锟秸比硷拷锟劫ｏ拷LED 锟金渐变暗
      if (pwm_val == 0) 
      {
        direction = 1; // 锟斤拷锟斤拷锟筋暗锟斤拷锟叫伙拷为锟斤拷锟斤拷
      }
    }

    // 锟斤拷态锟睫改讹拷时锟斤拷锟饺较寄达拷锟斤拷锟斤拷CCR1锟斤拷锟斤拷值锟斤拷锟接讹拷锟侥憋拷 PWM 占锟秸憋拷
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, pwm_val);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

```

### 3.代码编译 运行如上

### 4.实验结果

<video src="C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Video\2026-10\Ori\a689cc7306ddd73440c4ec6aa1d58d7b.mp4" controls=""></video>

### 5.PWM输出波形

#### （1）环境搭建

![044ac16e5704d192ad0e1f0e3aeea9df](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\044ac16e5704d192ad0e1f0e3aeea9df.png)

![acbfd02a172b5d081eeab11cf6481e00](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\acbfd02a172b5d081eeab11cf6481e00.png)

![fb616a68521b5dc1187eec1bb5e03365](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\fb616a68521b5dc1187eec1bb5e03365.png)

![3984134b6dc8e35ef146062b420b6294](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\3984134b6dc8e35ef146062b420b6294.png)

![46d51513088dd4e91f395973b90b3671](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\46d51513088dd4e91f395973b90b3671.png)

#### （2）分析波形图

![2d4bee70e582a9f75c17cbf19ab75cb5](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\2d4bee70e582a9f75c17cbf19ab75cb5.png)

![e024f6e1dd372c95f4639b3ef44d344c](C:\Users\32068\Documents\Tencent Files\3206842990\nt_qq\nt_data\Pic\2026-10\Ori\e024f6e1dd372c95f4639b3ef44d344c.png)

**PWM 周期整体观测**

下图为呼吸灯运行过程中 PWM 波形的整体观测，通过光标测量得到单个 PWM 周期时长为 **0.500556 ms**。

**周期参数验证：**

- 理论计算值： \(T = \frac{(ARR+1) \times (PSC+1)}{f_{clk}} = \frac{(499+1) \times (71+1)}{72 \times 10^6} = 0.5\ \text{ms}\)
- 实际仿真值：约 0.5006 ms

实测值与理论值误差小于 0.2%，验证了定时器预分频、自动重装载参数配置正确，PWM 输出频率稳定为 2kHz，高于人眼视觉暂留频率，保证 LED 亮度调节无频闪。

**脉宽细节与占空比观测**

图为波形放大后的脉宽测量，测得该时刻低电平持续时间为 **91.40278 μs**。

**占空比计算验证**： 已知周期 \(T \approx 500.556\ \mu\text{s}\)，低电平时间 \(T_L \approx 91.403\ \mu\text{s}\)，则： 高电平时间：

\(T_H = T - T_L \approx 500.556 - 91.403 = 409.153\ \mu\text{s}\) 占空比：

\(Duty = \frac{T_H}{T} \times 100\% \approx \frac{409.153}{500.556} \times 100\% \approx 81.7\%\)

该占空比对应呼吸灯 “渐亮 / 渐暗” 过程中的某一中间状态，验证了程序可以通过`__HAL_TIM_SetCompare()`动态修改比较寄存器值，连续调整 PWM 占空比，进而实现 LED 亮度的平滑过渡。

**呼吸过程整体验证**

连续观测波形可看到：PWM 的高电平宽度随时间线性递增，达到最大值后线性递减，周期性循环变化。完整的 “0%→100%→0%” 占空比变化周期约为 5 秒，与程序中 “5ms 步进 × 500 级亮度 × 2（亮 + 暗）” 的参数设计完全匹配，实现了平缓的呼吸灯光效

## 五、总结

本次实验让我深入掌握了STM32定时器中断与PWM输出的核心原理及PSC/ARR周期计算方法，并通过Keil软件仿真成功验证了占空比变化。在实战中，我积累了代码重定义、ST-Link烧录失败以及示波器无波形等常见报错的排查经验，深刻体会到嵌入式开发必须软硬结合、理论与调试工具并重，为后续复杂外设控制打下了坚实基础。