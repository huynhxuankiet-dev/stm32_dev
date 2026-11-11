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
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* PCF8574 Configuration */

#define PCF8574_ADDR_7BIT    0x3F
#define PCF8574_ADDR         (PCF8574_ADDR_7BIT << 1)

#define PCF8574_INPUT        0xFF

#define DIP1_MASK            (1 << 0)
#define DIP2_MASK            (1 << 1)
#define DIP3_MASK            (1 << 2)
#define DIP4_MASK            (1 << 3)
#define DIP5_MASK            (1 << 4)
#define DIP6_MASK            (1 << 5)
#define DIP7_MASK            (1 << 6)
#define DIP8_MASK            (1 << 7)

uint8_t dip_raw = 0xFF;
uint8_t dip_status = 0x00;

HAL_StatusTypeDef i2c_status;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
HAL_StatusTypeDef PCF8574_Init(void);

HAL_StatusTypeDef PCF8574_Write(uint8_t data);

HAL_StatusTypeDef PCF8574_Read(uint8_t *data);

HAL_StatusTypeDef DIP_Read(uint8_t *data);

void I2C_Scan(void);

void PCF8574_Test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int fd, char* ptr, int len) {
  HAL_StatusTypeDef hstatus;

  if (fd == 1 || fd == 2) {
	hstatus = HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
	if (hstatus == HAL_OK)
	  return len;
	else
	  return -1;
  }
  return -1;
}

HAL_StatusTypeDef PCF8574_Write(uint8_t data)
{
    return HAL_I2C_Master_Transmit(
        &hi2c1,
        PCF8574_ADDR,
        &data,
        1,
        100
    );
}

/* Initialize DIP switch inputs */
HAL_StatusTypeDef PCF8574_Read(uint8_t *data)
{
    if (data == NULL)
        return HAL_ERROR;

    return HAL_I2C_Master_Receive(
        &hi2c1,
        PCF8574_ADDR,
        data,
        1,
        100
    );
}

// Go through all possible i2c addresses
void I2C_Scan(void)
{
    printf("\r\nI2C SCAN START\r\n");

    for (uint8_t i = 1; i < 128; i++)
    {
        if (HAL_I2C_IsDeviceReady(
                &hi2c1,
                (uint16_t)(i << 1),
                2,
                20) == HAL_OK)
        {
            printf("FOUND: 0x%02X\r\n", i);
        }
    }

    printf("I2C SCAN FINISHED\r\n");
}
HAL_StatusTypeDef PCF8574_Init(void)
{
    /*
     * Write 1 to all pins.
     * Release all GPIO pins for input.
     */

    return PCF8574_Write(PCF8574_INPUT);
}

HAL_StatusTypeDef DIP_Read(uint8_t *data)
{
    HAL_StatusTypeDef status;

    if (data == NULL)
        return HAL_ERROR;

    // Step 1: Release all pins for input
    status = PCF8574_Write(0xFF);

    if (status != HAL_OK)
        return status;

    // Step 2: Read 8-bit data
    status = PCF8574_Read(data);

    return status;
}

void PCF8574_Test(void)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_IsDeviceReady(
        &hi2c1,
        PCF8574_ADDR,
        3,
        100
    );

    if (status == HAL_OK)
    {
        printf("PCF8574 FOUND!\r\n");
    }
    else
    {
        printf("PCF8574 NOT FOUND!\r\n");
        printf("I2C ERROR: 0x%lX\r\n", hi2c1.ErrorCode);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* MCU Configuration */

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_I2C1_Init();

    MX_USART1_UART_Init();


    /* USER CODE BEGIN 2 */

    printf("\r\n");
    printf("========================\r\n");
    printf("SYSTEM START\r\n");
    printf("========================\r\n");

    // Scan I2C devices
    I2C_Scan();

    HAL_Delay(100);

    // Test PCF8574
    PCF8574_Test();

    HAL_Delay(100);

    // Initialize PCF8574
    if (PCF8574_Init() == HAL_OK)
    {
        printf("PCF8574 INIT SUCCESS!\r\n");
    }
    else
    {
        printf("PCF8574 INIT FAILED!\r\n");
    }

    HAL_Delay(100);


    /* USER CODE END 2 */


    /* Infinite loop */

    while (1)
    {
        /* USER CODE BEGIN 3 */

        i2c_status = DIP_Read(&dip_raw);

        if (i2c_status == HAL_OK)
        {
            // DIP switch ON = 1
            // DIP switch OFF = 0

            dip_status = (uint8_t)(~dip_raw);

            printf("\r\n");

            printf("RAW DATA: 0x%02X\r\n", dip_raw);

            printf("DIP STATUS: 0x%02X\r\n", dip_status);

            printf("DIP1=%d ", (dip_status >> 0) & 1);
            printf("DIP2=%d ", (dip_status >> 1) & 1);
            printf("DIP3=%d ", (dip_status >> 2) & 1);
            printf("DIP4=%d ", (dip_status >> 3) & 1);

            printf("DIP5=%d ", (dip_status >> 4) & 1);
            printf("DIP6=%d ", (dip_status >> 5) & 1);
            printf("DIP7=%d ", (dip_status >> 6) & 1);
            printf("DIP8=%d\r\n", (dip_status >> 7) & 1);
        }
        else
        {
            printf("I2C READ ERROR: 0x%lX\r\n",
                   hi2c1.ErrorCode);
        }

        HAL_Delay(500);

        /* USER CODE END 3 */
    }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
