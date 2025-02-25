#pragma once
#include <Arduino.h>

#define DEBUG_PRINT

// /**
//  * @brief Следует раскомментировать одно из определений, а другое закомментировать.
//  *  - RADIO_1 - раскомментировать если используется одно радио
//  *  - RADIO_2 - раскомментировать в случае если будет два радиомодуля
//  */
#define RADIO_1             //Раскомментировать если будет использован только один модем LR1121
// //#define RADIO_2             //Раскомментировать если будет использовано два модема LR1121


// /**
//  * @brief Выбор режима работы радио при компиляции. Раскомментировать то, что необходимо
//  * 
//  */

// //#define RECEIVER                //раскомментировать, если модуль будет использоваться как простой приёмник
#define TRANSMITTER             //раскомментировать, если модуль будет использоваться как простой передатчик

// #define LRS_DIO_PINS            //раскомментировать, если модуль будет использоваться как модуль ЛРС

//**************************************************** Параметры радио для компиляции ************************************************




//Задаём параметры конфигурации радиотрансивера 1
#define RADIO_1_FREQ 441
#define RADIO_1_BANDWIDTH 125
#define RADIO_1_SPREAD_FACTOR 9
#define RADIO_1_CODING_RATE 7
#define RADIO_1_SYNC_WORD RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE
#define RADIO_1_OUTPUT_POWER 15
#define RADIO_1_CURRENT_LIMIT 300
#define RADIO_1_PREAMBLE_LENGTH 8
#define RADIO_1_GAIN 0

#ifdef RADIO_2
//Задаём параметры конфигурации радиотрансивера 2
#define RADIO_2_FREQ 1941
#define RADIO_2_BANDWIDTH 125
#define RADIO_2_SPREAD_FACTOR 9
#define RADIO_2_CODING_RATE 7
#define RADIO_2_SYNC_WORD RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE
#define RADIO_2_OUTPUT_POWER 10
#define RADIO_2_CURRENT_LIMIT 100
#define RADIO_2_PREAMBLE_LENGTH 8
#define RADIO_2_GAIN 0
#endif


//**************************************************** Параметры радио для компиляции ************************************************


#ifdef TRANSMITTER 
String RADIO_1_NAME = F("TX_1");
String RADIO_2_NAME = F("TX_2");
#endif

#ifdef RECEIVER
String RADIO_1_NAME = F("RX_1");
String RADIO_2_NAME = F("RX_2");
#endif

#ifndef DLRS_DIO_PINS
  #define DLRS_DIO_PINS -1
#endif

//*********************************************************************************************************************************** */
/**
 * @brief Пины I2C
 * 
 */
#ifndef I2C_SDA_PIN
  #define I2C_SDA_PIN -1
#endif

#ifndef I2C_SCL_PIN
  #define I2C_SCL_PIN -1
#endif


// _______________________ Пин вентилятора (если такой нужен) _______________________
#ifndef FUN
  #define FUN -1
#endif

// _______________________ Пин пользовательского светодиода _______________________
#ifndef LED_PIN
  #define LED_PIN -1
#endif


// _______________________ Пины модема номер 1 _______________________
#ifndef NSS_PIN_1
  #define NSS_PIN_1 -1
#endif

#ifndef IRQ_PIN_1
  #define IRQ_PIN_1 -1
#endif

#ifndef NRST_PIN_1
  #define NRST_PIN_1 -1
#endif

#ifndef BUSY_PIN_1
  #define BUSY_PIN_1 -1
#endif


// _______________________ Пины модема номер 2 _______________________
#ifndef NSS_PIN_2
  #define NSS_PIN_2 -1
#endif

#ifndef IRQ_PIN_2
  #define IRQ_PIN_2 -1
#endif

#ifndef NRST_PIN_2
  #define NRST_PIN_2 -1
#endif

#ifndef BUSY_PIN_2
  #define BUSY_PIN_2 -1
#endif



/**
 * @brief Пины SPI управления модемом (модемами)
 * По-умолчанию выбран SPI - VSPI (если конешно пользователь не задаст свои в файле .ini): 
 * _________________________________________________________________________
 *    СПИ	    *      МОСИ	    *      МИСО	    *     СКЛК	    *      КС     *
 *__________________________________________________________________________
 *    VSPI	  *    GPIO 23	  *    GPIO 19	  *    GPIO 18	  *    GPIO5    *
 * ___________*_______________*_______________*_______________*_____________*
 *    HSPI	  *    GPIO 13	  *    GPIO 12	  *    GPIO 14	  *    GPIO 15  *
 * ___________*_______________*_______________*_______________*_____________*
 * 
 */
#ifndef MOSI_RADIO
  #define MOSI_RADIO 23
#endif

#ifndef MISO_RADIO
  #define MISO_RADIO 19
#endif

#ifndef SCK_RADIO
  #define SCK_RADIO 18
#endif


// _______________________ Пины .................... _______________________

#ifndef UNDEF_PIN
  #define UNDEF_PIN -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

#ifndef xx
  #define xx -1
#endif

