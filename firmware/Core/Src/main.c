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
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "usbd_cdc_if.h"
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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
void myprintf(const char *fmt, ...);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void startup(void);
void read_sensor(void);
void sleep(void);
void update_display(void);
void sdcard_write(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t last_activity_time = 0;
uint8_t current_state = 0; // 0: Menu, 1: Temp, 2: Hum, 3: Light
uint8_t menu_selection = 0;
uint32_t last_interrupt_time = 0;

float current_temp = 0.0f;
float current_hum = 0.0f;
float current_light = 0.0f;

void myprintf(const char *fmt, ...) {
  static char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  int len = strlen(buffer);
  CDC_Transmit_FS((uint8_t*)buffer, len);

}
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  startup();
  last_activity_time = HAL_GetTick();
  uint32_t last_log_time = HAL_GetTick(); // Timer to track SD card writes

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // 1. Check for 2-minute inactivity timeout (120,000 ms)
	        if ((HAL_GetTick() - last_activity_time) > 120000) {
	            sleep();

	            // --- MCU wakes up and resumes execution here ---
	            // Reset the timer so it doesn't immediately go back to sleep
	            last_activity_time = HAL_GetTick();
	        }

	        // 2. Poll the I2C sensors for data on demand
	        read_sensor();

	        // 3. Update the ST7735 screen based on the current state machine
	        update_display();

	        // 4. Log to SD card every 10 seconds (10,000 ms)
	        if ((HAL_GetTick() - last_log_time) > 10000) {
	                    sdcard_write();
	                    last_log_time = HAL_GetTick(); // Reset the timer
	                }
	        // 4. Small delay to prevent screen flickering and bus overload
	        HAL_Delay(100);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DC_Pin|BLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI_CS2_Pin|SPI_CS1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Display_RES_GPIO_Port, Display_RES_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA3 PA10 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3|GPIO_PIN_10|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ROT1_Pin ROT2_Pin */
  GPIO_InitStruct.Pin = ROT1_Pin|ROT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : DC_Pin */
  GPIO_InitStruct.Pin = DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW1_Pin */
  GPIO_InitStruct.Pin = SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB12 PB13
                           PB14 PB15 PB4 PB5
                           PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI_CS2_Pin SPI_CS1_Pin */
  GPIO_InitStruct.Pin = SPI_CS2_Pin|SPI_CS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BLK_Pin Display_RES_Pin */
  GPIO_InitStruct.Pin = BLK_Pin|Display_RES_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t current_time = HAL_GetTick();

	    // Ignore bounces faster than 50ms
	    if ((current_time - last_interrupt_time) > 50) {
	        last_activity_time = current_time;
	        last_interrupt_time = current_time;

	        if (GPIO_Pin == SW1_Pin) {
	            if (current_state == 0) current_state = 1;
	            else current_state = 0;
	            myprintf("Button Clicked! New State: %d\r\n", current_state);
	        }
	        else if (GPIO_Pin == ROT1_Pin) {
	            if (current_state == 0) {
	                if (HAL_GPIO_ReadPin(GPIOA, ROT2_Pin) == GPIO_PIN_SET) {
	                    if (menu_selection < 2) menu_selection++;
	                } else {
	                    if (menu_selection > 0) menu_selection--;
	                }
	                myprintf("Encoder Turned! Selection: %d\r\n", menu_selection);
	            }
	        }
	    }
}

