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
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/
#define CANT_DIGITS 3
uint8_t arreglo[CANT_DIGITS]; 

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

gpioConf_t vector_pines[4]={{GPIO_20,GPIO_OUTPUT}, 
						{GPIO_21,GPIO_OUTPUT}, 
						{GPIO_22,GPIO_OUTPUT}, 
						{GPIO_23,GPIO_OUTPUT}};
						
gpioConf_t vector_puertos[3]={{GPIO_19,GPIO_OUTPUT}, 
						{GPIO_18,GPIO_OUTPUT}, 
						{GPIO_9,GPIO_OUTPUT}};			
/*==================[internal functions declaration]=========================*/


/*==================[external functions definition]==========================*/
void  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for(int i=digits-1; i>=0; i--)
    {
        bcd_number[i] = data % 10; //guardo el 6 en el arreglo
        data/=10;//divido x10 para obtener 45 y seguir recorriendo el array
    }
}

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

void mostrarEnDisplay(uint32_t data, uint8_t digits, gpioConf_t *vec_pines, gpioConf_t *vec_puertos){
	for(uint8_t i=0; i<2; i++){
			GPIOInit(vec_puertos[i].pin,vec_puertos[i].dir);
	}

	convertToBcdArray(data, digits, arreglo);

	for(int i=0; i<digits; i++){
		functionBCD (arreglo[i], vec_pines);
		GPIOOn(vec_puertos[i].pin);
		GPIOOff(vec_puertos[i].pin);
	}	
}

void app_main(void){
	mostrarEnDisplay(101, CANT_DIGITS, vector_pines, vector_puertos);	
}
/*==================[end of file]============================================*/