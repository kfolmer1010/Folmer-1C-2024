#ifndef _SENSORMQ3_H_
#define _SENSORMQ3_H_


#include "stdint.h"

#define RO_CLEAN_AIR_FACTOR 9.5  //!< Valor de la relacion R0/RS en aire limpio.

#define CALIBARAION_SAMPLE_TIMES     50    //!< Numero de vueltas de calibracion de R0.
#define CALIBRATION_SAMPLE_INTERVAL  200   //!< Intervalo de tiempo para calibracion de R0.
#define READ_SAMPLE_INTERVAL         50    //!< Intervalo de tiempo para lectura de RS.
#define READ_SAMPLE_TIMES            5     //!< Numero de vueltas de lectura de RS.


/** @fn MQInit
 *  @brief Inicializa el sensor MQ3
 *  @return null
 */
void MQInit();

/** @fn MQResistanceCalculation
 *  @brief Obtiene el valor de la resistencia R0
 *  @param[in] raw_adc
 *  @return Entero de 16 bits
 */
uint16_t MQResistanceCalculation(uint16_t raw_adc);

/** @fn MQCalibration
 *  @brief Calibra la resistencia R0
 *  @return Entero de 16 bits
 */
uint16_t MQCalibration();

/** @fn MQRead
 *  @brief Lee el valor de la resistencia RS
 *  @return Entero de 16 bits
 */
uint16_t MQRead();

/** @fn MQGetPercentage
 *  @brief Obtiene el valor del gas butano en ppm
 *  @return Flotante de 32 bits
 */
float  MQGetPercentage(uint16_t rs_ro_ratio);


#endif /* MODULES_LPC4337_M4_DRIVERS_DEVICES_INC_SENSORMQ2_H_ */
