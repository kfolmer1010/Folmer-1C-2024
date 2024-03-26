/*! @mainpage Template
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
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
#define ON 1
#define OFF 0
#define TOGGLE 3
#define CONFIG_BLINK_PERIOD 100
/*==================[internal data definition]===============================*/
struct leds
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;        //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
} my_leds; 
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

void ControlLeds(struct leds *leds_ptr)
{
	switch (leds_ptr->mode)
	{
	case ON:
		if (leds_ptr->n_led == LED_1){
			LedOn(LED_1);
		}
			
		else if(leds_ptr->n_led == LED_2){
				LedOn(LED_2);
		}
		else if(leds_ptr->n_led == LED_3){
				LedOn(LED_3);
		}	
		
		break;
	
	case OFF:
		if(leds_ptr->n_led == LED_1)
				LedOff(LED_1);
			else if(leds_ptr->n_led == LED_2)
				LedOff(LED_2);
			else if(leds_ptr->n_led == LED_3)
				LedOff(LED_3);
		
		break;

	case TOGGLE:
		for(int i=0; i<leds_ptr->n_ciclos; i++){
			if(leds_ptr->n_led == LED_1)
				LedToggle(LED_1);
			else if(leds_ptr->n_led == LED_2)
				LedToggle(LED_2);
			else if(leds_ptr->n_led == LED_3)
				LedToggle(LED_3);
			for(int j=0; j<leds_ptr->periodo; j++){
						vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);

			}
		}			
		break;
	}
}


void app_main(void){
	LedsInit();
	my_leds.mode = 3;
	my_leds.n_led = LED_1;
	my_leds.n_ciclos = 10;
	my_leds.periodo = 5;
	ControlLeds(&my_leds);
}




