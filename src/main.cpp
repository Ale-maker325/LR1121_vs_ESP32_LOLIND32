/**
 * @file main.cpp
 * @author Ale-maker325
 * 
 * @brief Пример работы с модулями LR1121 для ESP32 LOLIN D32. Пример основан на примерах библиотек Adafruit SSD1306 и RadioLib,
 * и рассчитан для применения с дисплеем OLED SSD1306.
 * 
 * Для работы передатчика нужно также определиться, будет он работать как передатчик, либо как приёмник. Для этого необходимо
 * раскомментировать один из дефайнов: #define RECEIVER или #define TRANSMITTER, а второй закомментировать.
 *  
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <display.h>
#include <pins.h>
#include <radioLR.h>
#include <settings.h>
#include <RadioLib.h>





byte test_is_ok = false;              //Переменная, хранящая успешность теста при старте передатчика. Значение либо true, либо false




boolean FUN_IS_ON = false;    //Логический флаг включения/отключения вентилятора охлаждения
//Счётчик для сохранения количества отправленных/полученных пакетов
uint64_t count = 0;





String RSSI = F("RSSI("); //Строка для печати RSSI
String dBm = F(")dBm");   //Строка для печати RSSI

String SNR = F("SNR(");   //Строка для печати SNR
String dB = F(")dB");     //Строка для печати SNR

String FR_ERR = F("F_Err(");  //Строка для печати SNR
String HZ = F(")Hz");         //Строка для печати SNR

String DT_RATE = F("RATE(");  //Строка для печати скорости передачи данных
String BS = F(")B/s");        //Строка для печати скорости передачи данных

String TRANSMIT_1 = F("TRANSMIT_1: ");  //Строка сообщения для передачи
String TRANSMIT_2 = F("TRANSMIT_2: ");  //Строка сообщения для передачи

String RECEIVE = F("RECEIVE: ");  //Строка сообщения для приёма


























/**
* @brief Функция отправляет данные, выводит на экран информацию об отправке,
* выводит информацию об отправке в сериал-порт
* 
* @param transmit_str - строка для передачи
*/
void transmit_and_print_data(String &transmit_str)
{
  //Посылаем очередной пакет
  Serial.print(F("Send packet ... "));

  state_1 = radio1.startTransmit(transmit_str);
  

  #ifdef RADIO_2
  state_2 = radio2.startTransmit(transmit_str);
  #endif

  //Если передача успешна, выводим сообщение в сериал-монитор
  if (state_1 == RADIOLIB_ERR_NONE) {
    //Выводим сообщение об успешной передаче
    Serial.println(F("transmission finished succes!"));

    display.setCursor(0, 0);
    String str1 = TRANSMIT_1 + transmit_str;
    display.print(str1);
                  
    //Выводим в сериал данные отправленного пакета
    Serial.print(F("Data:\t\t"));
    Serial.println(transmit_str);

    display.display();
    display.clearDisplay();

    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета

    
          
  } else {
    //Если были проблемы при передаче, сообщаем об этом
    Serial.print(F("transmission failed, code = "));
    Serial.println(state_1);
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print(F("ERROR: "));
    display.print(state_1);
    display.display();

  }

  state_1 = radio1.finishTransmit();
  state_1 = radio1.startReceive();
  


  #ifdef RADIO_2
  //Если передача успешна, выводим сообщение в сериал-монитор
  if (state_2 == RADIOLIB_ERR_NONE) {
    //Выводим сообщение об успешной передаче
    Serial.println(F("transmission finished succes!"));

    display.setCursor(0, 20);
    String str2 = TRANSMIT_2 + transmit_str;
    display.print(str2);
                  
    //Выводим в сериал данные отправленного пакета
    Serial.print(F("Data:\t\t"));
    Serial.println(transmit_str);

    display.display();
    display.clearDisplay();

    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
          
  } else {
    //Если были проблемы при передаче, сообщаем об этом
    Serial.print(F("transmission failed, code = "));
    Serial.println(state_2);
    display.clearDisplay();
    display.setCursor(0, 30);
    display.print(F("ERROR: "));
    display.print(state_2);
    display.display();

  }
  #endif

}
  




