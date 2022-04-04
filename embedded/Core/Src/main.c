/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
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
DAC_HandleTypeDef hdac1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t SPI_DMA_FL = 0;
uint32_t SPI_DMA_CNT = 1;

/** Timing Flags */
/** Contains previous time to update LCD display. */
int lcdPrevTime = 0;
/** Contains previous time to update LED display. */
int ledPrevTime = 0;
/** Contains previous time to check touch. */
int touchPrevTime = 0;
/** Contains previous time to update ADC display. */
int adcPrevTime = 0;
/** Contains previous sample time to read ADC values. */
int sampleTime = 0;
/** Contains previous time to send data to the GUI. */
int sendTime = 0;

int bufferSize = 100;

/** Voltage buffer */
float vBuffer[2000] = {0};
/** Current buffer */
float iBuffer[2000] = {0};
/** Buffer Index */
int bufferIndex = 0;

/***************** TO BE FINISHED *************************/
Mode prevMode = DC; // ESSENTIAL
float contThreshold = 0.1;
int samplePeriod = 20;
int sendPeriod = 500;
float rZero = 100;
int buffCount = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_DAC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void serialReceiver(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * Callback SPI function.
 *
 * return: (none)
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) // Your TxCpltCallback
{
    SPI_DMA_CNT--;
    if (SPI_DMA_CNT == 0) {
        HAL_SPI_DMAStop(&hspi1);
        SPI_DMA_CNT = 1;
        SPI_DMA_FL = 1;
    }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    MX_DAC1_Init();
    MX_SPI1_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */
    // Intialise peripherals.
    usartInit(&huart2);
    pwmInit(&htim2);

    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    setDACValue(&hdac1, DAC_CHANNEL_1, stm32VoltageToDAC(1));

    ILI9163_init(1); // initialize the display
    XPT2046_init();  // initialize the touch functionality
    ee_init();

    /** Reading contrinuity threshold from flash */
    uint8_t arr2[5];
    ee_read(0, sizeof(arr2), arr2);
    currentCT = atof((char *)arr2);

    /** Raw and converted value from ADC. */
    int16_t rawValue;
    float voltageValue;

    // ESSENTIAL. Needs to be here if were starting in voltage mode
    setRelayMode(RELAY_STATUS_PROTECTED);
    /* USER CODE END 2 */

    // delay to let voltages settle
    HAL_Delay(1000);

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        // track mode changes and init/deinit new modes
        if (prevMode != currentMode) {
            if ((prevMode == RESISTANCE || prevMode == CONTINUITY) &&
                (currentMode != RESISTANCE && currentMode != CONTINUITY)) {
                deinitRMeas(&hdac1);
                samplePeriod = 20;
                if (currentMode == AC) {
                    bufferSize = 2000;
                } else {
                    bufferSize = MAX_BUFFERSIZE;
                }
            }
        } else if ((prevMode != RESISTANCE && prevMode != CONTINUITY) &&
                   (currentMode == RESISTANCE || currentMode == CONTINUITY)) {
            initRMeas(&hdac1);
            samplePeriod = 20;
            zeroStatus = false;
            bufferSize = 100;
        }

        prevMode = currentMode;

        /** Read from ADC */
        if (HAL_GetTick() - adcPrevTime >= 1) {
            adcPrevTime = HAL_GetTick();
            rawValue = ads1115ReadRawADC(&hi2c1, DC_VOLTAGE_CIRCUIT);
            voltageValue = adc1115VoltageValue(rawValue);
            vBuffer[bufferIndex] = voltageValue;

            adcPrevTime = HAL_GetTick();
            rawValue = ads1115ReadRawADC(&hi2c1, RESISTANCE_CIRCUIT);
            voltageValue = adc1115VoltageValue(rawValue);
            iBuffer[bufferIndex++] = voltageValue;

            if (bufferIndex >= bufferSize) {
                buffCount++;
                if (buffCount == 5) {
                    // 5th time buffer is filled
                    adcMax = 0.0;
                    adcMin = 0.0;
                    float vInit = calcVDC(vBuffer);
                    if (vInit < 0.5) {
                        setVZero(vInit);
                    }
                }
                buffCount++;
                bufferIndex = 0;
            }
        }

        // Sample ADC value from buffer.
        if (HAL_GetTick() - sampleTime >= samplePeriod && !holdStatus) {
            sampleTime = HAL_GetTick();

            switch (currentMode) {
            case DC:
                dcOffset = calcVDC(vBuffer);
                adcCurr = dcOffset;
                break;

            case AC:
                adcCurr = calcVRMS(vBuffer);
                dcOffset = calcVDC(vBuffer);
                break;
            case RESISTANCE:
                resistance = calcR(&hdac1, vBuffer, iBuffer);
                // handle zeroing
                if (zeroStatus) {
                    zeroStatus = false;
                    rZero = resistance;
                }
                resistance -= rZero;
                processResistance();
                break;
            case CONTINUITY:
                resistance = calcR(&hdac1, vBuffer, iBuffer) - rZero;
                if (resistance < currentCT) {
                    currentCTStatus = 1;
                } else {
                    currentCTStatus = 0;
                }
                break;
            }

            // This checks if the min and max values have been set.
            if (firstADCFlag == 1) {
                firstADCFlag = 0;
                adcMin = adcCurr;
                adcMax = adcCurr;
            } else { // Min and max already set.
                if (adcCurr > adcMax) {
                    adcMax = adcCurr;
                } else if (adcCurr < adcMin) {
                    adcMin = adcCurr;
                }
            }
        }

        // Sample ADC value from buffer.
        if (HAL_GetTick() - sendTime >= sendPeriod && !holdStatus) {
            sendTime = HAL_GetTick();
            // Sending packet to GUI containing the mode information.
            sendPacket(&huart2, 0);
        }

        // Hold status is activated when hold flag is active.
        if (holdFlag == 1) {
            sendPacket(&huart2, 1);
            holdFlag = 0;
        }

        // Brightness status is activated when brightness is active.
        if (brightnessFlag == 1) {
            sendPacket(&huart2, 2);
            brightnessFlag = 0;
        }

        // Update LCD.
        if (HAL_GetTick() - lcdPrevTime >= 500) {
            lcdPrevTime = HAL_GetTick();
            updateDisplay();
        }

        // Update touch.
        if (HAL_GetTick() - touchPrevTime >= 150) {
            touchPrevTime = HAL_GetTick();
            updateTouch();
        }

        // Process the receive packet.
        if (packetReady == 1) {
            serialReceiver();
        }

        // Toggle LED.
        if (HAL_GetTick() - ledPrevTime >= 1000) {
            ledPrevTime = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOB, LED_PIN);
        }

        // Send Join packet.
        connectGui(&huart2);

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_10;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection =
        RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) !=
        HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief DAC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_DAC1_Init(void) {

    /* USER CODE BEGIN DAC1_Init 0 */

    /* USER CODE END DAC1_Init 0 */

    DAC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN DAC1_Init 1 */

    /* USER CODE END DAC1_Init 1 */
    /** DAC Initialization
     */
    hdac1.Instance = DAC1;
    if (HAL_DAC_Init(&hdac1) != HAL_OK) {
        Error_Handler();
    }
    /** DAC channel OUT1 config
     */
    sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
    sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
    if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN DAC1_Init 2 */

    /* USER CODE END DAC1_Init 2 */
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00707CBB;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) !=
        HAL_OK) {
        Error_Handler();
    }
    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 9;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 39;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) !=
        HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 20;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) !=
        HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
    HAL_TIM_MspPostInit(&htim2);
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB,
                      RELAY_1_PIN | RELAY_2_PIN | LED_PIN | LCD_RST_PIN |
                          LCD_DATA_CS_PIN | RELAY_3_PIN | RELAY_4_PIN,
                      GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, LCD_CS_PIN | TOUCH_CS_PIN, GPIO_PIN_RESET);

    /*Configure GPIO pin : PB0 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : Relay_1_Pin Relay_2_Pin LED_Pin LCD_RST_Pin
                             LCD_DATA_CS_Pin Relay_3_Pin Relay_4_Pin */
    GPIO_InitStruct.Pin = RELAY_1_PIN | RELAY_2_PIN | LED_PIN | LCD_RST_PIN |
                          LCD_DATA_CS_PIN | RELAY_3_PIN | RELAY_4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : LCD_CS_Pin TOUCH_CS_Pin */
    GPIO_InitStruct.Pin = LCD_CS_PIN | TOUCH_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/**
 * Process the packet that is sent to the GUI. Update the continuity threshold,
 * message, brightness level, reset and hold button.
 *
 * return: (none)
 */
void serialReceiver(void) {

    char temp[5] = {0};

    // Checking the correct packet is received.
    if (packetRX[0] != 0x12 && packetRX[0] != 0x13 && packetRX[0] != 0x14) {
        currentMode = packetRX[0] - '0';

        // Hold status.
        if (packetRX[6] == '1') {
            holdStatus = true;
        } else {
            holdStatus = false;
        }

        // Brightness level.
        brightnessLevel = packetRX[7] - '0';
        adjustBrightness();

        // Reset status.
        if (packetRX[8] == '0') {
            resetStatus = false;
        } else if (packetRX[8] == '1') {
            resetStatus = true;
        }

        // Process the continuity threshold.
        memcpy(message, &packetRX[9], 16);
        memcpy(temp, &packetRX[1], 5);
        currentCT = atof(temp);

        ee_writeToRam(0, sizeof(temp), (uint8_t *)temp);
        ee_commit();
    }

    // Set the default to 0.
    packetReady = 0;
}
/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    /* USER CODE BEGIN Callback 0 */

    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */

    /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
