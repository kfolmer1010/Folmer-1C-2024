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


/*Escribir una función que reciba como parámetro un dígito BCD y un vector 
de estructuras del tipo gpioConf_t. Incluya el archivo de cabecera gpio_mcu.h
*/

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void functionBCD(uint8_t digit, gpioConf_t *vector){
	for(int i=0; i<4; i++){
		GPIOInit(vector[i].pin, vector[i].dir);
	}
	
	for (int i=0; i < 4; i++){
		if((digit&(1<<i))==0)//multiplico digit x1 
		{
			GPIOOff(vector[i].pin);
		}
		else 
			GPIOOn(vector[i].pin);
 	}	
}

void app_main(void){
	gpioConf_t vector_pines[4]={{GPIO_20,GPIO_OUTPUT}, 
						{GPIO_21,GPIO_OUTPUT}, 
						{GPIO_22,GPIO_OUTPUT}, 
						{GPIO_23,GPIO_OUTPUT}};
	functionBCD(3, vector_pines);
}
/*==================[end of file]============================================*/