#ifndef ________RADIO_LR1121________
#define ________RADIO_LR1121________


#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include <stdint.h>
#include <settings.h>



int state_1 = RADIOLIB_ERR_NONE; // Переменная, хранящая код состояния передачи/приёма
int state_2 = RADIOLIB_ERR_NONE; // Переменная, хранящая код состояния передачи/приёма

SPIClass SPI_MODEM;


LR1121 radio1 = new Module(NSS_PIN_1, IRQ_PIN_1, NRST_PIN_1, BUSY_PIN_1, SPI_MODEM); //Инициализируем экземпляр радио
#ifdef RADIO_2
LR1121 radio2 = new Module(NSS_PIN_2, IRQ_PIN_2, NRST_PIN_2, BUSY_PIN_2, SPI_MODEM); //Инициализируем экземпляр радио
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
IRAM_ATTR void setFlag_1(void) {
// мы отправили или получили пакет, установите флаг
  operationDone_1 = true;
}



#ifdef RADIO_2
//Флаг окончания операции отправки/получения модема №2 чтобы указать, что пакет был отправлен или получен
volatile bool operationDone_2 = false;

// Эта функция вызывается, когда модем №2 передает или получает полный пакет
// ВАЖНО: эта функция ДОЛЖНА БЫТЬ 'пуста' типа и НЕ должна иметь никаких аргументов!
IRAM_ATTR void setFlag_2(void) {
// мы отправили или получили пакет, установите флаг
  operationDone_2 = true;
}
#endif


















// // установите конфигурацию радиочастотного переключателя для Wio WM1110
// // Wio WM1110 использует DIO5 и DIO6 для радиочастотного переключения.
// // ПРИМЕЧАНИЕ: другие платы могут отличаться!
// static const uint32_t rfswitch_dio_pins[] = { 
//   RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8,
//   RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC
// };



// static const Module::RfSwitchMode_t rfswitch_table[] = {
//   // mode                  DIO7  DIO8 
//   { LR11x0::MODE_STBY,   { LOW,  LOW  } },
//   { LR11x0::MODE_RX,     { HIGH, LOW  } },
//   { LR11x0::MODE_TX,     { HIGH, HIGH } },
//   { LR11x0::MODE_TX_HP,  { LOW,  HIGH } },
//   { LR11x0::MODE_TX_HF,  { LOW,  LOW  } },
//   { LR11x0::MODE_GNSS,   { LOW,  LOW  } },
//   { LR11x0::MODE_WIFI,   { LOW,  LOW  } },
//   END_OF_MODE_TABLE,
// };




// set RF switch configuration for Wio WM1110
// Wio WM1110 uses DIO5 and DIO6 for RF switching
// NOTE: other boards may be different!
static const uint32_t rfswitch_dio_pins[] = { 
  RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6,
  RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8, RADIOLIB_NC
};


// static const uint32_t rfswitch_dio_pins[] = { 
//   RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8, RADIOLIB_NC, RADIOLIB_NC,
//    RADIOLIB_NC
// };


static const Module::RfSwitchMode_t rfswitch_table_1[] = {
  // mode                  DIO5  DIO6 
  { LR11x0::MODE_STBY,   { LOW,  LOW, HIGH, LOW } },
  { LR11x0::MODE_RX,     { LOW, HIGH, HIGH, HIGH  } },
  { LR11x0::MODE_TX,     { HIGH, LOW, LOW, LOW } },
  { LR11x0::MODE_TX_HP,  { HIGH, LOW, HIGH, LOW } },
  { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
  { LR11x0::MODE_GNSS,   { LOW,  LOW, LOW,  LOW  } },
  { LR11x0::MODE_WIFI,   { LOW,  LOW, LOW,  LOW  } },
  END_OF_MODE_TABLE,
};

#ifdef RADIO_2
static const Module::RfSwitchMode_t rfswitch_table_2[] = {
  // mode                  DIO5  DIO6 
  { LR11x0::MODE_STBY,   { LOW,  LOW, HIGH, LOW } },
  { LR11x0::MODE_RX,     { LOW, HIGH, HIGH, HIGH  } },
  { LR11x0::MODE_TX,     { HIGH, LOW, LOW, LOW } },
  { LR11x0::MODE_TX_HP,  { HIGH, LOW, HIGH, LOW } },
  { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
  { LR11x0::MODE_GNSS,   { LOW,  LOW, LOW,  LOW  } },
  { LR11x0::MODE_WIFI,   { LOW,  LOW, LOW,  LOW  } },
  END_OF_MODE_TABLE,
};
#endif






typedef uint8_t Radio_Number;
enum
{
    Radio_NONE = 0b00000000,     // Bit mask for no radio
    Radio_1    = 0b00000001,     // Bit mask for radio 1
    Radio_2    = 0b00000010,     // Bit mask for radio 2
    Radio_All  = 0b00000011      // bit mask for both radios
};





bool ICACHE_RAM_ATTR WaitOnBusy(Radio_Number radioNumber)
{
    constexpr uint32_t wtimeoutUS = 1000U;
    uint32_t startTime = 0;

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
  }
}







#endif