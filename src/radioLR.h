#ifndef ________RADIO_LR1121________
#define ________RADIO_LR1121________


#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include <stdint.h>
#include <settings.h>
#include <display.h>
#include <dio_rf_switch.h>



int state_1 = RADIOLIB_ERR_NONE; // Переменная, хранящая код состояния передачи/приёма
int state_2 = RADIOLIB_ERR_NONE; // Переменная, хранящая код состояния передачи/приёма

SPIClass SPI_MODEM;


LR1121 radio1 = new Module(NSS_PIN_1, IRQ_PIN_1, NRST_PIN_1, BUSY_PIN_1, SPI_MODEM); //Инициализируем экземпляр радио_1
#ifdef RADIO_2
LR1121 radio2 = new Module(NSS_PIN_2, IRQ_PIN_2, NRST_PIN_2, BUSY_PIN_2, SPI_MODEM); //Инициализируем экземпляр радио_2
#endif






/**
* @brief Структура для настройки параметров радиотрансивера
* 
*/
struct LORA_CONFIGURATION
{
  float frequency = 434.0;        //Частота работы передатчика (по-умолчанию 434 MHz)
  float bandwidth = 125.0;        //Полоса пропускания (по-умолчанию 125 килогерц)
  uint8_t spreadingFactor = 9;   //Коэффициент расширения (по-умолчанию 9)
  uint8_t codingRate = 7;         //Скорость кодирования (по-умолчанию 7)
  uint8_t syncWord = 0x18;        //Слово синхронизации (по-умолчанию 0х18). ВНИМАНИЕ! Значение 0x34 зарезервировано для сетей LoRaWAN и нежелательно для использования
  int8_t outputPower = 15;        //Установить выходную мощность (по-умолчанию 10 дБм) (допустимый диапазон -3 - 17 дБм) ПРИМЕЧАНИЕ: значение 20 дБм позволяет работать на большой мощности, но передача рабочий цикл НЕ ДОЛЖЕН ПРЕВЫШАТЬ 1
  uint8_t currentLimit = 80;      //Установить предел защиты по току (по-умолчанию до 80 мА) (допустимый диапазон 45 - 240 мА) ПРИМЕЧАНИЕ: установить значение 0 для отключения защиты от перегрузки по току
  int16_t preambleLength = 8;    //Установить длину преамбулы (по-умолчанию в 8 символов) (допустимый диапазон 6 - 65535)
  uint8_t gain = 0;               //Установить регулировку усилителя (по-умолчанию 1) (допустимый диапазон 1 - 6, где 1 - максимальный рост) ПРИМЕЧАНИЕ: установить значение 0, чтобы включить автоматическую регулировку усиления оставьте в 0, если вы не знаете, что вы делаете


};



//Экземпляр структуры для настройки параметров радиотрансивера 1
LORA_CONFIGURATION config_radio1;
//Экземпляр структуры для настройки параметров радиотрансивера 1
LORA_CONFIGURATION config_radio2;


/**
 * @brief Настройка радио передатчика в соответствии с директивами,
 * которые заданы в файле "settings.h"
 * 
 */
void setRadioMode()
{
  //Задаём параметры конфигурации радиотрансивера 1
  config_radio1.frequency = RADIO_1_FREQ;
  config_radio1.bandwidth = RADIO_1_BANDWIDTH;
  config_radio1.spreadingFactor = RADIO_1_SPREAD_FACTOR;
  config_radio1.codingRate = RADIO_1_CODING_RATE;
  config_radio1.syncWord = RADIO_1_SYNC_WORD;
  config_radio1.outputPower = RADIO_1_OUTPUT_POWER;
  config_radio1.currentLimit = RADIO_1_CURRENT_LIMIT;
  config_radio1.preambleLength = RADIO_1_PREAMBLE_LENGTH;
  config_radio1.gain = RADIO_1_GAIN;

  #ifdef RADIO_2
  //Задаём параметры конфигурации радиотрансивера 2
  config_radio2.frequency = RADIO_2_FREQ;
  config_radio2.bandwidth = RADIO_2_BANDWIDTH;
  config_radio2.spreadingFactor = RADIO_2_SPREAD_FACTOR;
  config_radio2.codingRate = RADIO_2_CODING_RATE;
  config_radio2.syncWord = RADIO_2_SYNC_WORD;
  config_radio2.outputPower = RADIO_2_OUTPUT_POWER;
  config_radio2.currentLimit = RADIO_2_CURRENT_LIMIT;
  config_radio2.preambleLength = RADIO_2_PREAMBLE_LENGTH;
  config_radio2.gain = RADIO_2_GAIN;
  #endif
}






//Флаг окончания операции отправки/получения модема №1 чтобы указать, что пакет был отправлен или получен
volatile bool operationDone_1 = false;

