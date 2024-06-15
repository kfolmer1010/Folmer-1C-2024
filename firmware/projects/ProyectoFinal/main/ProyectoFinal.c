/*! @mainpage ProyectoFinal
 *
 * @section genDesc General Description
 *
 * Trabajo integrador de la materia. Consiste en un alcoholímetro que mide el nivel de alcohol en ppm. 
 * Cuando el nivel de alcohol sea mayor a cierto umbral se activará una alarma y se mantendrá tapado el orificio donde se introduce la llave para arrancar el auto.
 * Si el nivel de alcohol es menor a cierto umbral se abre la tapa para introducir la llave y arrancar el auto.
 * 
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	SERVO	 	| 	GPIO_19		|
 * | 	BUZZER	 	| 	GPIO_21  	|
 * | 	DETECTOR 	| 	GPIO_3		|
 * |  SENSOR MQ-3 	| 	CH1	    	|
 * |  VENTILADOR	| 	CH2		    |
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 09/06/2024 | Document creation		                         |
 *
 * @author Karen Folmer (karen.folmer@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "servo_sg90.h"
#include "sensorMQ3.h"
#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "buzzer.h"
#include "analog_io_mcu.h"
#include "ble_mcu.h"
#include "string.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
/**Defino el periodo en el que se realizaran las interrupciones de las tareas. */
#define CONFIG_BLINK_PERIOD 1000000
/**Periodo al que quiero que lea lo que le mando por las teclas.*/
#define CONFIG_BLINK_PERIOD_TECLAS 200
/**Defino el GPIO donde conectare el servo. */
#define GPIO_SERVO GPIO_19
/**Defino el GPIO donde conectare el Buzzer. */
#define GPIO_BUZZER GPIO_21
/**Defino el GPIO donde conectare el Detector de línea. */
#define GPIO_DETECTOR GPIO_3
/**Declaro la tarea para detectar si la llave está puesta o no. */
TaskHandle_t detectorllave_task_handle = NULL;
/**Declaro la tarea para hacer sonar la alarma */
TaskHandle_t alarmasound_task_handle = NULL;
/**Declaro la tarea para leer el valor de voltaje del ventilador y sensar el nivel de alcohol. */
TaskHandle_t readvalue_task_handle = NULL;
/**Declaro la tarea para mandarle la data a la app. */
TaskHandle_t senddata_task_handle = NULL;
/*==================[internal data definition]===============================*/
/**Defino el angulo inicial de la tapa que va a mover el servo. */
uint8_t ANG_SERVO_INICIAL = 0;
/**Defino el angulo final, hasta donde el servo va a abrir la tapa. */
uint8_t ANG_SERVO_FINAL = 120;
/**Variable que almacena el nivel de alcohol en ppm. */
float conc_alcohol;
/**Variable que se utiliza despues para calibrar Ro del sensor MQ-3.  */
uint16_t Ro = 0;
/**Si tapaOpen sea true se abrirá la tapa. */
volatile bool tapaOpen;
/**Cuando es true suena la alarma */
volatile bool alarma;
/**Almacena el valor de voltaje del ventilador. */
uint16_t valor;
/**Guarda el mensaje para mandar a la app. */
char msg [50];
/**guarda el valor de la tecla que se presiona */
uint8_t teclas;
/** */
uint8_t data[10];
/*==================[internal functions declaration]=========================*/
/**
 * @fn static void AlarmaSoundTask()
 * @brief Es una tarea que activa una alarma cada vez que se supera el umbral de alcohol.
 * 
 */
static void AlarmaSoundTask();

/**
 * @fn void TapaMove()
 * @brief Mueve la tapa con el servo cuando el umbral de alcohol es menor al umbral.
 * 
 */
void TapaMove();

/**
 * @fn void ReadVoltageValueTask()
 * @brief Lee el valor de voltaje del ventilador y sensa el nivel de alcohol. Le manda información a TapaMove() y a AlarmaSoundTask() dependiendo el caso.
 * 
 */
void ReadValueTask();

/**
 * @fn void DetectorLlaveTask()
 * @brief Tarea para detectar si está puesta la llave o no, para saber si cerrar la tapa o no.
 * 
 */
void DetectorLlaveTask();

/**
 *  @fn void SendDataTask()
 *  @brief Le envía los datos de nivel de alcohol a la app.
 * 
 */
void SendDataTask();

/**
 * @fn void FuncTimer(void* param)
 * @brief Notifica a las tareas cuando deben ser interrumpidas.
 * @param void* param
 */
