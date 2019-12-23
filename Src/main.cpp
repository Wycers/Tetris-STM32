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
#include <vector>
#include <cstring>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef unsigned int uint;
typedef unsigned char uchar;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_X 22
#define MAX_Y 13
#define MAX_C 7
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int cur_frame = 0;
int map[MAX_X][MAX_Y] = {0};
int curx, cury;
int nextbox[2];
int score, key_value;
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


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void rotate();

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

int newfall() {
    curx = 0;
    cury = 4;

    curbox = block[nextbox[0]];
    rebuidNext();
    return test(curx, cury, curbox);
}

void dis_map() {
    for (int i = 0; i < MAX_X; i++)
        for (int j = 0; j < MAX_Y; j++) {
            if (map[i][j])
                mainPanel.draw(i, j, YELLOW);
            else
                mainPanel.draw(i, j, BLACK);
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

void putBox() {
    for (int x = 3; x >= 0; x--)
        for (int y = 3; y >= 0; y--)
            if (curbox.get(x, y))
                map[x + curx][y + cury] = curbox.get(x, y);
}

int drop() {
    int newx = curx + 1;
    if (test(newx, cury, curbox)) {
        curx = newx;
        return 1;
    }
    return 0;
}

void move(int dir) {
    int newy = dir ? cury + 1 : cury - 1;
    if (test(curx, newy, curbox)) {
        cury = newy;
    }
}

void rotate() {
    box newbox = box(&curbox);
    if (test(curx, cury, newbox))
        curbox = newbox;
}

void fall() {
    while (drop());
}

void Get_scroce() {
    LCD_ShowNum(180, 160, score, 5, 16);
    LCD_ShowNum(180, 230, level, 2, 16);
}

int clear_line() {
    int flag, cnt = 0;
    for (int x = 1; x < MAX_X - 1; ++x) {
        flag = 1;
        for (int y = 1; y < MAX_Y - 1; ++y) {
            if (!map[x][y]) {
                flag = 0;
                break;
            }
        }
        if (flag) {
            for (int dx = x; dx > 0; --dx) {
                for (int dy = 1; dy < MAX_Y - 1; ++dy)
                    map[dx][dy] = map[dx - 1][dy];
            }
            for (int dy = 1; dy < MAX_Y - 1; ++dy)
                map[0][dy] = 0;
            ++cnt;
        }
    }
    return cnt;
}

void display_clear() {
    memset(map, 0, sizeof map);
    for (auto &x : map)
        x[0] = x[MAX_Y - 1] = 1;
    for (int y = 0; y < MAX_Y; ++y)
        map[MAX_X - 1][y] = 1;
}

extern "C" {
uint8_t rxBuffer[20];

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData,
                                      uint16_t Size);

void USART1_IRQHandler(void) {
    /* USER CODE BEGIN USART1_IRQn 0 */
    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */
    HAL_UART_Receive_IT(&huart1, (uint8_t *) rxBuffer, 1);
    /* USER CODE END USART1_IRQn 1 */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        static unsigned char uRx_Data[1024] = {0};
        static unsigned char uLength = 0;

        if (rxBuffer[0] == '\n') {
//            HAL_UART_Transmit(&huart1, (uint8_t *) uRx_Data, strlen((const char *) uRx_Data), 0xffff);
            uRx_Data[uLength - 1] = '\0';
            uLength = 0;
            if (strcmp((const char *) uRx_Data, "l") == 0) {
                move(0);
                return;
            }
            if (strcmp((const char *) uRx_Data, "r") == 0) {
                move(1);
                return;
            }
            if (strcmp((const char *) uRx_Data, "u") == 0) {
                rotate();
                return;
            }
            if (strcmp((const char *) uRx_Data, "d") == 0) {
                fall();
                return;
            }
        } else {
            uRx_Data[uLength] = rxBuffer[0];
            uLength++;
        }
    }
}

}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main() {
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
    HAL_UART_Receive_IT(&huart1, (uint8_t *) rxBuffer, 1);
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
    display_clear();
    LCD_DrawRectangle(5, 280, 235, 315);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    for (;;) {
        POINT_COLOR = RED;
        LCD_ShowString(15, 290, 200, 16, 16, (uint8_t *) "Press any key.... ");
        key_value = 0;
        while (key_value == 0);
        score = 0;
        level = 0;
        LCD_ShowString(15, 290, 200, 16, 16, (uint8_t *) "                  ");
        rebuidNext();
        display_clear();
        dis_map();
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        LCD_ShowString(15, 281, 200, 16, 16, (uint8_t *) "                  ");
        LCD_ShowString(15, 298, 200, 16, 16, (uint8_t *) "                  ");
        for (;;) {
            if (!newfall()) {
                POINT_COLOR = RED;
                LCD_ShowString(15, 290, 200, 16, 16, (uint8_t *) "Game over!");
                key_value = 0;
                while (key_value == 0);
                break;
            }
            Get_scroce();
            dis_next_box();
            for (;;) {
                if (!drop()) {
                    putBox();
                    int cnt = clear_line();
                    score += cnt * 233;
                    level = std::min(5, score / 500);
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
void SystemClock_Config() {
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
