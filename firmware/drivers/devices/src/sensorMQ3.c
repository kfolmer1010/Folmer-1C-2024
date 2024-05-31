
/*==================[inclusions]=============================================*/
#include "sensorMQ3.h"
#include "analog_io_mcu.h"
#include "delay_mcu.h"
#include "math.h"

float concAlcoholCurve[3]  =  {1.74,0.36,-0.65};   //!< arreglo caracteristico de la curva del gas licuado de petroleo. formato {x,y,pendiente}

analog_input_config_t my_conversor = {
		.input = CH1, //le paso el canal
		.mode = ADC_SINGLE,//el modo en el que va a operar
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0
	};

void MQInit()
{
	AnalogInputInit(&my_conversor);
	AnalogOutputInit();
}

//Calcula el valor de Ro
uint16_t MQResistanceCalculation(uint16_t raw_adc)//raw_adc es el voltaje de alcohol que hay cuando se sensa
{
	uint16_t RL_VALUE = 1;
	uint16_t valores_bits = 1023;
	return ((RL_VALUE*(valores_bits-raw_adc))/raw_adc);
}

//calibra Ro
uint16_t MQCalibration()
{
	int i;
	uint16_t val=0;
	uint16_t value=0;

	for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++)
	{
		AnalogInputReadSingle(my_conversor.input, &value);
	    val += MQResistanceCalculation(value);
	    DelayMs(CALIBRATION_SAMPLE_INTERVAL);
	}
	val = val/CALIBARAION_SAMPLE_TIMES; 

	val = val/RO_CLEAN_AIR_FACTOR;

	if(val == 0)
		val = 1;

	return val;
}

//Calcula Rs
uint16_t MQRead()
{
	int i;
	uint16_t rs=0;
	uint16_t value=0;

	for (i=0;i<READ_SAMPLE_TIMES;i++) {
		AnalogInputRead(my_conversor.input, &value);
		rs += MQResistanceCalculation(value);
   		DelayMs(READ_SAMPLE_INTERVAL);
	}
	rs = rs/READ_SAMPLE_TIMES;
	return rs;
}

float  MQGetPercentage(uint16_t rs_ro_ratio)
{
	return (pow(10,( ((log((float)rs_ro_ratio)-concAlcoholCurve[1])/concAlcoholCurve[2]) + concAlcoholCurve[0])));
}
