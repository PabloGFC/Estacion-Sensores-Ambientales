#include "cmsis_os.h"
#include "principal.h"

#define MSGQUEUE_OBJECTS 4


RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

MSGQUEUE_MODEM_t modem;
MSGQUEUE_SENSORES_t sensor;
MSGQUEUE_PRINCIPAL_t principal;

char msg[64];
uint8_t puntero = 0;
char buffer[64];
char almacen[128];

/*------------------------------------------------------------------------------
 *      Thread: 'Thread_Principal': Módulo principal del sistema que se encarga
 *      de coordinar todos los demás. Recibe los datos y los reenvía al hilo
 *		del módem.Asimismo, recibe información del modem en función de la conexión.
 *		Decide las acciones a tomar en función del estado del sistema
 *-----------------------------------------------------------------------------*/

osThreadId_t tid_Principal;                        // thread id
osMessageQueueId_t mid_PRINCIPALQueue;
void Init_Principal(void *argument);             // thread function


const osThreadAttr_t Principal_attributes = {
  .name = "Principal",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


const osMessageQueueAttr_t MSG_principal_attributes = {
  .name = "MSG_principal"
};

int Thread_Principal(void) {

	mid_PRINCIPALQueue = osMessageQueueNew (MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_PRINCIPAL_t), &MSG_principal_attributes);

	if(mid_PRINCIPALQueue == NULL){
		return (-1);
	}

	tid_Principal = osThreadNew(Init_Principal, NULL, &Principal_attributes);
    if (tid_Principal == NULL) {
        return (-1);
    }
    Thread_Sensores();
    Thread_Modem();
    return 0;
}

void Init_Principal(void *argument)
{
  /* USER CODE BEGIN 5 */
	osStatus_t status;
	osStatus_t status2;

	MX_GPIO_Init();
	MX_RTC_Init();

  /* Infinite loop */
  for(;;)
  {
	  status = osMessageQueueGet(mid_SENSORESQueue, &sensor, 0U, osWaitForever);
	  if (status == osOK) {
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		  osDelay(200);
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		  Get_RTC_Time_Date();
		  sprintf(msg, "D:%02d/%02d/%02d F:%02d:%02d:%02d - T:%.2f H:%.2f CO2:%.0f\r\n", sDate.Date, sDate.Month, sDate.Year, sTime.Hours, sTime.Minutes, sTime.Seconds, sensor.t, sensor.h, sensor.c);
		  strcpy(principal.info, msg);
		  osMessageQueuePut(mid_PRINCIPALQueue, &principal, 0U, 0U);
		}
	  status2 = osMessageQueueGet(mid_MODEMQueue, &modem, 0U, osWaitForever);
	  if (status2 == osOK && modem.act == 1) {
		  guardaMedidas();
		}
  }
  osThreadYield();
  /* USER CODE END 5 */
}


/* creation of Principal */


void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 LD2_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void Get_RTC_Time_Date(void) {
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void guardaMedidas(){
	strcpy(almacen[puntero], msg);
	if(puntero < 127){
		puntero++;
	}else{
		puntero = 0;
	}
}