// Эта функция вызывается, когда модем №1 передает или получает полный пакет
// ВАЖНО: эта функция ДОЛЖНА БЫТЬ 'пуста' типа и НЕ должна иметь никаких аргументов!
IRAM_ATTR void flag_operationDone_1(void) {
// мы отправили или получили пакет, установите флаг
  operationDone_1 = true;
}



#ifdef RADIO_2
//Флаг окончания операции отправки/получения модема №2 чтобы указать, что пакет был отправлен или получен
volatile bool operationDone_2 = false;

// Эта функция вызывается, когда модем №2 передает или получает полный пакет
// ВАЖНО: эта функция ДОЛЖНА БЫТЬ 'пуста' типа и НЕ должна иметь никаких аргументов!
IRAM_ATTR void flag_operationDone_2(void) {
// мы отправили или получили пакет, установите флаг
  operationDone_2 = true;
}
#endif










typedef uint8_t Radio_Number;
enum
{
    Radio_NONE = 0b00000000,     // Bit mask for no radio
    Radio_1    = 0b00000001,     // Bit mask for radio 1
    Radio_2    = 0b00000010,     // Bit mask for radio 2
    Radio_All  = 0b00000011      // bit mask for both radios
};




// set RF switch configuration for Wio WM1110
// Wio WM1110 uses DIO5 and DIO6 for RF switching
// NOTE: other boards may be different!
static const uint32_t rfswitch_dio_pins_1[] = { 
  RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
  RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8, RADIOLIB_NC
};


#ifdef RADIO_2
// set RF switch configuration for Wio WM1110
// Wio WM1110 uses DIO5 and DIO6 for RF switching
// NOTE: other boards may be different!
static const uint32_t rfswitch_dio_pins_2[] = { 
  RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
  RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8, RADIOLIB_NC
};
#endif





enum MODE_RF
{
  TRANSMITTER_PICO_KIT,
  RECEIVER_PICO_KIT,
  TRANSMITTER_D32,
  RECEIVER_D32,

};



//------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------















bool ICACHE_RAM_ATTR WaitOnBusy(Radio_Number radioNumber)
{
    constexpr uint32_t wtimeoutUS = 1000U;
    uint32_t startTime = 0;
    #ifdef DEBUG_PRINT
    Serial.println("");
    Serial.println("WaitOnBusy.....................");
    Serial.println("");
    #endif

    while (true)
    {
        if (radioNumber == Radio_1)
        {
            if (digitalRead(BUSY_PIN_1) == LOW) return true;
        }
        else if (radioNumber == Radio_2)
        {
            if (BUSY_PIN_2 == UNDEF_PIN || digitalRead(BUSY_PIN_2) == LOW) return true;
        }
        else if (radioNumber == Radio_All)
        {
            if (BUSY_PIN_2 != UNDEF_PIN)
            {
                if (digitalRead(BUSY_PIN_1) == LOW && digitalRead(BUSY_PIN_2) == LOW) return true;
            }
            else
            {
                if (digitalRead(BUSY_PIN_1) == LOW) return true;
            }
        }
        // Use this time to call micros().
        uint32_t now = micros();
        if (startTime == 0) startTime = now;
        if ((now - startTime) > wtimeoutUS) return false;
    }

        
}







void detected_CAD(Radio_Number radioNumber)
{
  int state;
  if (radioNumber == Radio_1)
  {
    state = radio1.getChannelScanResult();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa packet was detected
      #ifdef DEBUG_PRINT
      Serial.println(F("[LR1110] RX_1 packet detected!"));
      #endif
    } else {
      // some other error occurred
      #ifdef DEBUG_PRINT
      Serial.print(F("[LR1110] RX_1 packet detected Failed, code "));
      Serial.println(state);
      #endif

    }
  }
  if (radioNumber == Radio_2)
  {
    #ifdef RADIO_2
    state = radio2.getChannelScanResult();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa packet was detected
      #ifdef DEBUG_PRINT
      Serial.println(F("[LR1110] RX_2 packet detected!"));
      #endif
    } else {
      // some other error occurred
      #ifdef DEBUG_PRINT
      Serial.print(F("[LR1110] RX_2 packet detected Failed, code "));
      Serial.println(state);
      #endif

    }
    #endif
  }
}






void detectedPreamble(Radio_Number radioNumber)
{
  int state;
  if (radioNumber == Radio_1)
  {
    // start scanning current channel
    state = radio1.scanChannel();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa preamble was detected
      #ifdef DEBUG_PRINT
      Serial.println(F("Preamble detected!"));
      #endif

    } else {
      // some other error occurred
      #ifdef DEBUG_PRINT
      Serial.print(F("Preamble detected failed, code "));
      Serial.println(state);
      #endif

    }
  }
  if (radioNumber == Radio_2)
  {
    #ifdef RADIO_2
    // start scanning current channel
    state = radio2.scanChannel();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa preamble was detected
      #ifdef DEBUG_PRINT
      Serial.println(F("Preamble detected!"));
      #endif

    } else {
      // some other error occurred
      #ifdef DEBUG_PRINT
      Serial.print(F("Preamble detected failed, code "));
      Serial.println(state);
      #endif

    }
    #endif
  }
}






