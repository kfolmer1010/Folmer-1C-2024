/*! @mainpage Guia_1_EJ6
 *
 * @section genDesc General Description
 *Este programa consta basicamente de una función, mostrarEnDisplay, que recibe un dato de 32 bits,  
 *la cantidad de dígitos de salida en el display y dos vectores de estructuras del tipo  gpioConf_t. 
 *Uno  de estos vectores mapea los bits y el otro los puertos con el dígito del LCD a donde mostrar un dato.
 *El programa muestra por el display el valor que recibe mostrarEnDisplay.
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	  D1	 	| 	GPIO_20		|
 * | 	  D2	 	| 	GPIO_21		|
 * | 	  D3	 	| 	GPIO_22		|
 * | 	  D4	 	| 	GPIO_23 	|
 * | 	 SEL_1	 	| 	GPIO_19		|
 * | 	 SEL_2	 	| 	GPIO_18		|
 * | 	 SEL_3	 	| 	GPIO_9		|
 * | 	  +5V	 	| 	 +5V		|
 * | 	  GND	 	| 	 GND		|
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/03/2023 | Document creation		                         |
 *
 * @author Karen Folmer (karenfolmer@hotmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/
/**
 * @def CANT_DIGITS
 * @brief Cantidad de digitos que tendra el numero a mostrar por el display.
*/
#define CANT_DIGITS 3

/**Arreglo que almacena un numero, del tamaño de la cantidad de digitos del mismo.*/
uint8_t arreglo[CANT_DIGITS]; 

/*==================[internal data definition]===============================*/
/**
 * @struct almacena el numero de pin del GPIO y dirección del mismo.
 */
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/**Arreglo que mapea los GPIO de los bits.*/
gpioConf_t vector_pines[4]={{GPIO_20,GPIO_OUTPUT}, 
						{GPIO_21,GPIO_OUTPUT}, 
						{GPIO_22,GPIO_OUTPUT}, 
						{GPIO_23,GPIO_OUTPUT}};

/**Arreglo que mapea los GPIO de los puertos*/						
gpioConf_t vector_puertos[3]={{GPIO_19,GPIO_OUTPUT}, 
						{GPIO_18,GPIO_OUTPUT}, 
						{GPIO_9,GPIO_OUTPUT}};			

/*==================[internal functions definition]==========================*/
/**
 * @fn void  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number);
 * @brief Convierte el dato recibido a BCD y guarda cada uno de los dígitos de salida en el arreglo pasado como puntero.
 * @param[in] uint32_t data, uint8_t digits, uint8_t * bcd_number
 * @return 
*/
void  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number);

/**
 * @fn void functionBCD(uint8_t digit, gpioConf_t *vector);
 * @brief Cambia el estado de cada GPIO, a ‘0’ o a ‘1’, según el estado del bit correspondiente en el BCD ingresado.
 * @param[in] uint8_t digit, gpioConf_t *vector
 * @return 
*/
void functionBCD(uint8_t digit, gpioConf_t *vector);

/**
 * @fn void mostrarEnDisplay(uint32_t data, uint8_t digits, gpioConf_t *vec_pines, gpioConf_t *vec_puertos);
 * @brief Muestra por display el valor que recibe.
 * @param[in] uint32_t data, uint8_t digits, gpioConf_t *vec_pines, gpioConf_t *vec_puertos
 * @return 
*/
void mostrarEnDisplay(uint32_t data, uint8_t digits, gpioConf_t *vec_pines, gpioConf_t *vec_puertos);

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