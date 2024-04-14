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

/*==================[macros and definitions]=================================*/
uint16_t d;
uint16_t teclas;
bool on;
bool hold; 
#define CONFIG_BLINK_PERIOD 1000
#define CONFIG_BLINK_PERIOD_TECLAS 200

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void medirDistancia(){
	HcSr04Init(GPIO_3, GPIO_2);
	if(on){
		d=HcSr04ReadDistanceInCentimeters();
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
	else if(on==false){
		LedOff(LED_1);
		LedOff(LED_2);
		LedOff(LED_3);
		LcdItsE0803Off();
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
} 

void actualizarLed(){
	medirDistancia();
	LedsInit();
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

void actualizarDisplay(){
	LcdItsE0803Init();
	LcdItsE0803Write(d);
	vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
}

void leerTeclas(){
	SwitchesInit();
	while(1){
     	teclas  = SwitchesRead();
     	switch(teclas){
     		case SWITCH_1:
				on = !on;
				vTaskDelay(CONFIG_BLINK_PERIOD_TECLAS / portTICK_PERIOD_MS);
			break;
			
			case SWITCH_2:
				hold = !hold;
				vTaskDelay(CONFIG_BLINK_PERIOD_TECLAS / portTICK_PERIOD_MS);
			break;
		}
	}
}

void visualizar(){
	if(on==true){
		actualizarLed();
		if(hold==true){
			vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
		}
		else if(hold==false){
			actualizarDisplay();
		}
	}
	else if(on==false){
		LedOff(LED_1);
		LedOff(LED_2);
		LedOff(LED_3);
		LcdItsE0803Off();
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}

void app_main(void){
	
	
}
/*==================[end of file]============================================*/