void print_to_terminal_radio_state(String &RadioName, String state) __attribute__ ((weak));
void displayPrintState(int16_t x, int16_t y, String &RadioName, String state) __attribute__ ((weak));



void printRadioBeginResult(int &STATE, Radio_Number radio_number)
{
  String radio_name;
  int x,y;

  switch (radio_number)
  {
  case Radio_1: 
    radio_name = RADIO_1_NAME;
    x=5;
    y=5;
    break;
  case Radio_2:
    radio_name = RADIO_2_NAME;
    x=5;
    y=20;
    break;
  default:
    break;
  }
  if (STATE == RADIOLIB_ERR_NONE) {
    #ifdef DEBUG_PRINT
    print_to_terminal_radio_state(radio_name, F("INIT_GOOD"));
    #endif
    displayPrintState(x, y, radio_name, F("INIT_GOOD"));
  } else {

    String str = "ERROR " + (String)STATE;
    #ifdef DEBUG_PRINT
    print_to_terminal_radio_state(radio_name, str);
    #endif
    displayPrintState(x, y, radio_name, str);
    while (true);
  }
  
}






void ICACHE_RAM_ATTR selectRadio(Radio_Number radio_number)
{
  switch (radio_number)
  {
  //Если выбираем радио 1 
  case Radio_1:
    digitalWrite(NSS_PIN_1, LOW);
    //Если при этом есть и радио 2, то с него снимаем выделение
    #ifdef RADIO_2
      digitalWrite(NSS_PIN_2, HIGH);
    #endif
    break;
  //Если имеется радио 2, то выбираем его, сняв выделение с радио 1
  case Radio_2:
    digitalWrite(NSS_PIN_1, HIGH);
    //Если при этом есть и радио 2, то с него снимаем выделение
    #ifdef RADIO_2
      digitalWrite(NSS_PIN_2, LOW);
    #endif
    break;
  
  default:
    break;
  }
}






  



/**
 * @brief Настройка радио передатчика в соответствии с директивами,
 * которые заданы в файле "settings.h"
 */
void radioBeginAll()
{
  pinMode(NSS_PIN_1, OUTPUT);
  #ifdef RADIO_2
    pinMode(NSS_PIN_2, OUTPUT);
  #endif
  
  //Инициализируем радиотрансивер 1 со значениями по-умолчанию, заданными в
  //структуре LORA_CONFIGURATION
  #ifdef DEBUG_PRINT
    Serial.println(" ");
    Serial.println(F(""));
    Serial.println(" ");
    Serial.print(RADIO_1_NAME);
    Serial.println(F(" INIT....."));
  #endif

  #ifdef RADIO_2
  selectRadio(Radio_1);
  #endif


  #ifdef LRS_DIO_PINS
  radio1.XTAL = true;
  radio1.setRegulatorDCDC();
  //radio1.setRegulatorLDO();
  #endif
  
  
  radio1.setTCXO(1.6);
  
  int state_1 = radio1.begin();
  printRadioBeginResult(state_1, Radio_1);

  // WaitOnBusy(Radio_1);
  
  // radio1.setRfSwitchTable(rfswitch_dio_pins_1, rfswitch_table_1);
  


  // #ifdef DEBUG_PRINT
  // delay(500);
  // #endif

  #ifdef RADIO_2

    #ifdef DEBUG_PRINT
    //Инициализируем радиотрансивер 2 со значениями по-умолчанию
    Serial.println(" ");
    Serial.println(F("RADIO_2 INIT ...."));
    #endif

    selectRadio(Radio_2);

    #ifdef LRS_DIO_PINS
      radio2.XTAL = true;
      radio2.setRegulatorDCDC();
      //radio2.setRegulatorLDO();
    #endif
    
    radio2.setTCXO(1.6);
    
    int state_2 = radio2.begin();

    #ifdef DEBUG_PRINT
    Serial.println(F("printRadioBeginResult ...."));
    #endif
    printRadioBeginResult(state_2, Radio_2);
    
    // WaitOnBusy(Radio_2);

    // #ifdef RADIO_2
    //   radio2.setRfSwitchTable(rfswitch_dio_pins_2, rfswitch_table_2);
    // #endif

    
    #ifdef DEBUG_PRINT
    delay(1000);
    #endif
    
  #endif
}











#endif;