/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"

/* USER CODE BEGIN 0 */
#include <stdlib.h>
#include "LiquidCrystal.h"

extern int score;
extern int speed;
extern int length;
extern int snake[80][2];
int headRowChange = 0;
int headColChange = 1;
int snakeHeadRow=0;
int snakeHeadCol=2;
extern int appleRow;
extern int appleColumn;
extern char prev;
extern int totalTime;
extern float vol;
extern float snakeSpeed;
int flag=0;
int startCounter=-1;
extern int mode;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
int reset = 0;
int pause = 1;
int prev2 = 0;
int prevA = 0;
int prev4 = 0;
int prev5 = 0;
int prev6 = 0;
int prevB = 0;
int prevC = 0;

typedef unsigned char byte;
byte top[8] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x00,
  0x00,
  0x00,
  0x00
};

byte bot[8] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

byte full[8] = { 
	0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

byte topApple[8] = { 
  0x04,
  0x0A,
  0x0A,
  0x04,
  0x00,
  0x00,
  0x00,
  0x00
};

byte botApple[8] = { 
  0x00,
  0x00,
  0x00,
  0x00,
  0x04,
  0x0A,
  0x0A,
  0x04
};

byte snakeAppleTop[8] = {
  0x04,
  0x0A,
  0x0A,
  0x04,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

byte snakeAppleBot[8] = {
	 0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x04,
  0x0A,
  0x0A,
  0x04
 
};

byte heart[8] = {
  0x00,
  0x0A,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x00
};


int findRow(int r){
	switch (r){
		case 0:
		case 1:
			return 0;
			break;
		case 2:
		case 3:
			return 1;
			break;
		case 4:
		case 5:
			return 2;
			break;
		case 6:
		case 7:
			return 3;
			break;	
	}
}

void generateRandomFruit(){
	int flag1 = 0;
	int random = ((int)__HAL_TIM_GET_COUNTER(&htim2) )+ totalTime ;
	srand(random);
	appleRow = rand() % 8;
	appleColumn = rand() % 20;
	//check if apple hit the snake
	for(int i = 0 ; i<length ; i++){
		if(snake[i][0] == appleRow && snake[i][1] == appleColumn ) flag = 1;
	}
		if(flag == 0) {
			setCursor(appleColumn,findRow(appleRow));
			if(appleRow%2==0) write(5);
			else write(6);
	}
		else{ 
	int random = ((int)__HAL_TIM_GET_COUNTER(&htim2) )+ totalTime ;
	srand(random);
	appleRow = rand() % 8;
	appleColumn = rand() % 20;
		}
}

int eatFruit(){
	if(snake[0][0]== appleColumn && snake[0][1] == appleRow )
		return 1;
	else return 0;
}

void headVSFruit(){
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 ,0);
	if(snake[0][0]== appleColumn){//same column
		if(snake[0][1] < appleRow) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13,1);//below		
		else HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9,1); //above
	}
	
	else if(snake[0][1]== appleRow){//same row
		if(snake[0][0] > appleColumn) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15,1);//left		
		else HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11,1); //right
	}
	
	else if(snake[0][0] < appleColumn){
		if(snake[0][1] > appleRow) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10,1);//right up	
		else HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12,1); //right down
	}
	
	else{
		if(snake[0][1] > appleRow) HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8,1);//left	up	
		else HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14,1); //left down
	}	
}

int checkLose(int head1, int head2){
	int lose= 0;
	if(length>3){
	for(int i = 1; i < length; i++){
		if(head1== snake[i][0]&& head2== snake[i][1])
			lose = 1;
	}
}
	return lose;
}

int AIcheckLose(int head1, int head2, int change1, int change2){
	int lose= 0;
	if(length>3){
		for(int i = 1; i < length; i++){
			if(head1== snake[i][0]+change1&& head2== snake[i][1]+change2)
				lose = 1;
		}
	}
	return lose;
}

