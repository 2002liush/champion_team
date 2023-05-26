/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_AMP 100    //1000mv
#define DEFAULT_FRE 0.5
#define DEFAULT_BIAS 1500
#define DEFAULT_DUTY 0.1
#define MAX_VOLTAGE 3300
#define MAX_FRE 10000
#define MAX_DUTY 0.9
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t lastTime=0;
int wave = 0; 														//select wave shape, 0 is sin wave; 1 is square wave; 2 is rectangle wave.
int functions = 0; 												//select functions, 0 is frequency; 1 is amplitude; 2 is duty cycle.
double hz = 1000; 											//Ƶ��ֵ����λHZ
int amplitude = 1000;										//����ֵ����λmv
 																									
uint16_t sindds[200]={0};							//��Ų������ݵĻ�����
		
double duty_cycle = DEFAULT_DUTY;			//ռ�ձ�
int dots = 0;

int current = 0;
int previous = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//����sin����һ�����ڵ�ֵ
void dds_calculate_sin(int A,int dot_num){
	
	int i = 0;
	float val = 0;
	for(i=0;i<200;i++)
	{
		sindds[i]=0;
	}
	double inc = 2 * 3.1415926 / dot_num;
	for(i=0;i<dot_num;i++)
	{
		val = (1 + sin(inc * i)) * A / 3300.0 * 2048;
		if(val>4095)
			val = 4095;
		sindds[i] = val;
	}
}
//���㷽��һ�����ڵ�ֵ
void dds_calculate_square(int A, int dot_num){
	uint8_t i = 0;
	for(; i< (uint8_t)(dot_num / 2.0); i++){
		sindds[i] = A / 3300.0 * 4095;
	}
	for(; i<dot_num; i++){
		sindds[i] = 0;
	}
}
//������β�һ�����ڵ�ֵ
void dds_calculate_rectangle(int A, int dot_num, double duty_cycle){
	uint8_t i = 0;
	int positive = (int)(duty_cycle * dot_num);
	for(; i<positive; i++){
		sindds[i] = A / 3300.0 * 4095;
	}
	for(; i<dot_num; i++){
		sindds[i] = 0;
	}
}


//�������ö�ʱ��
//���ڸı�Ƶ��ʱ��Ҫ�������ö�ʱ�������ڴӶ��ı�Ƶ��
//arr:��װ��ֵ��psc:Ԥ��Ƶϵ��
void dac_dma_wave_enable(uint16_t arr, uint16_t psc)
{
 TIM_HandleTypeDef tim6_handle = {0};
 TIM_MasterConfigTypeDef master_config = {0};
 __HAL_RCC_TIM6_CLK_ENABLE(); /* TIM7 ʱ��ʹ�� */
 tim6_handle.Instance = TIM6; /* ѡ��ʱ�� 7 */
 tim6_handle.Init.Prescaler = psc; /* ��Ƶϵ�� */
 tim6_handle.Init.CounterMode = TIM_COUNTERMODE_UP; /* ���ϼ��� */
 tim6_handle.Init.Period = arr; /* ��װ��ֵ */
 tim6_handle.Init.AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_ENABLE;/*�Զ���װ*/
 HAL_TIM_Base_Init(&tim6_handle); /* ��ʼ����ʱ�� 7 */
 master_config.MasterOutputTrigger = TIM_TRGO_UPDATE;
 master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
 /* ���� TIM7 TRGO */
 HAL_TIMEx_MasterConfigSynchronization(&tim6_handle, &master_config);
 HAL_TIM_Base_Start(&tim6_handle); /* ʹ�ܶ�ʱ�� 7 */
 /* ��ֹ֮ͣǰ�Ĵ��� */
 HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
}

//���ı�Ƶ���Լ���ֵʱ��Ҫ��������DMA��������Ҫ��������������ݳ��ȵ�
void reset_DMA()
{
	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);//����ͣ��ʱ���Լ�DMA�Ĵ���
	HAL_TIM_Base_Stop(&htim6);						
	if(hz<1000 && hz >=1 )								//��Ƶ�ʵ�1-1000HZʱ��Ҫ�ı䶨ʱ����Ԥ��Ƶϵ��
	{
		dac_dma_wave_enable(5-1,(int)(1000.0 / hz * 25 - 1));
		dots = 200;
	}
	else if(hz < 1)												//��Ƶ��С��1HZʱ�����ı�Ԥ��Ƶϵ�����һ�Ҫ�ı���װ��ֵ
	{

		dac_dma_wave_enable((int)((1.0 / hz) * 5 - 1),25000-1);
		dots = 200;
	}
	else																	//��Ƶ�ʴ���1000HZʱ���ı䶨ʱ��ֻ�ı������ĸ���
	{
		dac_dma_wave_enable(5-1,25-1);
		dots = 1000.0 / hz * 200;
	}

	HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
	HAL_TIM_Base_Stop(&htim6);
	if(wave==0)														//�жϵ�ǰ���ĸ����δӶ������������ʽ
	{
		dds_calculate_sin(amplitude,dots);
	}
	else if(wave==1)
	{
		dds_calculate_square(amplitude,dots);
	
	}
	else
	{
		dds_calculate_rectangle(amplitude,dots,duty_cycle);
	}
	
	HAL_TIM_Base_Start_IT(&htim6);			//����������ʱ����DMA����
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)sindds, dots, DAC_ALIGN_12B_R);
	
}


