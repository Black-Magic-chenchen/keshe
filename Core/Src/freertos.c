/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "DHT11.h"
#include "oled.h"
#include "stdio.h"
#include "hal_key.h"
#include "gizwits_product.h"
#include "common.h"
#include "atgm336h.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define GPIO_KEY_NUM 2                ///< Defines the total number of key member
keyTypedef_t singleKey[GPIO_KEY_NUM]; ///< Defines a single key member array pointer
keysTypedef_t keys;
uint8_t temp = 0;
uint8_t humi = 0;
uint16_t adc_value = 0;
uint16_t distance = 0;
float latitude = 0.0f;
float longitude = 0.0f;
int temp_threshold = 30; // 30 degrees Celsius
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_4) // 假设你的按键接在PC13
  {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET) // 检测按键是否被按下
    {
      temp_threshold += 1;
    }
  }
  else if (GPIO_Pin == GPIO_PIN_5) // 假设你的按键接在PC14
  {
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET) // 检测按键是否被按下
    {
      temp_threshold -= 1;
    }
  }
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);
void StartTask02(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityIdle, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  OLED_Init();
  DHT11_Init();
  HAL_ADC_Start(&hadc1);
  int sign = 0;
  timerInit();
  uartInit();
  userInit();
  gizwitsInit();
  atgm336h_init();
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // Start PWM for LED control
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Start PWM for PB3 software PWM

  /* Infinite loop */
  for (;;)
  {
    parseGpsBuffer();
    if (Save_Data.isParseData)
    {
      Save_Data.isParseData = false;
      if (Save_Data.isUsefull)
      {
        Save_Data.isUsefull = false;
        latitude = g_LatAndLongData.latitude;
        longitude = g_LatAndLongData.longitude;
      }
    }
    userHandle();
    gizwitsHandle((dataPoint_t *)&currentDataPoint);

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    adc_value = HAL_ADC_GetValue(&hadc1);

    uint8_t cmd = 0x55;
    HAL_UART_Transmit(&huart1, &cmd, 1, 100);
    uint8_t rx_buf[2] = {0};
    if (HAL_UART_Receive(&huart1, rx_buf, 2, 100) == HAL_OK)
    {
      distance = rx_buf[0] * 256 + rx_buf[1];
    }
    else
    {
      distance = 0xFFFF;
    }
    distance = rx_buf[0] * 256 + rx_buf[1];
    if (distance < 100)
    {
      if (sign == 0)
      {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        sign = 1;
        delay_ms(50);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
      }
    }
    else
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
      sign = 0;
    }
    DHT11_Read_Data(&temp, &humi);
    if (temp > temp_threshold)
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Turn on the buzzer
    }
    else
    {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET); // Turn off the buzzer
    }
    char buffer[50];
    sprintf(buffer, "Dist: %dmm  ", distance);
    OLED_ShowString(0, 16, buffer, 12);
    sprintf(buffer, "ADC: %d  ", adc_value);
    OLED_ShowString(0, 0, buffer, 12);
    sprintf(buffer, "Temp_Thre: %dC  ", temp_threshold);
    OLED_ShowString(0, 32, buffer, 12);
    OLED_Refresh();
    // 光照值转化为LED亮度（假设adc_value范围0~4095，PWM最大值为999）
    uint16_t led_pwm = 0;
    if (adc_value <= 500)
    {
      led_pwm = 0; // 灯不亮
    }
    else if (adc_value >= 3000)
    {
      led_pwm = 999; // 最大亮度
    }
    else
    {
      led_pwm = (adc_value - 500) * 999 / (3000 - 500); // 线性映射
    }
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, led_pwm); // 设置LED亮度
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, led_pwm); // 设置PB3的PWM占空比
    HAL_IWDG_Init(&hiwdg);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
 * @brief Function implementing the myTask02 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTask02 */
void StartTask02(void const *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for (;;)
  {
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET)
    {
      temp_threshold++;
    }
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET)
    {
      temp_threshold--;
    }
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