/**
* @brief Функция получает данные, выводит на экран информацию о полученном,
* выводит информацию о получении в сериал-порт
* 
*/
//String str;
void receive_and_print_data()
{ 
  String str = " ";
  //можно прочитать полученные данные как строку
  state_1 = radio1.readData(str);

  //Если пакет данных был получен успешно, распечатываем данные
  //в сериал - монитор и на экран
  if (state_1 == RADIOLIB_ERR_NONE) {

    display.setCursor(0, 0); 
    display.print(RECEIVE);
    display.print(str);
                  
    Serial.println(F("Received packet!"));

    // print data of the packet
    Serial.print(F("Data:\t\t"));
    Serial.println(str);

    // print RSSI (Received Signal Strength Indicator)
    float rssi_data = radio1.getRSSI();
    String RSSI_DATA = (String)rssi_data;
          
    Serial.print(F("\t\t\t"));
    Serial.print(RSSI);
    Serial.print(RSSI_DATA);
    Serial.println(dBm);
          
    display.setCursor(0, 16);
    display.print(RSSI);
    display.print(RSSI_DATA);
    display.print(dBm);
              

    // print SNR (Signal-to-Noise Ratio)
    float snr_data = radio1.getSNR();
    String SNR_DATA = (String)snr_data;

    Serial.print(F("\t\t\t"));
    Serial.print(SNR);
    Serial.print(SNR_DATA);
    Serial.println(dB);

    display.setCursor(0, 27);
    display.print(SNR);
    display.print(SNR_DATA);
    display.print(dB);

    // print frequency error
    float freq_error = radio1.getFrequencyError();
    String FREQ_ERROR = (String) freq_error;

    Serial.print(F("\t\t\t"));
    Serial.print(FR_ERR);
    Serial.print(FREQ_ERROR);
    Serial.println(HZ);

    display.setCursor(0, 38);
    display.print(FR_ERR);
    display.print(FREQ_ERROR);
    display.print(HZ);

    display.display();
    display.clearDisplay();

    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета

    } else if (state_1 == RADIOLIB_ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC ERROR!"));
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print(F("CRC ERROR!"));
      display.display();

    } else {
      // some other error occurred
      Serial.print(F("Failed, code "));
      Serial.println(state_1);
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print(F("ERROR: "));
      display.print(state_1);
      display.display();
    }
}
  









void printVersions() {
  LR11x0VersionInfo_t version1;
  LR11x0VersionInfo_t version2;

  Serial.println(F("****************** [LR1110] Reading firmware versions ******************** "));

  int16_t state1 = radio1.getVersionInfo(&version1);
  #ifdef RADIO_2
  int16_t state2 = radio2.getVersionInfo(&version2);
  #endif

  if (state1 == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
    Serial.print(F("[LR1110] Device1: "));
    Serial.println(version1.device);

    Serial.print(F("[LR1110] Base firmware1: "));
    Serial.print(version1.fwMajor);
    Serial.print('.');
    Serial.println(version1.fwMinor);

    Serial.print(F("[LR1110] WiFi firmware1: "));
    Serial.print(version1.fwMajorWiFi);
    Serial.print('.');
    Serial.println(version1.fwMinorWiFi);

    Serial.print(F("[LR1110] GNSS firmware1: "));
    Serial.print(version1.fwGNSS);
    Serial.print('.');
    Serial.println(version1.almanacGNSS);

  } else {
    Serial.print(F("failed_radio №1:, code "));
    Serial.println(state1);
    while (true) { delay(10); }
  }

  #ifdef RADIO_2
    if (state2 == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
    Serial.print(F("[LR1110] Device1: "));
    Serial.println(version2.device);

    Serial.print(F("[LR1110] Base firmware1: "));
    Serial.print(version2.fwMajor);
    Serial.print('.');
    Serial.println(version2.fwMinor);

    Serial.print(F("[LR1110] WiFi firmware1: "));
    Serial.print(version2.fwMajorWiFi);
    Serial.print('.');
    Serial.println(version2.fwMinorWiFi);

    Serial.print(F("[LR1110] GNSS firmware1: "));
    Serial.print(version2.fwGNSS);
    Serial.print('.');
    Serial.println(version2.almanacGNSS);

  } else {
    Serial.print(F("failed_radio №2:, code "));
    Serial.println(state2);
    while (true) { delay(10); }
  }
  #endif

  Serial.println(F("****************** [LR1110] END firmware versions ******************** "));
  
}