//�����ⲿ�жϷ������
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
	 
	if(HAL_GetTick()-lastTime>300)
	{
		current++;															//current��һ��Ϊ������ѭ�������ж�֮ǰ�Ƿ������ж�
		if(GPIO_Pin == key1_pin){ 							//����1ѡ����				
			if(wave == 2){
				wave = 0;
				functions = 0;
			}
			else{
				wave++;
				if(wave == 2){
					functions = 2;
				}
				else{
					functions = 0;
				}
			}
		}
		if(GPIO_Pin == key2_pin){ 							//����2ѡ��Ҫ���ӵ���fuctions:0Ƶ�ʣ�functions:1���ȣ�functions:2ռ�ձ� 			 
			if(functions == 1){
				functions = 0;
			}
			else if(functions == 0){
				functions = 1;
			}
		}
		if(GPIO_Pin == key3_pin){ 							//����3��ÿ��һ������Ƶ��1k������1V��	ռ�ձ�10%		
			if(functions == 0){	                         
				double temp = hz + 1000;
				if(temp > MAX_FRE){
					hz = DEFAULT_FRE;
				}
				else{
					hz = temp;
				}
			}
			
			if(functions == 1){
				double temp = amplitude + 1000;
				if(temp > MAX_VOLTAGE){
					amplitude = DEFAULT_AMP;
				}
				else{
					amplitude = temp;
				}
			}
			if(functions == 2){
				double temp = duty_cycle + 0.1;
				if(temp > MAX_DUTY){
					duty_cycle = DEFAULT_DUTY;
				}
				else{
					duty_cycle = temp;
				}
			}
			
		}
		if(GPIO_Pin == key4_pin){               //����4��ÿ��һ������Ƶ��100HZ������100mV��	ռ�ձ�5%		
			if(functions == 0){
				double temp = hz + 100;
				if(temp > MAX_FRE){
					hz = DEFAULT_FRE;
				}
				else{
					hz = temp;
				}
			}
			if(functions == 1){
				double temp = amplitude + 100;
				if(temp > MAX_VOLTAGE){
					amplitude = DEFAULT_AMP;
				}
				else{
					amplitude = temp;
				}
			}
			if(functions == 2){
				double temp = duty_cycle + 0.05;
				if(temp > MAX_DUTY){
					duty_cycle = DEFAULT_DUTY;
				}
				else{
					duty_cycle = temp;
				}
			}
			reset_DMA();
		}
		if(GPIO_Pin == key5_pin){ 						//����5��ÿ��һ������Ƶ��10HZ������10mV��	ռ�ձ�1%		
			if(functions == 0){
				double temp = hz + 10;
				if(temp > MAX_FRE){
					hz = DEFAULT_FRE;
				}
				else{
					hz = temp;
				}
			}
			if(functions == 1){
				double temp = amplitude + 10;
				if(temp > MAX_VOLTAGE){
					amplitude = DEFAULT_AMP;
				}
				else{
					amplitude = temp;
				}
			}
			if(functions == 2){
				double temp = duty_cycle + 0.01;
				if(temp > MAX_DUTY){
					duty_cycle = DEFAULT_DUTY;
				}
				else{
					duty_cycle = temp;
				}
			}
			reset_DMA();
		}
	}
	lastTime = HAL_GetTick();
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
  MX_DMA_Init();
  MX_TIM6_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start_IT(&htim6);//������ʱ���ж�
	
	dots = 1000.0 / hz * 200;						//�����ĸ���
	dds_calculate_sin(amplitude,dots);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
		if(previous != current)			//�ж�֮ǰ�Ƿ������ж�
		{
			reset_DMA();
			previous = current;
		}
		//dds_calculate_sin(amplitude,dots);
		HAL_TIM_Base_Start_IT(&htim6);//������ʱ���ж�
		//HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
		HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)sindds, dots, DAC_ALIGN_12B_R);
		
		
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
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
