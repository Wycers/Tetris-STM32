/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "wwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "grid.h"
#include "box.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef unsigned int uint;
typedef unsigned char uchar;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_X 13
#define MAX_Y 22
#define MAX_C 7
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int map[MAX_X][MAX_Y] = {0};
int curx, cury;
int nextbox[2];
int scro, key_value;
int level;
box curbox, block[MAX_C] = {
        box({{0, 0, 0, 0},
             {0, 0, 0, 0},
             {1, 1, 1, 1},
             {0, 0, 0, 0}}, RED),
        box({{0, 0, 0, 0},
             {0, 0, 1, 0},
             {0, 1, 1, 1},
             {0, 0, 0, 0}}, GRAY),

        box({{0, 0, 0, 0},
             {0, 1, 1, 0},
             {0, 0, 1, 1},
             {0, 0, 0, 0}}, CYAN),

        box({{0, 0, 0, 0},
             {0, 0, 1, 1},
             {0, 1, 1, 0},
             {0, 0, 0, 0}}, BROWN),

        box({{0, 0, 0, 0},
             {0, 1, 1, 0},
             {0, 0, 1, 0},
             {0, 0, 1, 0}}, GBLUE),

        box({{0, 0, 0, 0},
             {0, 0, 1, 1},
             {0, 0, 1, 0},
             {0, 0, 1, 0}}, GREEN),

        box({{0, 0, 0, 0},
             {0, 1, 1, 0},
             {0, 1, 1, 0},
             {0, 0, 0, 0}}, BRRED),

};

Grid mainPanel, boxPanel[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config();

void rotate();

void move(int dir);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int flag = 0;

void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg) {
    HAL_WWDG_Refresh(hwwdg);
}

void move(int dir);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    HAL_Delay(100);
    switch (GPIO_Pin) {
        case KEY0_Pin:
            if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) {
                move(1);
                key_value = 1;
            }
            break;
        case KEY1_Pin:
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {
                move(0);
                key_value = 1;
            }
            break;
        case KEY_WK_Pin:
            if (HAL_GPIO_ReadPin(KEY_WK_GPIO_Port, KEY_WK_Pin) == GPIO_PIN_SET) {
                rotate();
                key_value = 1;
            }
            break;
        default:
            break;
    }
}

void init() {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    srand(HAL_ADC_GetValue(&hadc1));
    for (int i = 0; i < 10; ++i)
        srand(HAL_ADC_GetValue(&hadc1) + random());

    nextbox[0] = random() % MAX_C;
    nextbox[1] = random() % MAX_C;
}

void rebuidNext() {
    nextbox[0] = nextbox[1];
    nextbox[1] = random() % MAX_C;
}

int test(int mx, int my, box box) {
    int x, y;
    for (x = 3; x >= 0; x--)
        for (y = 3; y >= 0; y--)
            if (map[x + mx][y + my] && box.get(x, y))
                return 0;
    return 1;
}

int newfall(void) {
    int x, y;
    cury = 0;
    curx = 4;

    curbox = block[nextbox[0]];
    rebuidNext();
    return test(curx, cury, curbox);
}

void base_box(int x, int y, int c) {
    if (c == 0)
        mainPanel.draw(x, y, BLACK);
    if (c == 1)
        mainPanel.draw(x, y, RED);
    if (c == 2)
        mainPanel.draw(x, y, YELLOW);
}

void initMap() {
    int x, y;
    for (x = 0; x < MAX_X; x++)
        for (y = 0; y < MAX_Y; y++) {
            if (x < 1 || x > 11 || y > 20)
                map[x][y] = 1;
            else
                map[x][y] = 0;
        }
}

void dis_map() {
    int i, j;
    for (i = 0; i < MAX_X; i++)
        for (j = 0; j < MAX_Y; j++) {
            if (map[i][j])
                base_box(i, j, 2);
            else
                base_box(i, j, 0);
        }
}

void dis_box(int x, int y, box c_box) {
    for (int i = 3; i >= 0; i--) {
        for (int j = 0; j < 4; j++)
            if (c_box.get(i, j))
                mainPanel.draw(x + i, y + j, c_box.get_color());
    }
}

void dis_next_box() {
    int i, j;
    for (int t = 0; t < 2; ++t)
        for (i = 3; i >= 0; i--)
            for (j = 0; j < 4; j++)
                boxPanel[t].draw(i, j, block[nextbox[t]].get(i, j) ? block[nextbox[t]].get_color() : BLACK);
}