typedef uint8_t Radio_Number;
enum
{
    Radio_NONE = 0b00000000,     // Bit mask for no radio
    Radio_1    = 0b00000001,     // Bit mask for radio 1
    Radio_2    = 0b00000010,     // Bit mask for radio 2
    Radio_All  = 0b00000011      // bit mask for both radios
};





// bool ICACHE_RAM_ATTR WaitOnBusy(Radio_Number radioNumber)
// {
//     constexpr uint32_t wtimeoutUS = 1000U;
//     uint32_t startTime = 0;

//     while (true)
//     {
//         if (radioNumber == Radio_1)
//         {
//             if (digitalRead(GPIO_PIN_BUSY) == LOW) return true;
//         }
//         else if (radioNumber == Radio_2)
//         {
//             if (GPIO_PIN_BUSY_2 == UNDEF_PIN || digitalRead(GPIO_PIN_BUSY_2) == LOW) return true;
//         }
//         else if (radioNumber == Radio_All)
//         {
//             if (GPIO_PIN_BUSY_2 != UNDEF_PIN)
//             {
//                 if (digitalRead(GPIO_PIN_BUSY) == LOW && digitalRead(GPIO_PIN_BUSY_2) == LOW) return true;
//             }
//             else
//             {
//                 if (digitalRead(GPIO_PIN_BUSY) == LOW) return true;
//             }
//         }
//         // Use this time to call micros().
//         uint32_t now = micros();
//         if (startTime == 0) startTime = now;
//         if ((now - startTime) > wtimeoutUS) return false;
//     }
// }













































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


