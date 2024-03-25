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
/*==================[Actividad]=============================================*/
/*Escriba una función que reciba un dato de 32 bits,  la cantidad de dígitos de 
salida y un puntero a un arreglo donde se almacene los n dígitos. La función deberá 
convertir el dato recibido a BCD, guardando cada uno de los dígitos de salida en el 
arreglo pasado como puntero.
*/

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <gpio_mcu.h>
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for(int i=digits-1; i>=0; i--)
    {
        bcd_number[i] =  data % 10; //guardo el 6 en el arreglo
        data/=10;//divido x10 para obtener 45 y seguir recorriendo el array
    }
}
/*==================[external functions definition]==========================*/
void app_main(void)
{
    uint8_t arreglo[3];
    convertToBcdArray(456, 3, arreglo);
}
/*==================[end of file]============================================*/