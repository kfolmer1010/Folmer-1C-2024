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
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"

#define CONFIG_BLINK_PERIOD 1000 //macro: es como una etiqueta que establece un valor. No se puede modificar,
								 // por lo tanto no es una variable


void app_main(void){
	uint8_t teclas; //entero de 8 bits, puede ser de 16 o 32 bits
	LedsInit();
 	SwitchesInit();
	while(1)    {
     	teclas  = SwitchesRead();
     	switch(teclas){
     		case SWITCH_1:
				LedOn(LED_1);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				LedOff(LED_1);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    		break;
     		
			case SWITCH_2:
				LedOn(LED_2);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				LedOff(LED_2);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
     		break;

			case SWITCH_1 | SWITCH_2: //SWITCH_1 or SWITCH_2
				LedOn(LED_3);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
				LedOff(LED_3);
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);			
			break;
     	}

	}
}


/*==================[end of file]============================================*/