static const Module::RfSwitchMode_t rfswitch_table[] = {
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





























// #include "esp_clk.h"

// void displaySlowClockCalibration() { uint32_t slow_clk_cal = esp_clk_slowclk_cal_get(); Serial.print("Slow Clock Calibration Value: "); Serial.print(slow_clk_cal); Serial.println(" microseconds"); }
// void displayCpuFrequency() { int cpu_freq = esp_clk_cpu_freq(); Serial.print("CPU Frequency: "); Serial.print(cpu_freq); Serial.println(" Hz"); }
// void displayApbFrequency() { int apb_freq = esp_clk_apb_freq(); Serial.print("APB Frequency: "); Serial.print(apb_freq); Serial.println(" Hz"); }
// void displayRtcTime() { uint64_t rtc_time = esp_clk_rtc_time(); Serial.print("RTC Time: "); Serial.print(rtc_time); Serial.println(" microseconds"); }



void setup() {
  //Инициализируем сериал-монитор со скоростью 115200
  Serial.begin(115200);
  
  Serial.printf("Chip Model %s, ChipRevision %d, Cpu Freq %d, SDK Version %s\n", ESP.getChipModel(), ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
    
  //инициализируем дисплей
  displayInit();
  Serial.println(F("*****************     display is init     **********************"));
  
  pinMode(LED_PIN, OUTPUT);      //Контакт управления светодиодом
  pinMode(FUN, OUTPUT);          //Контакт управления вентилятором охлаждения

  
  // pinMode(13, OUTPUT);
  // digitalWrite(13, LOW);
  
  // установить конфигурацию управления радиочастотным переключателем, это необходимо сделать до вызова метода Begin()
  radio1.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);
  
  
  //Задаём параметры конфигурации радиотрансивера 1
  config_radio1.frequency = 434;
  config_radio1.bandwidth = 125;
  config_radio1.spreadingFactor = 9;
  config_radio1.codingRate = 7;
  config_radio1.syncWord = RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE;
  config_radio1.outputPower = 10;
  config_radio1.currentLimit = 100;
  config_radio1.preambleLength = 8;
  config_radio1.gain = 0;

  #ifdef RADIO_2
  //Задаём параметры конфигурации радиотрансивера 2
  config_radio2.frequency = 455;
  config_radio2.bandwidth = 125;
  config_radio2.spreadingFactor = 9;
  config_radio2.codingRate = 7;
  config_radio2.syncWord = RADIOLIB_LR11X0_LORA_SYNC_WORD_PRIVATE;
  config_radio2.outputPower = 20;
  config_radio2.currentLimit = 100;
  config_radio2.preambleLength = 8;
  config_radio2.gain = 0;
  #endif
  



  
  //Инициализируем радиотрансивер 1 со значениями по-умолчанию
  Serial.println(" ");
  Serial.print(F("Initializing radio 1 ... "));
  //Инициализируем просто значениями по-умолчанию
  //int state = radio1.begin();
  int state_1 = radio1.begin();
  //radio1.explicitHeader();
  //radio1.setCRC(2);

  if (state_1 == RADIOLIB_ERR_NONE) {
    Serial.println(F("SUCCES!"));
    bool test_is_ok = true;
    display.setCursor(0, 10);
    display.print(F("OK_1: "));
    display.print(state_1);
    display.display();

  } else {
    Serial.print(F("ERROR_1!  "));
    Serial.println(state_1);
    
    display.setCursor(0, 10);
    display.print(F("ERROR_1: "));
    display.print(state_1);
    display.display();

    test_is_ok = false;

    while (true);
  }

  
  #ifdef RADIO_2
  //Инициализируем радиотрансивер 2 со значениями по-умолчанию
  Serial.println(" ");
  Serial.print(F("Initializing radio 2 ... "));
  //Инициализируем просто значениями по-умолчанию
  int state_2 = radio2.begin();
  //radio2.explicitHeader();
  //radio2.setCRC(2);

  if (state_2 == RADIOLIB_ERR_NONE) {
    Serial.println(F("SUCCES!"));
    bool test_is_ok = true;
    display.setCursor(0, 20);
    display.print(F("OK_2: "));
    display.print(state_2);
    display.display();

  } else {
    Serial.print(F("ERROR_2!  "));
    Serial.println(state_2);
    
    display.setCursor(0, 20);
    display.print(F("ERROR_2: "));
    display.print(state_2);
    display.display();

    test_is_ok = false;

    while (true);
  }

  #endif


  printVersions();
  Serial.println(F("_________________________________________________________________________________"));
  Serial.println(" ");
    
  //Устанавливаем наши значения, определённые ранее в структуре config_radio1
  radio_setSettings(radio1, config_radio1, "1");
  #ifdef RADIO_2
  radio_setSettings(radio2, config_radio2, "2");
  #endif
  
  
  
  
  
  
  
  #ifdef RECEIVER   //Если определена работа модуля как приёмника

    //Устанавливаем функцию, которая будет вызываться при получении пакета данных
    radio1.setPacketReceivedAction(setFlag);

    //Начинаем слушать есть ли пакеты
    Serial.print(F("[SX1278] Starting to listen ... "));
    state = radio1.startReceive();
  
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("success!"));
      digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    } else {
      Serial.print(F("failed, code: "));
      Serial.println(state);
      while (true);
    }

    //получаем данные
    receive_and_print_data();

       
  #endif





  #ifdef TRANSMITTER   //Если определена работа модуля как передатчика

    //Устанавливаем функцию, которая будет вызываться при отправке пакета данных модемом №1
    radio1.setPacketSentAction(setFlag_1);
    #ifdef RADIO_2
    //Устанавливаем функцию, которая будет вызываться при отправке пакета данных модемом №2
    radio2.setPacketSentAction(setFlag_2);
    #endif


    //Начинаем передачу пакетов
    Serial.println(F("*************  Sending first packet  **************"));
    String str = F("START!");
    transmit_and_print_data(str);
    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    delay(2000);

  #endif
  

  Serial.println(F("**************************************"));

  digitalWrite(LED_PIN, HIGH);      //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета

  #ifdef TRANSMITTER
  //Если мощность усилителя передатчика больше 200 милливат (вы можете установить своё значение),
  // и вентилятор охлаждения не включен, то включаем вентилятор охлаждения
  if(config_radio1.outputPower > 1 && FUN_IS_ON != true)
  {
    //отмечаем вентилятор как включенный
    FUN_IS_ON = true;
    //и включаем его
    digitalWrite(FUN, HIGH);
  }
  #endif




  #ifdef TEST
    //Устанавливаем функцию, которая будет вызываться при получении пакета данных
    radio1.setPacketReceivedAction(setFlag);

    //Начинаем слушать есть ли пакеты
    Serial.print(F("[SX1278] Starting to listen ... "));
    state = radio1.startReceive();

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("success!"));
      digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
      test_is_ok = true;
    } else {
      test_is_ok = false;
      Serial.print(F("failed, code: "));
      Serial.println(state);
      Serial.print(F("test_is_ok = "));
      Serial.println(test_is_ok);
      while (true);
    }
  #endif


  
  Serial.println(" ");

  
}

 


