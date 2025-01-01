#ifndef ________settings__
#define ________settings__



/**
 * @brief Следует раскомментировать одно из определений, а другое закомментировать.
 *  - RADIO_1 - раскомментировать если используется одно радио
 *  - RADIO_2 - раскомментировать в случае если будет два радиомодуля
 */
//#define RADIO_1             //Раскомментировать если будет использован только один модем LR1121
#define RADIO_2             //Раскомментировать если будет использовано два модема LR1121


/**
 * @brief Выбор режима работы радио при компиляции. Раскомментировать то, что необходимо
 * 
 */
//#define TEST                    //раскомментировать, если модуль будет прошиваться для общей проверки работоспособности
//#define RECEIVER                //раскомментировать, если модуль будет использоваться как простой приёмник
#define TRANSMITTER             //раскомментировать, если модуль будет использоваться как простой передатчик


//**************************************************** Параметры радио для компиляции ************************************************

//Задаём параметры конфигурации радиотрансивера 1
#define RADIO_1_FREQ 434
#define RADIO_1_BANDWIDTH 125
#define RADIO_1_SPREAD_FACTOR 9
#define RADIO_1_CODING_RATE 7
#define RADIO_1_SYNC_WORD RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE
#define RADIO_1_OUTPUT_POWER 10
#define RADIO_1_CURRENT_LIMIT 100
#define RADIO_1_PREAMBLE_LENGTH 8
#define RADIO_1_GAIN 0

#ifdef RADIO_2
//Задаём параметры конфигурации радиотрансивера 2
#define RADIO_2_FREQ 455
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


#endif