void printSnake(int tailCol,int tailRow)
{ 
	//tail
	if(tailCol == appleColumn && findRow(tailRow) == findRow(appleRow)){
			setCursor(tailCol,findRow(tailRow));	
			if(appleRow%2==0) write(5);
			else write(6);
		}
	else{
		setCursor(tailCol,findRow(tailRow));
		print(" ");
	}
	
	if(findRow(snake[length-1][1]) == findRow(tailRow) && headRowChange!=0 ) {
		setCursor(snake[length-1][0],findRow(snake[length-1][1]));
		if ((snake[length-1][1])%2==0) write(2); 
					else write(3);
	}
	
	
	//head
	int t=0;
	setCursor(snake[0][0],findRow(snake[0][1]));
	if(headRowChange!=0){//up/down
	if(findRow(snake[0][1]) == findRow(snake[1][1]) ) 
			write(4);
	else if ((snake[0][1])%2==0) write(2); 
	else write(3);
}
	else{//forward backwaed
		for(int i = 1 ; i< length; i++){
			if(snake[i][0]==snake[0][0] && findRow(snake[0][1])==findRow(snake[i][1]))
				t = 1;
		}
		if(t==1) write(4);
		else if(appleColumn==snake[0][0] && findRow(snake[0][1])==findRow(appleRow)){
			if ((snake[0][1])%2==0) write(8); 
			else write(7);
		}
		else if ((snake[0][1])%2==0) write(2); 
	else write(3);
	}
	
		
} 


void initial(){
	length = 3;
	//head
	snake[0][0]= 2;
	snake[0][1]= 0;
	//body
	snake[1][0]= 1;
	snake[1][1]= 0;
	//tail
	snake[2][0]= 0;
	snake[2][1]= 0;
	
	createChar(2,top);
	setCursor(0,0);				
	write(2);
	setCursor(1,0);	
	write(2);
	setCursor(2,0);	
	write(2);
	
	createChar(3,bot);
	createChar(4,full);
	createChar(5,topApple);
	createChar(6,botApple);
	createChar(8,snakeAppleBot);
	createChar(7,snakeAppleTop);
	
}

void numbertobcd(int i){
	int x1=i&1;
	int x2=i&2;
	int x3=i&4;
	int x4=i&8;
	if(x1>0)
		x1=1;
	if(x2>0)
		x2=1;
	if(x3>0)
		x3=1;
	if(x4>0)
		x4=1;
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,x1);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,x2);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,x3);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,x4);
}
int change = 0;
void gameOver(){
	reset = 1;
	clear();
	setCursor(0,1);
	print("Game Over!");
	setCursor(0,2);
	print("score: ");
	int finalScore = (score/change)*10+(length-3)*100;
	setCursor(7,2);
	char ss[20];
	sprintf(ss,"%d",finalScore);
	print(ss);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Stop_IT(&htim4);
}

void AI(){

	if(appleRow > snake[0][1] && prev!= 'u' && AIcheckLose(snake[0][0],snake[0][1]+1,0,1)==0) {//down		
			headColChange = 0;
			headRowChange = 1;
			prev = 'd';		
	}
	
	else if(appleRow < snake[0][1] && prev!= 'd' && AIcheckLose(snake[0][0],snake[0][1]-1,0,-1)==0) {//up
			headColChange = 0;
			headRowChange = -1;
			prev = 'u';
	}
		else if(appleColumn < snake[0][0] && prev!= 'r' && AIcheckLose(snake[0][0]-1,snake[0][1],-1,0)==0) {//left
			headColChange = -1;
			headRowChange = 0;
			prev = 'l';
	}
	
	else if(appleColumn > snake[0][0] && prev!= 'l' && AIcheckLose(snake[0][0]+1,snake[0][1],1,0)==0) {//right
			headColChange = 1;
			headRowChange = 0;
			prev = 'r';
	}
	
}
int sevenSeg = 0;


/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line0 interrupt.
*/
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */
	for(int i = 0; i < 3; i++){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
		if(i==0){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
		if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0)){
			//2 up
			if(HAL_GetTick()> prev2+250){
				prev2 = HAL_GetTick();
				if(mode == 1){
					if(prev == 'd') gameOver();
					headColChange = 0;
					headRowChange = -1;
					prev = 'u';
				}
			}
		}
	}
		if(i==1){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
			if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0)){
			  //A mode
			  if(HAL_GetTick()> prevA+250){
				prevA = HAL_GetTick();
				if(mode == 1) mode = 2;
				else mode = 1;
		 }
		}	
	}
		
}
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
		
  /* USER CODE END EXTI0_IRQn 1 */
}

