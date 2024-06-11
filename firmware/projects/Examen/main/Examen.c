/*! @mainpage Examen
 *
 * @section genDesc General Description
 *
 * El siguiente programa fue implementado con motivo de la realización del parcial de Electrónica Programable.
 * El mismo consiste en un sistema de control de pH y humedad. 
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
 * | 11/06/2024 | Document creation		                         |
 *
 * @author Karen Folmer (karen.folmer@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"
#include "analog_io_mcu.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_SENSADO 3000000
#define CONFIG_BLINK_PERIOD_INFORME 5000000

#define GPIO_BOMBA_AGUA GPIO_15
#define GPIO_BOMBA_PHA GPIO_16
#define GPIO_BOMBA_PHB GPIO_17
#define GPIO_SENSOR_RIEGO GPIO_18
#define GPIO_SENSOR_PH GPIO_19

/*==================[internal data definition]===============================*/
uint16_t valor_sensado, data; 
/*==================[internal functions declaration]=========================*/
void ValorSensado()
{
	AnalogInputReadSingle(CH1, &valor_sensado);
	data = valor_sensado*14/3; 
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	
}
/*==================[end of file]============================================*/