void FuncTimer(void* param);

/**
 * @fn void LeerTeclas();
 * @brief Determina las acciones a realizar si se apreta una tecla u otra.
 * 
 */
void LeerTeclas();

/**
 * @fn void ReadData(uint8_t * data, uint8_t length)
 * @brief Lee la información que le paso por la app para prender o apagar la alarma.
 * @param uint8_t * data 
 * @param uint8_t length

 */
void ReadData(char * data, uint8_t length);

/*==================[external functions definition]==========================*/
static void AlarmaSoundTask()
{
	while(true)
	{
		if(conc_alcohol>200 && alarma == true)
		{
			GPIOOn(GPIO_BUZZER);
			BuzzerOn();
			BuzzerSetFrec(1000);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			BuzzerOff();
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}	
}

void TapaMove()
{
	if(tapaOpen == true)
	{
		ServoMove(SERVO_0, ANG_SERVO_FINAL);
	}
	else
	{
		ServoMove(SERVO_0, ANG_SERVO_INICIAL);
	} 
}

void ReadValueTask()
{
	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		AnalogInputReadSingle(CH2, &valor);
		conc_alcohol = MQGetPercentage(MQRead()/Ro);
		if(conc_alcohol<=200 && valor>200)
		{
			tapaOpen = true;
			TapaMove();
			alarma = false;
		}
		else if(conc_alcohol>200 && valor>200)
		{
			tapaOpen = false;
			TapaMove();
			alarma = true;
		}
	}
}

void DetectorLlaveTask()
{
	while (true)
	{
			vTaskDelay(5000 / portTICK_PERIOD_MS);
			if(GPIORead(GPIO_DETECTOR))
			{
				tapaOpen = true;
				TapaMove();
			}
			else
			{
				tapaOpen = false;	
				TapaMove();
			} 
	}
}

void SendDataTask()
{
	while(true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		if(valor>200)
		{
			strcpy(msg,"");
			sprintf(msg, "*aNivel de alcohol[ppm]: %f\n", conc_alcohol);
			BleSendString(msg);
		}
	}	
} 

void ReadData(char * data, uint8_t length)
{
    switch(data[0])
	{
        case 'A':
            alarma = true;
            break;
        case 'a':
            alarma = false;
            break;
    }
}

void FuncTimer(void* param)
{
	vTaskNotifyGiveFromISR(readvalue_task_handle, pdFALSE);
	vTaskNotifyGiveFromISR(senddata_task_handle, pdFALSE);
}

void LeerTeclas()
{
	while(1)
	{
     	teclas  = SwitchesRead();
     	switch(teclas)
		{
     		case SWITCH_1:
				alarma = false;
			break;
		}
		vTaskDelay(CONFIG_BLINK_PERIOD_TECLAS / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	analog_input_config_t my_ad_x = 
	{
		.input = CH2, //le paso el canal
		.mode = ADC_SINGLE,//el modo en el que va a operar
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0
	};
	AnalogInputInit(&my_ad_x);

	GPIOInit(GPIO_SERVO, GPIO_OUTPUT);
	GPIOInit(GPIO_BUZZER, GPIO_OUTPUT);
	GPIOInit(GPIO_DETECTOR, GPIO_INPUT);

	ServoInit(SERVO_0, GPIO_SERVO);
	ServoMove(SERVO_0, ANG_SERVO_INICIAL);
	BuzzerInit(GPIO_BUZZER);

    ble_config_t ble_configuration = 
	{
        .device_name = "EDU_KARU",
		.func_p = ReadData
    };
    BleInit(&ble_configuration);

	timer_config_t timer_1 = 
	{
			.timer = TIMER_A,
			.period = CONFIG_BLINK_PERIOD,
			.func_p = FuncTimer,
			.param_p = NULL
	};
	TimerInit(&timer_1);


	MQInit();
	Ro = MQCalibration();
	
	xTaskCreate(&DetectorLlaveTask, "DetectorLlaveTask", 2048, NULL, 4, &detectorllave_task_handle);
	xTaskCreate(&AlarmaSoundTask, "AlarmaSoundTask", 2048, NULL, 4, &alarmasound_task_handle);
	
	xTaskCreate(&ReadValueTask, "ReadVoltageValueTask", 2048, NULL, 4, &readvalue_task_handle);
	xTaskCreate(&SendDataTask, "SendDataTask", 2048, NULL, 4, &senddata_task_handle);

	TimerStart(timer_1.timer);
}
/*==================[end of file]============================================*/