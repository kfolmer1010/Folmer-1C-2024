/*! @mainpage Guia_2_EJ1
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	GPIO_3	 	| 	ECHO		|
 * | 	GPIO_2	 	| 	TRIGGER		|
 * | 	 +5V	 	| 	 +5V		|
 * |  	 GND	 	| 	 GND		|
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 04/04/2023 | Document creation		                         |
 *
 * @author Karen Folmer (karenfolmer@hotmail.com)
 *
 */

/*! @mainpage Guia_2_EJ1
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	GPIO_3	 	| 	ECHO		|
 * | 	GPIO_2	 	| 	TRIGGER		|
 * | 	 +5V	 	| 	 +5V		|
 * |  	 GND	 	| 	 GND		|
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 18/04/2023 | Document creation		                         |
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
/*==================[macros and definitions]=================================*/
/**La variable d almacena el valor que mide de distancia.*/
uint16_t d;

/**Si es TRUE prende el display, si es FALSE lo apaga.*/
bool on;

/**Si es TRUE congela el valor que se muestra por display.*/
bool hold; 

#define CONFIG_BLINK_PERIOD 500000

TaskHandle_t medir_task_handle = NULL;
TaskHandle_t leds_task_handle = NULL;
TaskHandle_t display_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;

/*==================[internal data definition]===============================*/
/*==================[internal functions declaration]=========================*/
/**
 * @fn static void FuncTimer(void *pvParameter);
 * @brief Notifica cada tarea cuando debe ser interrumpida.
 * @param[in] void *pvParameter
 * @return 
*/
static void FuncTimer(void* param);

/**
 * @fn static void MedirTask(void *pvParameter);
 * @brief Tarea para medir la distancia con el sensor, la guarda en centimetros en una variable.
 * @param[in] void *pvParameter
 * @return 
*/
static void MedirTask(void *pvParameter);

/**
 * @fn static void LedsTask(void *pvParameter);
 * @brief Tarea para prender o apagar los leds dependiendo que distancia estè midiendo el sensor.
 * @param[in] void *pvParameter
 * @return 
*/
static void LedsTask(void *pvParameter);

/**
 * @fn static void DisplayTask(void *pvParameter);
 * @brief Tarea que escribe en el display las distancias que mide si está en ON, o apaga el display si esta !ON.
 * @param[in] void *pvParameter
 * @return 
*/
static void DisplayTask(void *pvParameter);

/**
 * @fn static void CambiarEstado();
 * @brief Si le paso 'O' prende el display o lo apaga..
 * @param[in] 
 * @return 
*/
static void CambiarEstado();

/**
 * @fn static void Congelar();
 * @brief Si se pasa H por consola deja fijo ese valor que se ve por display, hasta que se vuelva a apretar H.
 * @param[in] 
 * @return 
*/
static void Congelar();

/**
 * @fn static void LeerPuerto();
 * @brief Interpreta lo que recibe por consola y cambia el estado o congela el display.
 * @param[in] 
 * @return 
*/
static void LeerPuerto();

/**
 * @fn static void EnviarDatos();
 * @brief Le pasa a UART los datos que quiero mostrar en el monitor.
 * @param[in] 
 * @return 
*/
static void EnviarDatos();

/**
 * @fn static void MostrarTask(void *pvParameter);
 * @brief Es una tarea para mostrar por el monitor los datos que está sensando.
 * @param[in] void *pvParameter
 * @return 
*/
static void MostrarTask(void *pvParameter);
/*==================[external functions definition]==========================*/
void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(leds_task_handle, pdFALSE);//le manda una notif a la tarea
	vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);
    vTaskNotifyGiveFromISR(display_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);
}

static void MedirTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		if(on == true){
			d = HcSr04ReadDistanceInCentimeters();
		}
	}
}

static void LedsTask(void *pvParameter){
	while(1){
    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    
		if(on==true){
			if(d<=10){
				LedOff(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			else if(d>10&&d<=20){ 
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			else if(d>20&&d<=30){ 
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}
			else if(d>30){
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}
		}
		else{
			LedsOffAll();			
		}		
	}
}

static void DisplayTask(void *pvParameter){
	while(true){
    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		if(on == true){
			if(hold == false){
				LcdItsE0803Write(d);
			}
		}
		else{
			LcdItsE0803Off();
		} 
	}
}

static void CambiarEstado(){
	on=!on;
}

static void Congelar(){
	hold=!hold;
}

static void LeerPuerto(){
	uint8_t data;
	UartReadByte(UART_PC, &data);
	switch (data)
	{
		case 'O':
			CambiarEstado();
			break;
		case 'H':
			Congelar();
			break;
	}
}

static void EnviarDatos(){
	UartSendString(UART_PC, (char*)UartItoa(d, 10));
	UartSendString(UART_PC, " cm\r\n");
}

static void MostrarTask(void *pvParameter){
	while(true){
    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		if(on == true){
			if(hold == false){
				EnviarDatos();
 			}
		}
		else{
		} 
	}
}

void app_main(void){
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	LedsOffAll();
	SwitchesInit();
	LcdItsE0803Init();

	serial_config_t my_uart = {
		.port = UART_PC, 
		.baud_rate = 9600, 
		.func_p = &LeerPuerto, 
		.param_p = NULL
	};
	UartInit(&my_uart);

	SwitchActivInt(SWITCH_1, CambiarEstado, NULL);
	SwitchActivInt(SWITCH_2, Congelar, NULL);
	
	timer_config_t timer_1 = {
			.timer = TIMER_A,
			.period = CONFIG_BLINK_PERIOD,
			.func_p = FuncTimer,//puntero a la funcion que quiero q se ejecute x interrupcion
			.param_p = NULL
	};
	TimerInit(&timer_1);

	/// creación de las tareas que quiero ejecutar 

	xTaskCreate(&MedirTask, "Medir", 512, NULL, 4, &medir_task_handle);
	xTaskCreate(&LedsTask, "Leds", 512, NULL, 4, &leds_task_handle);
	xTaskCreate(&DisplayTask, "Display", 512, NULL, 4, &display_task_handle);
	xTaskCreate(&MostrarTask, "Mostrar", 512, NULL, 4, &mostrar_task_handle);

	TimerStart(timer_1.timer);
}
/*==================[end of file]============================================*/




