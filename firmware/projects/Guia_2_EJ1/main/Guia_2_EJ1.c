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
/**La variable d almacena el valor que mide de distancia.*/
uint16_t d;

/**Si es TRUE prende el display, si es FALSE lo apaga.*/
bool on;

/**Si es TRUE congela el valor que se muestra por display.*/
bool hold; 


#define CONFIG_BLINK_PERIOD 1000
#define CONFIG_BLINK_PERIOD_TECLAS 200

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
/**
 * @fn void mostrarDistancia();
 * @brief Escribe en d lo que sensa y lo muestra por display.
 * @param[in] 
 * @return 
*/
void MostrarDistancia();

/**
 * @fn void actualizarLed();
 * @brief Se encarga de prender o apagar los leds dependiendo que distancia este midiendo el sensor.
 * @param[in] 
 * @return 
*/
void ActualizarLed();

/**
 * @fn void ActualizarDisplay();
 * @brief Escribe el valor que se ve por display.
 * @param[in]
 * @return
*/
void ActualizarDisplay();

/**
 * @fn void LeerTeclas();
 * @brief Determina las acciones a realizar si se apreta una tecla u otra.
 * @param[in]
 * @return
*/
void LeerTeclas();

/**
 * @fn void Visualizar();
 * @brief Si ON es TRUE permite visualizar por display los valores que mide el sensor.
 * @param[in]
 * @return
*/
void Visualizar();
/*==================[external functions definition]==========================*/
void MostrarDistancia(){
	while (1){
		if(on==true){
			d=HcSr04ReadDistanceInCentimeters();
		}
		else if(on==false){
			LedsOffAll();
			//LcdItsE0803Off();
		}
		Visualizar();
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
} 

void ActualizarLed(){
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

void ActualizarDisplay(){
	LcdItsE0803Write(d);
}

void LeerTeclas(){
	while(1){
     	teclas  = SwitchesRead();
     	switch(teclas){
     		case SWITCH_1:
				on = !on;
			break;
			case SWITCH_2:
				hold = !hold;
			break;
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_TECLAS / portTICK_PERIOD_MS);
	}
}

void Visualizar(){
	if(on==true){
		ActualizarLed();
		if(hold==true){
		}
		else if(hold==false){
			ActualizarDisplay();
		}
	}
	else if(on==false){
		LedsOffAll();
		LcdItsE0803Off();
	}
}

void app_main(void){
	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	LedsOffAll();
	SwitchesInit();
	LcdItsE0803Init();
	xTaskCreate(&LeerTeclas, "teclas_task",512, NULL,5,NULL);
	xTaskCreate(&MostrarDistancia, "distancia_task",512, NULL,5,NULL);
}
/*==================[end of file]============================================*/