/**
* @brief This function handles EXTI line1 interrupt.
*/
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */

  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI1_IRQn 1 */
		for(int i = 0; i < 4; i++){
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);			
				if(i == 0){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
					if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)){
						//4 left
						if(HAL_GetTick()> prev4+250){
							prev4 = HAL_GetTick();
							if(mode == 1){
								if(prev == 'r') gameOver();
								headColChange = -1;
								headRowChange = 0;
								prev = 'l';
							}
						}
					}
				}
				
				if(i == 1){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
					if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)){
						//5 down
						if(HAL_GetTick()> prev5+250){
							prev5 = HAL_GetTick();
							if(mode == 1){
								if(prev == 'u') gameOver();
								headColChange = 0;
								headRowChange = 1;
								prev = 'd';	
							}
						}
					}						
				}	
					
				if(i== 2) {
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_SET);
					if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)){
					//6 right
						if(HAL_GetTick()> prev6+250){
							prev6 = HAL_GetTick();
							if(mode == 1){
								if(prev == 'l') gameOver();
								headColChange= 1;
								headRowChange = 0;
								prev = 'r';
							}
						}
					}
				}	

			else{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
					if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)){
						//B reset
						if(HAL_GetTick()> prevB+250){
							prevB = HAL_GetTick();
							if(reset == 1)
							HAL_NVIC_SystemReset();
						}
					}
				}				
			}
		
	
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
		
  /* USER CODE END EXTI1_IRQn 1 */
}

/**
* @brief This function handles EXTI line2 and Touch Sense controller.
*/
void EXTI2_TSC_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_TSC_IRQn 0 */

  /* USER CODE END EXTI2_TSC_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_TSC_IRQn 1 */
		for(int i = 0; i < 1; i++){
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);			
				if(i == 0){
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
					if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2)){
						//C pause
						if(HAL_GetTick()> prevC+250){
							prevC = HAL_GetTick();
							if(pause == 1) {
								pause = 0;
							}
							else pause = 1;
						}
					}
				}					
		}
	
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
		//while(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2));
  /* USER CODE END EXTI2_TSC_IRQn 1 */
}

/**
* @brief This function handles EXTI line3 interrupt.
*/
void EXTI3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI3_IRQn 0 */

  /* USER CODE END EXTI3_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI3_IRQn 1 */

  /* USER CODE END EXTI3_IRQn 1 */
}

/**
* @brief This function handles ADC1 and ADC2 interrupts.
*/
void ADC1_2_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_2_IRQn 0 */

  /* USER CODE END ADC1_2_IRQn 0 */
  HAL_ADC_IRQHandler(&hadc2);
  /* USER CODE BEGIN ADC1_2_IRQn 1 */

	int v = ((HAL_ADC_GetValue(&hadc2)*6)/63);
	switch (v) {
		case 0: 
			vol = 0.5;
			break;
		case 1: 
			vol = 1;
			break;
		case 2: 
			vol = 1.5;
			break;
		case 3: 
			vol = 2;
			break;
		case 4: 
			vol = 2.5;
			break;
		case 5: 
			vol = 3;
			break;
		
	}
		
	HAL_ADC_Start_IT(&hadc2);
  /* USER CODE END ADC1_2_IRQn 1 */
}

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */
	int s =(snakeSpeed >vol? vol : snakeSpeed)*10;
	if(sevenSeg == 0){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,1);
		numbertobcd(s/10);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,0);	
		sevenSeg++;		
	}
	else if(sevenSeg == 1){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,1);
		numbertobcd(s%10);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,0);	
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,1);//decimal point
		sevenSeg++;
	}
	else if(sevenSeg == 2){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7,1);
		numbertobcd(length/10);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,0);	
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,0);//decimal point
		sevenSeg++;
	}
	else if(sevenSeg == 3){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,1);
		numbertobcd(length%10);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,0);		
		sevenSeg=0;
	}
		
  /* USER CODE END TIM2_IRQn 1 */
}

