/**
 * @file main.cpp
 * @author Ale-maker325
 * 
 * @brief Пример работы с модулями LR1121 для ESP32 LOLIN D32. Пример основан на примерах библиотек Adafruit SSD1306 и RadioLib,
 * и рассчитан для применения с дисплеем OLED SSD1306.
 * 
 * Для работы передатчика нужно также определиться, будет он работать как передатчик, либо как приёмник. Для этого необходимо
 * раскомментировать один из дефайнов: #define RECEIVER или #define TRANSMITTER в файле settings.h, который соотчетствует выбранному
 * инвайрменту.
 *  
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <display.h>
#include <radioLR.h>
#include <settings.h>
#include <RadioLib.h>



byte    test_is_ok = false;   //Переменная, хранящая успешность теста при старте передатчика. Значение либо true, либо false
boolean FUN_IS_ON = false;    //Логический флаг включения/отключения вентилятора охлаждения
uint64_t count = 0;           //Счётчик для сохранения количества отправленных/полученных пакетов





String RSSI = F("RSSI("); //Строка для печати RSSI
String dBm = F(")dBm");   //Строка для печати RSSI

String SNR = F("SNR(");   //Строка для печати SNR
String dB = F(")dB");     //Строка для печати SNR

String FR_ERR = F("F_Err(");  //Строка для печати SNR
String HZ = F(")Hz");         //Строка для печати SNR

String DT_RATE = F("RATE(");  //Строка для печати скорости передачи данных
String BS = F(")B/s");        //Строка для печати скорости передачи данных

String RECEIVE = F("RECEIVE: ");  //Строка сообщения для приёма

String TABLE_RIGHT = F("     ***************************");
String TABLE_LEFT  = F("***************************     ");
String SPACE = F(" ");

















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
  Serial.print(RADIO_1_NAME);
  Serial.println(F(" INIT....."));
  #endif

  digitalWrite(NSS_PIN_1, LOW);
  #ifdef RADIO_2
  digitalWrite(NSS_PIN_2, HIGH);
  #endif

  //Инициализируем просто значениями по-умолчанию
  int state_1 = radio1.begin();
  
  //radio1.explicitHeader();
  //radio1.setCRC(2);

  if (state_1 == RADIOLIB_ERR_NONE) {
    #ifdef DEBUG_PRINT
    print_to_terminal_radio_state(RADIO_1_NAME, F("INIT_GOOD"));
    #endif
    displayPrintState(5, 5, RADIO_1_NAME, F("INIT_GOOD"));
  } else {

    String str = "ERROR " + (String)state_1;
    #ifdef DEBUG_PRINT
    print_to_terminal_radio_state(RADIO_1_NAME, str);
    #endif
    displayPrintState(5, 5, RADIO_1_NAME, str);
    while (true);
  }
  WaitOnBusy(Radio_1);
  digitalWrite(NSS_PIN_1, HIGH);
  digitalWrite(NSS_PIN_2, LOW);
  delay(2000);

  #ifdef RADIO_2
  //Инициализируем радиотрансивер 2 со значениями по-умолчанию
  Serial.println(" ");
  Serial.println(F("RADIO_2 INIT ...."));
  //Инициализируем просто значениями по-умолчанию
  int state_2 = radio2.begin();
  //radio2.explicitHeader();
  //radio2.setCRC(2);

  if (state_2 == RADIOLIB_ERR_NONE) {
    #ifdef DEBUG_PRINT
    print_to_terminal_radio_state(RADIO_2_NAME, F("INIT_GOOD"));
    #endif
    displayPrintState(5, 20, RADIO_2_NAME, F("INIT_GOOD"));
  } else {
    String str = "ERROR " + (String)state_2;
    #ifdef DEBUG_PRINT
    print_to_terminal_radio_state(RADIO_2_NAME, str);
    #endif
    displayPrintState(5, 20, RADIO_2_NAME, str);
    while (true);
  }
  WaitOnBusy(Radio_2);

  #endif

  delay(2000);
}









/**
* @brief Функция установки настроек передатчика
* 
* @param radio - экземпляр класса передатчика
* @param config - экземпляр структуры для настройки модуля
*/
void radio_setSettings(LR1121 radio, LORA_CONFIGURATION config_radio, String radio_name)
{
  #ifdef DEBUG_PRINT
  Serial.print(TABLE_LEFT);
  Serial.print(F("SET SETTINGTH OF RADIO "));
  Serial.print(radio_name);
  Serial.println(TABLE_RIGHT);
  #endif
  
  // Устанавливаем необходимую нам частоту работы трансивера
  if (radio.setFrequency(config_radio.frequency) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Selected frequency is invalid for this module!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set frequency = "));
  Serial.println(config_radio.frequency);
  #endif


  // установить полосу пропускания до 250 кГц
  if (radio.setBandwidth(config_radio.bandwidth) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set bandWidth = "));
  Serial.println(config_radio.bandwidth);
  #endif

  // коэффициент расширения 
  if (radio.setSpreadingFactor(config_radio.spreadingFactor) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set spreadingFactor = "));
  Serial.println(config_radio.spreadingFactor);
  #endif

  // установить скорость кодирования
  if (radio.setCodingRate(config_radio.codingRate) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Selected coding rate is invalid for this module!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set codingRate = "));
  Serial.println(config_radio.codingRate);
  #endif

  // Устанавливаем слово синхронизации
  if (radio.setSyncWord(config_radio.syncWord) != RADIOLIB_ERR_NONE) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Unable to set sync word!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set syncWord = "));
  Serial.println(config_radio.syncWord);
  #endif

  // Устанавливаем выходную мощность трансивера
  if (radio.setOutputPower(config_radio.outputPower) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Selected output power is invalid for this module!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set setOutputPower = "));
  Serial.println(config_radio.outputPower); 
  #endif

  // установить длину преамбулы (допустимый диапазон 6 - 65535)
  if (radio.setPreambleLength(config_radio.preambleLength) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
    #ifdef DEBUG_PRINT
    Serial.println(F("Selected preamble length is invalid for this module!"));
    #endif
    while (true);
  }
  #ifdef DEBUG_PRINT
  Serial.print(F("Set preambleLength = "));
  Serial.println(config_radio.preambleLength);

  

  Serial.println(F("All settings successfully changed!"));

  Serial.print(TABLE_LEFT);
  Serial.print(F("END SETTINGTH OF RADIO "));
  Serial.print(radio_name);
  Serial.println(TABLE_RIGHT);
  Serial.println(SPACE);
  #endif
}





// flag to indicate that a scan was completed
volatile bool scanFlag = false;

void wifi_scan(LR1121 radio, void (*func)(void))
{
  int state;
  
  
  // scan all WiFi signals with default scan configuration
  Serial.print(F("[LR1110] Starting passive WiFi scan ... "));
  state = radio.startWifiScan('*');
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    Serial.println(" ");
  }

  scanFlag = false;



  if(scanFlag) {
    // reset flag
    scanFlag = false;

    // get the number of scan results
    uint8_t count = 0;
    Serial.print(F("[LR1110] Reading WiFi scan results ... "));
    int state = radio.getWifiScanResultsCount(&count);
    if(state == RADIOLIB_ERR_NONE) {
      Serial.println(F("success!"));

      // print the table header
      Serial.print(F("[LR1110] Reading "));
      Serial.print(count);
      Serial.println(F(" scan results:"));
      Serial.println(F(" # | WiFi type\t| Frequency\t| MAC Address\t    | Country\t| RSSI [dBm]\t| SSID"));

      // read all results one by one
      // this result type contains the most information, including the SSID
      LR11x0WifiResultExtended_t result;
      for(int i = 0; i < count; i++) {
        if(i < 10) { Serial.print(" "); } Serial.print(i); Serial.print(" | ");
        state = radio.getWifiScanResult(&result, i);
        if(state != RADIOLIB_ERR_NONE) {
          Serial.print(F("Failed to read result, code "));
          Serial.println(state);
          continue;
        }

        // print the basic information
        Serial.print(F("802.11")); Serial.print(result.type); Serial.print("\t| ");
        Serial.print(result.channelFreq); Serial.print(" MHz\t| ");

        // print MAC address
        for(int j = 0; j < 6; j++) {
          if(result.mac[j] < 0x10) { Serial.print("0"); }
          Serial.print(result.mac[j], HEX);
          if(j < 5) { Serial.print(":"); }
        }
        Serial.print(" | ");

        // print the two-letter country code
        String country = result.countryCode;
        Serial.print(country);
        Serial.print("  \t| ");

        // print the RSSI
        Serial.print(result.rssi);
        Serial.print("\t| ");
        
        // print the network SSID
        Serial.println((char*)result.ssid);
      }
    
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
    
    // start scanning again
    Serial.print(F("[LR1110] Starting passive WiFi scan ... "));
    state = radio.startWifiScan('*');
    if (state == RADIOLIB_ERR_NONE) {
      Serial.println(F("success!"));
    } else {
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }
}



















































// #include "esp_clk.h"

// void displaySlowClockCalibration() { uint32_t slow_clk_cal = esp_clk_slowclk_cal_get(); Serial.print("Slow Clock Calibration Value: "); Serial.print(slow_clk_cal); Serial.println(" microseconds"); }
// void displayCpuFrequency() { int cpu_freq = esp_clk_cpu_freq(); Serial.print("CPU Frequency: "); Serial.print(cpu_freq); Serial.println(" Hz"); }
// void displayApbFrequency() { int apb_freq = esp_clk_apb_freq(); Serial.print("APB Frequency: "); Serial.print(apb_freq); Serial.println(" Hz"); }
// void displayRtcTime() { uint64_t rtc_time = esp_clk_rtc_time(); Serial.print("RTC Time: "); Serial.print(rtc_time); Serial.println(" microseconds"); }



void setup() {
  //Инициализируем сериал-монитор со скоростью 115200
  #ifdef DEBUG_PRINT
  Serial.begin(115200);
  #endif

  display_wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 100000);
  SPI_MODEM.begin(SCK_RADIO, MISO_RADIO, MOSI_RADIO);
  
  #ifdef DEBUG_PRINT
  Serial.printf("Chip Model %s, ChipRevision %d, Cpu Freq %d, SDK Version %s\n", ESP.getChipModel(), ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
  #endif
    
  //инициализируем дисплей
  displayInit();

  #ifdef DEBUG_PRINT
  Serial.print(TABLE_LEFT);
  Serial.print(F("DISPLAY INIT"));
  Serial.println(TABLE_RIGHT);
  Serial.println(SPACE);
  #endif
  
  pinMode(LED_PIN, OUTPUT);      //Контакт управления светодиодом (на плате LOLIN D32 он на пине 5)
  pinMode(FUN, OUTPUT);          //Контакт управления вентилятором охлаждения
  
  // установить конфигурацию управления радиочастотным переключателем, это необходимо сделать до вызова метода Begin()
  radio1.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table_1);
  #ifdef RADIO_2
  radio2.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table_2);
  #endif
  
  setRadioMode();

  radioBeginAll();
  
  #ifdef DEBUG_PRINT
  printVersions();
  #endif

  #ifdef DEBUG_PRINT
  Serial.println(SPACE);
  Serial.println(SPACE);
  #endif
    
  //Устанавливаем наши значения, определённые ранее в структуре config_radio1
  radio_setSettings(radio1, config_radio1, "1");
  #ifdef RADIO_2
  radio_setSettings(radio2, config_radio2, "2");
  #endif
  
  
  
  
  
  
  
  #ifdef RECEIVER   //Если определена работа модуля как приёмника

    //Устанавливаем функцию, которая будет вызываться при получении пакета данных
    radio1.setPacketReceivedAction(setFlag_1);
    #ifdef RADIO_2
    radio2.setPacketReceivedAction(setFlag_2);
    #endif

    #ifdef DEBUG_PRINT
    //Начинаем слушать есть ли пакеты
    Serial.print(TABLE_LEFT);
    Serial.print(F("[SX1278] Starting to listen RX_1 "));
    Serial.println(TABLE_RIGHT);
    #endif

    state_1 = radio1.startReceive();
    if (state_1 == RADIOLIB_ERR_NONE) {
      #ifdef DEBUG_PRINT
      Serial.println(F("success!"));
      Serial.println(SPACE);
      #endif
      digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    } else {
      #ifdef DEBUG_PRINT
      Serial.print(F("failed, code: "));
      Serial.println(state_1);
      #endif
      while (true);
    }

    #ifdef RADIO_2
    #ifdef DEBUG_PRINT
    //Начинаем слушать есть ли пакеты
    Serial.print(TABLE_LEFT);
    Serial.print(F("[SX1278] Starting to listen RX_2 "));
    Serial.println(TABLE_RIGHT);
    #endif

    state_2 = radio2.startReceive();
    if (state_2 == RADIOLIB_ERR_NONE) {
      #ifdef DEBUG_PRINT
      Serial.println(F("success!"));
      Serial.println(SPACE);
      #endif
      digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    } else {
      #ifdef DEBUG_PRINT
      Serial.print(F("failed, code: "));
      Serial.println(state_2);
      #endif
      while (true);
    }
    #endif

    String str;
    receive_and_print_data(str);

  #endif





  #ifdef TRANSMITTER   //Если определена работа модуля как передатчика

    //Устанавливаем функцию, которая будет вызываться при отправке пакета данных модемом №1
    radio1.setPacketSentAction(setFlag_1);
    #ifdef RADIO_2
    //Устанавливаем функцию, которая будет вызываться при отправке пакета данных модемом №2
    //radio2.setPacketSentAction(setFlag_2);

    radio2.setIrqAction(setFlag_2);
    Serial.println(F("***** radio2.setIrqAction(setFlag_2) *****"));
    wifi_scan(radio2, setFlag_2);
    #endif

    #ifdef DEBUG_PRINT
    //Начинаем передачу пакетов
    Serial.print(TABLE_LEFT);
    Serial.print(F("SENDING FIRST PACKET"));
    Serial.println(TABLE_RIGHT);
    #endif

    String str = F("START!");
    transmit_and_print_data(str);
    
    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    delay(1000);

  #endif
  
  #ifdef DEBUG_PRINT
  Serial.println(F("**************************************"));
  #endif

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




  #ifdef DEBUG_PRINT
  Serial.println(" ");
  #endif
}

 






void loop() {
  delay(500);
  digitalWrite(LED_PIN, HIGH); //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета
  


  #ifdef RECEIVER   //Если определен модуль как приёмник
    //проверяем, была ли предыдущая передача успешной
    #ifdef DEBUG_PRINT
    Serial.println("..................................................");
    #endif
    if(operationDone_2) {
      
      //Сбрасываем сработавший флаг прерывания
      operationDone_2 = false;

      //готовим строку для отправки
      String str = "#" + String(count++);

      receive_and_print_data(str);
      
      
    }

    // check CAD result
    detected_CAD(Radio_1);
    detectedPreamble(Radio_1);
    

    #ifdef RADIO_2
    if(operationDone_2) {
      
      //Сбрасываем сработавший флаг прерывания
      operationDone_2 = false;

      //готовим строку для отправки
      String str = "#" + String(count++);

      receive_and_print_data(str);

      // check CAD result
      detected_CAD(Radio_2);
      detectedPreamble(Radio_2);

      
      
    }
    #endif
  #endif


  #ifdef TRANSMITTER   //Если определен как передатчик
    //проверяем, была ли предыдущая передача успешной
    #ifdef DEBUG_PRINT
    Serial.println("..................................................");
    #endif
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
      #ifdef DEBUG_PRINT
      Serial.println(F("[LR1110] Packet detected!"));
      #endif

    } else if (state == RADIOLIB_CHANNEL_FREE) {
      // channel is free
      #ifdef DEBUG_PRINT
      Serial.println(F("[LR1110] Channel is free!"));
      #endif

    } else {
      // some other error occurred
      #ifdef DEBUG_PRINT
      Serial.print(F("[LR1110] Failed, code "));
      Serial.println(state);
      #endif

    }

    #ifdef DEBUG_PRINT
    Serial.println(F("[LR1110] Scanning channel for LoRa transmission ... "));
    #endif

    // start scanning current channel
    state = radio1.scanChannel();

    if (state == RADIOLIB_LORA_DETECTED) {
      // LoRa preamble was detected
      #ifdef DEBUG_PRINT
      Serial.println(F("detected!"));
      #endif

    } else if (state == RADIOLIB_CHANNEL_FREE) {
      // no preamble was detected, channel is free
      #ifdef DEBUG_PRINT
      Serial.println(F("channel is free!"));
      #endif

    } else {
      // some other error occurred
      #ifdef DEBUG_PRINT
      Serial.print(F("failed, code "));
      Serial.println(state);
      #endif

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

  
}



