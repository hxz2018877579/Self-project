
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "oled.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "servo.h"
#include "config.h"
#include "stdio.h"
#include "openmv.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "esp01s.h"
#include "Onenet.h"
#include "MQTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern uint8_t center_x ,center_y ,color_type ;
unsigned short timeCount = 0;	//发送间隔变量
unsigned char *dataPtr = NULL;
extern double center_x_cm,center_y_cm;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
    MX_TIM3_Init();
    MX_USART3_UART_Init();
    MX_USART1_UART_Init();
		  ESP01S_Init();  //8266初始化
			  while(OneNet_DevLink())  //接入onenet
					 HAL_Delay(500);
    ESP01S_Clear();    //
     OLED_Init();
     OLED_Clear();
    
    /* USER CODE BEGIN 2 */
    HAL_UART_Receive_IT(&huart3,(void *)&USART3_RXbuff,1);
		
	  pwm_start();

    servo_reset_begin();

    int temp_x = center_x,temp_y = center_y;
    char temp_color = color_type; 

    while (1)
    {
			OLED_ShowCHinese(0,0,7);//垃圾智能分拣
			OLED_ShowCHinese(16,0,8);
			OLED_ShowCHinese(32,0,9);
			OLED_ShowCHinese(48,0,10);
			OLED_ShowCHinese(64,0,11);
			OLED_ShowCHinese(80,0,12);
		
        temp_x = center_x,temp_y = center_y,temp_color = color_type;
        HAL_Delay(200);
        coordinate_transformation(temp_x,temp_y);
        
        
        if(temp_color == 'B')
        {
            blue_task();
		 OLED_Part_Clear(3) ;
		OLED_Part_Clear(4) ;			
        }
        
        else if(temp_color == 'Y')
        {
            yellow_task();
			OLED_Part_Clear(3) ;
					OLED_Part_Clear(4) ;
        }
        
        else
        {   
            servo_back();
         //   continue;
        }
        temp_color = 'A'; 
       
				
				
				
		 if(++timeCount >= 80)									//上传数据约3s一次
		{
             
			
	        OneNet_SendData( );			 
			timeCount = 0;
			ESP01S_Clear( );
		}
		
        
		  dataPtr = ESP01S_GetIPD(3);//完成需要15个毫秒，三次循环，一次5个毫秒       
		  if(dataPtr != NULL)
          {                         
	      OneNet_RevPro(dataPtr);
          }
		  HAL_Delay(10); 
counters++;
				
//            pwm_out(0,0,0,0,0,90);
//            HAL_Delay(1000);
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
//		HSI：High Speed Internal，高速内部时钟，RC振荡器；
//HSE：High Speed External,高速外部时钟，可接石英/陶瓷谐振器，或者接外部时钟源；
//LSI：Low Speed Internal，低速内部时钟，RC振荡器，频率为37kHz；
//LSE：Low Speed External，低速外部时钟，接频率为32.768kHz的石英晶体；
//PLL：Phase Locked Loop，锁相环倍频分频输出。

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