void putBox(void) {
    for (int x = 3; x >= 0; x--)
        for (int y = 3; y >= 0; y--)
            if (curbox.get(x, y))
                map[x + curx][y + cury] = curbox.get(x, y);
}

int drop(void) {
    int newy = cury + 1;
    if (test(curx, newy, curbox)) {
        cury = newy;
        return 1;
    }
    return 0;
}

void move(int dir) {
    int newx = dir ? curx + 1 : curx - 1;
    if (test(newx, cury, curbox)) {
        curx = newx;
    }
}

void rotate(void) {
    box newbox = box(&curbox);
    if (test(curx, cury, newbox))
        curbox = newbox;
}

void Get_scroce(void) {
    LCD_ShowNum(180, 160, scro, 5, 16);
    LCD_ShowNum(180, 230, level, 2, 16);
}

void clear_line(void) {
    int x, y;
    int dx, dy;
    int fullflag;
    for (y = 1; y < MAX_Y - 1; y++) {
        fullflag = 1;
        for (x = 1; x < MAX_X - 1; x++) {
            if (!map[x][y]) {
                fullflag = 0;
                break;
            }
        }
        if (fullflag) {
            for (dy = y; dy > 0; dy--)
                for (dx = 1; dx < MAX_X - 1; dx++)
                    map[dx][dy] = map[dx][dy - 1];
            for (dx = 1; dx < MAX_X - 1; dx++)
                map[dx][dy] = 0;
            scro += 100;
            if (scro >= 1000)
                level++;
        }
    }
}

void display_clear() {
    memset(map, 0, sizeof map);
    for (int x = 0; x < MAX_X; x++)
        map[x][MAX_Y - 1] = 1;
    for (int y = 0; y < MAX_Y; y++)
        map[0][y] = map[MAX_X - 1][y] = 1;
}

void display_init() {
    mainPanel = Grid(22, 13, 12, 12);
    mainPanel.set_start_point(10, 10);
    mainPanel.set_background_color(BLACK);
    mainPanel.set_line_color(BLUE);
    mainPanel.display();

    boxPanel[0] = Grid(4, 4, 12, 12);
    boxPanel[0].set_start_point(180, 30);
    boxPanel[0].set_background_color(BLACK);
    boxPanel[0].set_line_color(MAGENTA);
    boxPanel[0].display();

    boxPanel[1] = Grid(4, 4, 12, 12);
    boxPanel[1].set_start_point(180, 100);
    boxPanel[1].set_background_color(BLACK);
    boxPanel[1].set_line_color(MAGENTA);
    boxPanel[1].display();

    POINT_COLOR = BLACK;
    display_clear();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
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
    MX_USART1_UART_Init();
    MX_ADC1_Init();
    MX_TIM1_Init();
    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start(&htim1);

    LCD_Init();
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    POINT_COLOR = BLACK;
//    Font_Init();
    init();
    display_init();
    LCD_DrawRectangle(5, 280, 235, 315);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        POINT_COLOR = RED;
        LCD_ShowString(10, 280, 200, 16, 16, (uint8_t *) "Press any key.... ");
        key_value = 0;
        while (key_value == 0);
        LCD_ShowString(5, 265, 200, 16, 16, (uint8_t *) "                  ");
        rebuidNext();
        display_init();
        dis_map();
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        while (1) {
            if (!newfall()) {
                POINT_COLOR = RED;
                LCD_ShowString(5, 265, 200, 16, 16, (uint8_t *) "The end of the game ");
                LCD_ShowString(5, 298, 200, 16, 16, (uint8_t *) "Press any key.... ");
                LCD_ShowString(5, 281, 200, 16, 16, (uint8_t *) "make persistent efforts!");
                key_value = 0;
                while (key_value == 0);
                break;
            }
            Get_scroce();
            LCD_ShowString(5, 265, 200, 16, 16, (uint8_t *) "                    ");
            LCD_ShowString(5, 298, 200, 16, 16, (uint8_t *) "                  ");
            LCD_ShowString(5, 281, 200, 16, 16, (uint8_t *) "                        ");
            dis_next_box();
            while (1) {
                if (!drop()) {
                    putBox();
                    clear_line();
                    break;
                }
                dis_map();
                dis_box(curx, cury, curbox);
                HAL_Delay((6 - level) * 100);
            }
        }
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

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