/**
* @brief This function handles TIM3 global interrupt.
*/
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */
	scrollDisplayRight();
	if(flag == 0) {noDisplay(); flag = 1;}
	else {display(); flag = 0;}
  /* USER CODE END TIM3_IRQn 1 */
}

/**
* @brief This function handles TIM4 global interrupt.
*/
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */
	
	speed =((1/(snakeSpeed >vol? vol : snakeSpeed))*10)*pause;
		
	if(totalTime <100 && totalTime%10==0) {
	startCounter++;
	setCursor(5,2);
	print("Snake Game");	
	setCursor(11,3);
	print("  ");
	setCursor(10,3);
	char start[4];
	sprintf(start,"%d",10-startCounter);
	print(start);
		
	if(flag==0) {
	createChar(10,heart);
	setCursor(6,1);
	write(10);
	setCursor(7,1);		
	print("Welcome");
	setCursor(14,1);
	write(10);
	flag=1;		
	}
	else{
		setCursor(6,1);
		print("           ");
		flag=0;		
	}		
}
	
	if(appleRow == -1 && totalTime == 100){
		setCursor(7,1);
		print("         ");
		setCursor(5,2);
		print("            ");	
		setCursor(10,3);
		print("  ");
		initial();
		generateRandomFruit();
	}
	
	if(totalTime>100) {
		
		if(totalTime%speed==0){
		display();
		int tailRow = snake[length-1][1];
		int tailCol = snake[length-1][0];
			
		if(mode == 2)AI();
		//print head
		snakeHeadRow=snake[0][1] + headRowChange;
		snakeHeadCol=snake[0][0] + headColChange;
		
		//set snake's new cordinates
		for(int i = length-1; i>0; i--) {
			snake[i][0]= snake[i-1][0]; //col
			snake[i][1]= snake[i-1][1]; //row
		}
		//no border
		if(snakeHeadCol == -1) snakeHeadCol = 19;
		if(snakeHeadCol == 20) snakeHeadCol = 0;
		if(snakeHeadRow == -1) snakeHeadRow = 7;
		if(snakeHeadRow == 8) snakeHeadRow = 0;
		
		snake[0][0] = snakeHeadCol;
		snake[0][1] = snakeHeadRow;
		
		if(eatFruit()){
			snake[length][0] = tailCol;
			snake[length][1] = tailRow;
			setCursor(tailCol, findRow(tailRow));
			if(tailRow%2==0) write(2);
			else write(3);
			length++;
			generateRandomFruit();
			snakeSpeed += 0.5;
			noDisplay();	
		}
		printSnake(tailCol,tailRow);
		if(checkLose(snake[0][0],snake[0][1]) && mode==1) gameOver();	
		headVSFruit();
		score += (snakeSpeed >vol? vol : snakeSpeed)*10;
		change++;
	}
	
}	
	//if(totalTime==300) storeData();
	totalTime++;

  /* USER CODE END TIM4_IRQn 1 */
}

/**
* @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
*/
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	extern unsigned char dir[1];
	switch(dir[0]){
		case 'W': //up
		case 'w':
			if(mode == 1){
				if(prev == 'd') gameOver();
				headColChange = 0;
				headRowChange = -1;
				prev = 'u';
			}
		break;
		case 'S': //down
		case 's':	
			if(mode == 1){
				if(prev == 'u') gameOver();
				headColChange = 0;
				headRowChange = 1;
				prev = 'd';
			}
		break;
		case 'A': //left
		case 'a':
			if(mode == 1){
				if(prev == 'r') gameOver();
				headColChange = -1;
				headRowChange = 0;
				prev = 'l';
	}
		break;
		case 'D': //right
		case 'd':	
			if(mode == 1){
				if(prev == 'l') gameOver();		
				headColChange= 1;
				headRowChange = 0;
				prev = 'r';
		}
		break;	
		case 'M': //mode
		case 'm':	
			if(mode== 1) mode = 2;
			else mode = 1;		
		break;
		case 'R': //reset
		case 'r':	
			HAL_NVIC_SystemReset();		
		break;
		case 'P': //pause
		case 'p':	
			if(pause == 1){ 
				pause = 0;
			}
		  else pause = 1;			
		break;
	}
	
	HAL_UART_Receive_IT(&huart2, dir, sizeof(dir));	
  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