void loop() {
  digitalWrite(15, LOW);
  delay(500 );
  digitalWrite(LED_PIN, HIGH); //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета
  digitalWrite(15, HIGH);


  #ifdef RECEIVER   //Если определен модуль как приёмник
    //проверяем, была ли предыдущая передача успешной
    Serial.println("..................................................");
    if(operationDone) {
      //Сбрасываем сработавший флаг прерывания
      operationDone = false;
      receive_and_print_data();
    }
  #endif


  #ifdef TRANSMITTER   //Если определен как передатчик
    //проверяем, была ли предыдущая передача успешной
    Serial.println("..................................................");
    if(operationDone_1) {
      
      //Сбрасываем сработавший флаг прерывания
      operationDone_1 = false;

      //готовим строку для отправки
      String str = "#" + String(count++);

      transmit_and_print_data(str);
      
      
    }

    // check CAD result
    int state = radio1.getChannelScanResult();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa packet was detected
      Serial.println(F("[LR1110] Packet detected!"));

    } else if (state == RADIOLIB_CHANNEL_FREE) {
      // channel is free
      Serial.println(F("[LR1110] Channel is free!"));

    } else {
      // some other error occurred
      Serial.print(F("[LR1110] Failed, code "));
      Serial.println(state);

    }

    Serial.println(F("[LR1110] Scanning channel for LoRa transmission ... "));

    // start scanning current channel
    state = radio1.scanChannel();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa preamble was detected
      Serial.println(F("detected!"));

    } else if (state == RADIOLIB_CHANNEL_FREE) {
      // no preamble was detected, channel is free
      Serial.println(F("channel is free!"));

    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);

    }


    #ifdef RADIO_2
    if(operationDone_2) {
      
      //Сбрасываем сработавший флаг прерывания
      operationDone_2 = false;

      //готовим строку для отправки
      String str = "#" + String(count++);

      transmit_and_print_data(str);
      
      
    }
    #endif
  #endif

  #ifdef TEST
    if(test_is_ok)
    {
      delay(1000);
      digitalWrite(LED_PIN, LOW); //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета
    }
  #endif


}



