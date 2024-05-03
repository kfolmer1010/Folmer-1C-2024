/*! @mainpage Guia_2_EJ2
 *
 * @section genDesc General Description
 *Este programa tiene como función leer la distancia que mide un sensor y mostrarla por display. Ademas cambia los leds que estan prendidos o
 *apagados dependiendo que valor tiene esa distancia. Por otro lado tambien realiza determinadas acciones según apreto una tecla u otra.
 *Todas estas funciones se realizan a traves de tareas y a un determinado periodo, definido con un timer.
 *
 * 
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
 * | 11/04/2024 | Document creation		                         |
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

/*==================[macros and definitions]=================================*/
/**La variable d almacena el valor que mide de distancia.*/
uint16_t d;

/**Si es TRUE prende el display, si es FALSE lo apaga.*/
bool on;

/**Si es TRUE congela el valor que se muestra por display.*/
bool hold; 

/**Periodo en el que se realizaran las tareas.*/
#define CONFIG_BLINK_PERIOD 1000000

TaskHandle_t medir_task_handle = NULL;
TaskHandle_t leds_task_handle = NULL;
TaskHandle_t display_task_handle = NULL;
/*==================[internal data definition]===============================*/
/*==================[internal functions declaration]=========================*/
/**
 * @fn static void FuncTimer(void *pvParameter);
 * @brief Notifica cada tarea cuando debe ser interrumpida.
 * @param[in] void *pvParameter
 * @return 
*/
void FuncTimer(void* param);

/**
 * @fn static void MedirTask(void *pvParameter);
 * @brief Tarea para medir la distancia con el sensor, la guarda en centimetros en una variable.
 * @param[in] void *pvParameter
 * @return 
*/
void MedirTask(void *pvParameter);

/**
 * @fn static void LedsTask(void *pvParameter);
 * @brief Tarea para prender o apagar los leds dependiendo que distancia estè midiendo el sensor.
 * @param[in] void *pvParameter
 * @return 
*/
void LedsTask(void *pvParameter);


/**
 * @fn static void DisplayTask(void *pvParameter);
 * @brief Tarea que escribe en el display las distancias que mide si está en ON, o apaga el display si esta !ON.
 * @param[in] void *pvParameter
 * @return 
*/
void DisplayTask(void *pvParameter);


/**
 * @fn static void CambiarEstado();
 * @brief Si le paso 'O' prende el display o lo apaga.
 * @param[in] 
 * @return 
*/
void CambiarEstado();

/**
 * @fn static void Congelar();
 * @brief Si se pasa H por consola deja fijo el valor que se veia por display, hasta que se vuelva a apretar H.
 * @param[in] 
 * @return 
*/
void Congelar();


/*==================[external functions definition]==========================*/
void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(leds_task_handle, pdFALSE);//le manda una notif a la tarea
	vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);
    vTaskNotifyGiveFromISR(display_task_handle, pdFALSE);
}

void MedirTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		printf("Medir\n");
		if(on == true){
			d = HcSr04ReadDistanceInCentimeters();
		}
	}
}

void LedsTask(void *pvParameter){
	while(1){
    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);    
		printf("Leds\n");
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

void DisplayTask(void *pvParameter){
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

void CambiarEstado(){
	on=!on;
}

void Congelar(){
	hold=!hold;
}

void app_main(void){
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	LedsOffAll();
	SwitchesInit();
	LcdItsE0803Init();

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

	TimerStart(timer_1.timer);
}
/*==================[end of file]============================================*/