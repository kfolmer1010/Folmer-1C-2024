/*! @mainpage Guia_2_EJ4
 *
 * @section genDesc General Description
 *Primero digitaliza una señal analógica y la transmite a un graficador de puerto serie de la PC. Despues se convierte una señal digital de un 
 *ECG en una señal analógica para visualizarla utilizando el osciloscopio.
 * 
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 25/04/2024 | Document creation		                         |
 *
 * @author Karen Folmer (karenfolmer@hotmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "hc_sr04.h"
#include "lcditse0803.h"
#include <led.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
/**Defino el periodo al que se va a convertir la señal analogica a digital.*/
#define CONFIG_BLINK_PERIOD 2000

/**Defino cada cuanto quiero mandar la data que recibo por CH.*/
#define CONFIG_BLINK_PERIOD_MUESTRAS 4329//1/231

TaskHandle_t convadc_task_handle = NULL;
TaskHandle_t medir_task_handle = NULL;

/**Defino el tamaño de la muestra*/
#define BUFFER_SIZE 231

/**Guarda los datos que leo en el CH.*/
uint16_t data;

int i = 0;
/*==================[internal data definition]===============================*/
/**Es un arreglo que guarda los datos del ecg.*/
const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};
/*==================[internal functions declaration]=========================*/
/**
 * @fn static void FuncTimer(void *pvParameter);
 * @brief Notifica cada tarea cuando debe ser interrumpida.
 * @param[in] void *pvParameter
 * @return 
*/
void FuncTimer(void* param);

/**
 * @fn static void ConvADCTask(void *pvParameter)
 * @brief Lee el dato que esta en el canal y lo convierte a ASCII
 * @param[in] void *pvParameter
 * @return 
*/
static void ConvADCTask(void *pvParameter);

/**
 * @fn static void FuncTimer(void *pvParameter);
 * @brief Escribe la muestra del ecg cada un determinado periodo.
 * @param[in] void *pvParameter
 * @return 
*/
void FuncTimerOut(void* param);

/*==================[external functions definition]==========================*/
void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(convadc_task_handle, pdFALSE);
}

static void ConvADCTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		AnalogInputReadSingle(CH1, &data);
		UartSendString(UART_PC, (char*)UartItoa(data, 10));
		UartSendString(UART_PC, " \r");
	}  	
}

void FuncTimerOut(void *param){
	if(i<BUFFER_SIZE){
		AnalogOutputWrite(ecg[i]);
		i++; 
	}
	else{
		i=0;
	}
}

void app_main(void){
	analog_input_config_t analog = {
		.input = CH1, //le paso el canal
		.mode = ADC_SINGLE,//el modo en el que va a operar
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0
	};
	AnalogInputInit(&analog);
	AnalogOutputInit();
	timer_config_t timer_1 = {
			.timer = TIMER_A,
			.period = CONFIG_BLINK_PERIOD,
			.func_p = FuncTimer,
			.param_p = NULL
	};
	TimerInit(&timer_1);

	timer_config_t timer_2 = {
			.timer = TIMER_B,
			.period = CONFIG_BLINK_PERIOD_MUESTRAS,
			.func_p = FuncTimerOut,
			.param_p = NULL
	};
	TimerInit(&timer_2);

	serial_config_t my_uart = {
		.port = UART_PC, 
		.baud_rate = 38400, 
		.func_p = NULL, 
		.param_p = NULL
	};
	UartInit(&my_uart);

	xTaskCreate(&ConvADCTask, "convADC", 512, NULL, 4, &convadc_task_handle);
	TimerStart(timer_1.timer);
	TimerStart(timer_2.timer);	
}
/*==================[end of file]============================================*/