void startup(void){

		// 1. Turn on the display backlight
	    HAL_GPIO_WritePin(BLK_GPIO_Port, BLK_Pin, GPIO_PIN_SET);

	    // 2. HARDWARE RESET: Pull low for 50ms, then pull high for 50ms
	    HAL_GPIO_WritePin(Display_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
	    HAL_Delay(50);
	    HAL_GPIO_WritePin(Display_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
	    HAL_Delay(50);

	    myprintf("\r\n--- SYSTEM BOOTING ---\r\n");
	    myprintf("Initializing TFT Display...\r\n");
	    // 3. Initialize the ST7735 Display
	    ST7735_Init();
	    ST7735_FillScreen(ST7735_BLACK);
	    // 4. Configure the OPT3001 Light Sensor
	    // It boots in shutdown mode. We write 0xC810 to the Configuration Register (0x01)
	    // to set it to Continuous Conversion mode with an 800ms integration time.
	    // Note: Assuming I2C address 0x88 (0x44 shifted left by 1).
	    myprintf("Configuring OPT3001 Light Sensor...\r\n");

	    uint8_t opt_config[3] = {0x01, 0xC8, 0x10};
	    HAL_I2C_Master_Transmit(&hi2c1, 0x8A, opt_config, 3, HAL_MAX_DELAY);
	    myprintf("Light sensor configured successfully!\r\n");
	    // 5. Mount the MicroSD Card
	    // The '1' at the end forces FatFs to mount immediately so we can catch errors early.
	    // (You will uncomment this once FatFs is fully enabled in CubeMX)
	    // if (f_mount(&SDFatFs, "", 1) != FR_OK) {
	    //     // Optional: Draw an "SD Error" message on the screen if it fails
}

void read_sensor(void){
		uint8_t sht_data[6];
	    uint8_t opt_data[2];

	    // Command 0x2400: High repeatability measurement, clock stretching disabled
	    uint8_t sht_cmd[2] = {0x24, 0x00};

	    // =========================================================================
	    // 1. SHT3x-DIS (Temperature & Humidity)
	    // Hardware: ADDR to GND -> 0x44 -> Shifted for HAL: 0x88
	    // =========================================================================

	    // Send the measurement command
	    HAL_I2C_Master_Transmit(&hi2c1, 0x88, sht_cmd, 2, HAL_MAX_DELAY);

	    // Wait for the measurement to complete (High repeatability takes max 15ms)
	    HAL_Delay(15);

	    // Read the 6 bytes: Temp MSB, Temp LSB, Temp CRC, Hum MSB, Hum LSB, Hum CRC
	    if (HAL_I2C_Master_Receive(&hi2c1, 0x88, sht_data, 6, HAL_MAX_DELAY) == HAL_OK) {

	        // Combine MSB and LSB into 16-bit raw integers
	        uint16_t raw_temp = (sht_data[0] << 8) | sht_data[1];
	        uint16_t raw_hum = (sht_data[3] << 8) | sht_data[4];

	        // Apply mathematical conversion formulas
	        current_temp = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);
	        current_hum = 100.0f * ((float)raw_hum / 65535.0f);
	    }else {
	        myprintf("[ERROR] SHT3x Read Failed!\r\n");
	    }

	    // =========================================================================
	    // 2. OPT3001-Q1 (Ambient Light)
	    // Hardware: ADDR to 3.3V -> 0x45 -> Shifted for HAL: 0x8A
	    // =========================================================================

	    // Because we set continuous conversion in startup(), we simply read the Result Register (0x00)
	    if (HAL_I2C_Mem_Read(&hi2c1, 0x8A, 0x00, I2C_MEMADD_SIZE_8BIT, opt_data, 2, HAL_MAX_DELAY) == HAL_OK) {

	        // Combine MSB and LSB into a 16-bit raw integer
	        uint16_t raw_light = (opt_data[0] << 8) | opt_data[1];

	        // Extract the 4-bit Exponent and 12-bit Fractional result using bitwise masking
	        uint16_t exponent = (raw_light & 0xF000) >> 12;
	        uint16_t fractional = raw_light & 0x0FFF;

	        // Calculate final Lux value using bitwise shifting for the 2^E exponent math
	        current_light = 0.01f * (1 << exponent) * fractional;
	    }else {
	        myprintf("[ERROR] OPT3001 Read Failed!\r\n");
	    }
}

void sleep(void){
	// Let us know over USB that it's going to sleep
	    myprintf("\r\nEntering STOP mode.\r\n");
	    HAL_Delay(10); // Crucial: Give the USB buffer a split second to flush out the text before halting clocks!

		// --- 1. PREPARE FOR SLEEP ---
		    // Turn off the display backlight to save battery
		    HAL_GPIO_WritePin(BLK_GPIO_Port, BLK_Pin, GPIO_PIN_RESET);

		    // Suspend the 1ms system timer.
		    // If we don't do this, the SysTick interrupt will wake the MCU up every 1 millisecond!
		    HAL_SuspendTick();

		    // --- 2. ENTER STOP MODE ---
		    // The processor stops executing code exactly at this line.
		    // It will remain asleep here until a hardware interrupt (EXTI) occurs.
		    // Because you configured SW1_Pin and ROT1_Pin as EXTI in CubeMX,
		    // pressing or turning the encoder will automatically wake it up.
		    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

		    // =========================================================
		    //               MCU IS ASLEEP HERE
		    // =========================================================

		    // --- 3. WAKE UP SEQUENCE ---
		    // Code execution resumes here immediately after the EXTI interrupt fires.

		    // STOP mode disables the main oscillator (HSE) and the PLL to save power.
		    // We must reconfigure the clocks before doing anything else, or the MCU will run very slowly.
		    SystemClock_Config();

		    // Resume the 1ms system timer so HAL_Delay() and HAL_GetTick() work again
		    HAL_ResumeTick();

		    // Turn the display backlight back on
		    HAL_GPIO_WritePin(BLK_GPIO_Port, BLK_Pin, GPIO_PIN_SET);

	    // Let us know over USB that it successfully woke up and restored its clocks
	    myprintf("\r\nWoke up! Clocks restored.\r\n");
}

void update_display(void){

	char buffer[32]; // Buffer to hold our formatted text strings

	    if (current_state == 0) {
	        // --- MAIN MENU ---
	        // Highlight the selected option by making it GREEN, others WHITE
	        uint16_t color0 = (menu_selection == 0) ? ST7735_GREEN : ST7735_WHITE;
	        uint16_t color1 = (menu_selection == 1) ? ST7735_GREEN : ST7735_WHITE;
	        uint16_t color2 = (menu_selection == 2) ? ST7735_GREEN : ST7735_WHITE;

	        // Draw the menu text (assuming Font_7x10 exists in your fonts.h)
	        ST7735_WriteString(10, 20, "1. Temperature", Font_7x10, color0, ST7735_BLACK);
	        ST7735_WriteString(10, 40, "2. Humidity   ", Font_7x10, color1, ST7735_BLACK);
	        ST7735_WriteString(10, 60, "3. Light      ", Font_7x10, color2, ST7735_BLACK);

	    } else if (current_state == 1) {
	        // --- DATA VIEW ---
	        // We add extra spaces at the end of the strings to overwrite any leftover pixels
	        // from previous longer numbers, acting as a quick way to clear the line.
	        if (menu_selection == 0) {
	        	int t_int = (int)current_temp;
	        	int t_dec = (int)(current_temp * 100) % 100;
	        	if(t_dec < 0) t_dec = -t_dec; // Handle negative temps
	        	sprintf(buffer, "Temp: %d.%02d C  ", t_int, t_dec);
	            ST7735_WriteString(10, 40, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);
	        }
	        else if (menu_selection == 1) {
	        	int h_int = (int)current_hum;
	        	int h_dec = (int)(current_hum * 100) % 100;
	        	sprintf(buffer, "Hum: %d.%02d %%   ", h_int, h_dec);
	            ST7735_WriteString(10, 40, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);
	        }
	        else if (menu_selection == 2) {
	        	sprintf(buffer, "Light: %d lx  ", (int)current_light);
	            ST7735_WriteString(10, 40, buffer, Font_7x10, ST7735_WHITE, ST7735_BLACK);
	        }
	    }
}


void sdcard_write(void){
    FATFS FatFs;
    FIL fil;
    FRESULT fres;
    char log_buffer[100]; // Buffer to hold our formatted string
    UINT bytesWrote;

    // 1. Mount the file system
    fres = f_mount(&FatFs, "", 1);
    if (fres != FR_OK) {
        myprintf("SD Mount error: %i\r\n", fres);
        return; // Stop here if mount fails
    }

    // 2. Open the file (FA_OPEN_ALWAYS creates it if it doesn't exist)
    fres = f_open(&fil, "data.txt", FA_WRITE | FA_OPEN_ALWAYS);

    if(fres == FR_OK) {
        // *** THE FIX: Move the "cursor" to the end of the file so we don't overwrite! ***
        f_lseek(&fil, f_size(&fil));

        // 3. Format the sensor data into a text string
        int t_int = (int)current_temp;
        int t_dec = (int)(current_temp * 100) % 100;
        if(t_dec < 0) t_dec = -t_dec;
        int h_int = (int)current_hum;
        int h_dec = (int)(current_hum * 100) % 100;

        sprintf(log_buffer, "Temp: %d.%02d C, Hum: %d.%02d %%, Light: %d lx\r\n", t_int, t_dec, h_int, h_dec, (int)current_light);

        // 4. Write the string to the SD card
        fres = f_write(&fil, log_buffer, strlen(log_buffer), &bytesWrote);

        if(fres == FR_OK) {
            myprintf("Logged to SD: %s", log_buffer); // Echo to USB for debugging
        } else {
            myprintf("SD Write error: %i\r\n", fres);
        }

        // 5. Close the file to finalize and save the data to the silicon
        f_close(&fil);
    } else {
        myprintf("SD Open error: %i\r\n", fres);
    }

    // 6. Unmount the drive (Important to prevent corruption if power is lost)
    f_mount(NULL, "", 0);
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
