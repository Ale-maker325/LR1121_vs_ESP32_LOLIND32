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

String TABLE_RIGHT = F("     ***************************");
String TABLE_LEFT  = F("***************************     ");
String SPACE = F(" ");
String RADIO_1_NAME = F("RADIO_1");
String RADIO_2_NAME = F("RADIO_2");







/**
 * @brief Функция, выводит в сериал монитор сообщения
 * 
 * @param RadioName - имя радио
 * @param state     - текущее состояние радио - это просто информационная сторока
 */
void print_to_terminal_radio_state(String &RadioName, String state)
{
  String str = RadioName + " : " + state;
  Serial.println(str);
}




void (*print_to_terminal)(String &RadioName, String state) = print_to_terminal_radio_state;
// void print_to_terminal_radio_state(String &RadioName, String state)  __attribute__ ((weak));
// int recv_callback()  {return 0;}






/**
 * @brief Функция, которая обеспечивает вывод текущего состояния 
 * приёмопередатчика в сериал-порт (если он задан) и на дисплей
 * 
 * @param state         - текущее состояние, полученное от передатчика при его работе
 * @param transmit_str  - строка для передачи
 * @param radioNumber   - номер передатчика (так как их может быть два)
 */
void printStateResult(int &state, String &transmit_str, Radio_Number radioNumber)
{
  String RADIO_NAME;
  int x, y;
  switch (radioNumber)
  {
  case Radio_1: 
    RADIO_NAME = RADIO_1_NAME;
    x = 5;
    y = 5;
    break;
  case Radio_2: 
    RADIO_NAME = RADIO_2_NAME;
    x = 5;
    y = 20;
    break;
  
  default: 
    RADIO_NAME = "RADIO_NONAME!!!";
    x = 5;
    y = 5;
    break;
  }

  //Если передача успешна, выводим сообщение в сериал-монитор
  if (state == RADIOLIB_ERR_NONE) {
    //Выводим сообщение об успешной передаче
    print_to_terminal_radio_state(RADIO_NAME, F("SEND PACKET"));
    print_to_terminal_radio_state(RADIO_NAME, F("TRANSMITT SUCCES!"));
    displayPrintState(x, y, RADIO_NAME, transmit_str);
                  
    //Выводим в сериал данные отправленного пакета
    Serial.print(F("Data:\t\t"));
    Serial.println(transmit_str);

    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета

    
          
  } else {
    //Если были проблемы при передаче, сообщаем об этом
    String str = (String)state_1;
    Serial.print(F("transmission failed, "));
    print_to_terminal_radio_state(RADIO_NAME, str);

    displayPrintState(x, y, RADIO_NAME, str);
  
  }
  
}







/**
* @brief Функция отправляет данные, выводит на экран информацию об отправке,
* выводит информацию об отправке в сериал-порт
* 
* @param transmit_str - строка для передачи
*/
void transmit_and_print_data(String &transmit_str)
{
  display.clearDisplay();

  //Посылаем пакет
  state_1 = radio1.startTransmit(transmit_str);
  //Ждём завершения передачи
  WaitOnBusy(Radio_1);
  //Печатаем данные куда надо (в сериал, если он активирован, и на дисплей)
  printStateResult(state_1, transmit_str, Radio_1);
  //Оканчиваем передачу первым передатчиком
  state_1 = radio1.finishTransmit();
  //state_1 = radio1.startReceive();

  #ifdef RADIO_2
  //Посылаем пакет
  state_2 = radio2.startTransmit(transmit_str);
  //Ждём завершения передачи
  WaitOnBusy(Radio_2);
  //Печатаем данные куда надо (в сериал, если он активирован, и на дисплей)
  printStateResult(state_2, transmit_str, Radio_2);
  WaitOnBusy(Radio_2);
  //Оканчиваем передачу первым передатчиком
  state_2 = radio2.finishTransmit();
  //state_2 = radio2.startReceive();

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
  








/**
 * @brief Функция, выводящая значение прошивки радио модуля
 * 
 */
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





/**
 * @brief Первоначальная инициализация радио или двух радио
 * 
 */
void radioBeginAll()
{
  pinMode(NSS_PIN_1, OUTPUT);
  #ifdef RADIO_2
  pinMode(NSS_PIN_2, OUTPUT);
  #endif
  
  //Инициализируем радиотрансивер 1 со значениями по-умолчанию, заданными в
  //структуре LORA_CONFIGURATION
  Serial.println(" ");
  Serial.print(RADIO_1_NAME);
  Serial.println(F(" INIT....."));

  digitalWrite(NSS_PIN_1, LOW);
  #ifdef RADIO_2
  digitalWrite(NSS_PIN_2, HIGH);
  #endif

  //Инициализируем просто значениями по-умолчанию
  int state_1 = radio1.begin();
  
  //radio1.explicitHeader();
  //radio1.setCRC(2);

  if (state_1 == RADIOLIB_ERR_NONE) {
    print_to_terminal_radio_state(RADIO_1_NAME, F("INIT_GOOD"));
    displayPrintState(5, 5, RADIO_1_NAME, F("INIT_GOOD"));
  } else {

    String str = "ERROR " + (String)state_1;
    print_to_terminal_radio_state(RADIO_1_NAME, str);
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
  Serial.print(F("Initializing radio 2 ... "));
  //Инициализируем просто значениями по-умолчанию
  int state_2 = radio2.begin();
  //radio2.explicitHeader();
  //radio2.setCRC(2);

  if (state_2 == RADIOLIB_ERR_NONE) {
    print_to_terminal_radio_state(RADIO_2_NAME, F("INIT_GOOD"));
    displayPrintState(5, 20, RADIO_2_NAME, F("INIT_GOOD"));
  } else {
    String str = "ERROR " + (String)state_2;
    print_to_terminal_radio_state(RADIO_2_NAME, str);
    displayPrintState(5, 20, RADIO_2_NAME, str);
    while (true);
  }
  WaitOnBusy(Radio_2);

  #endif

  delay(2000);
}





/**
 * @brief Настройка радио передатчика
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




/**
* @brief Функция установки настроек передатчика
* 
* @param radio - экземпляр класса передатчика
* @param config - экземпляр структуры для настройки модуля
*/
void radio_setSettings(LR1121 radio, LORA_CONFIGURATION config_radio, String radio_name)
{
  Serial.print(TABLE_LEFT);
  Serial.print(F("SET SETTINGTH OF RADIO "));
  Serial.print(radio_name);
  Serial.println(TABLE_RIGHT);
  
  // Устанавливаем необходимую нам частоту работы трансивера
  if (radio.setFrequency(config_radio.frequency) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set frequency = "));
  Serial.println(config_radio.frequency);


  // установить полосу пропускания до 250 кГц
  if (radio.setBandwidth(config_radio.bandwidth) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set bandWidth = "));
  Serial.println(config_radio.bandwidth);

  // коэффициент расширения 
  if (radio.setSpreadingFactor(config_radio.spreadingFactor) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set spreadingFactor = "));
  Serial.println(config_radio.spreadingFactor);

  // установить скорость кодирования
  if (radio.setCodingRate(config_radio.codingRate) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set codingRate = "));
  Serial.println(config_radio.codingRate);

  // Устанавливаем слово синхронизации
  if (radio.setSyncWord(config_radio.syncWord) != RADIOLIB_ERR_NONE) {
    Serial.println(F("Unable to set sync word!"));
    while (true);
  }
  Serial.print(F("Set syncWord = "));
  Serial.println(config_radio.syncWord);

  // Устанавливаем выходную мощность трансивера
  if (radio.setOutputPower(config_radio.outputPower) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
    Serial.println(F("Selected output power is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set setOutputPower = "));
  Serial.println(config_radio.outputPower); 

  // установить длину преамбулы (допустимый диапазон 6 - 65535)
  if (radio.setPreambleLength(config_radio.preambleLength) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println(F("Selected preamble length is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set preambleLength = "));
  Serial.println(config_radio.preambleLength);

  

  Serial.println(F("All settings successfully changed!"));

  Serial.print(TABLE_LEFT);
  Serial.print(F("END SETTINGTH OF RADIO "));
  Serial.print(radio_name);
  Serial.println(TABLE_RIGHT);
  Serial.println(SPACE);
}















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

  Serial.print(TABLE_LEFT);
  Serial.print(F("DISPLAY INIT"));
  Serial.println(TABLE_RIGHT);
  Serial.println(SPACE);
  
  pinMode(LED_PIN, OUTPUT);      //Контакт управления светодиодом (на плате LOLIN D32 он на пине 5)
  pinMode(FUN, OUTPUT);          //Контакт управления вентилятором охлаждения
  
  // установить конфигурацию управления радиочастотным переключателем, это необходимо сделать до вызова метода Begin()
  radio1.setRfSwitchTable(rfswitch_dio_pins, rfswitch_table);
  
  setRadioMode();

  radioBeginAll();
  
  printVersions();

  Serial.println(SPACE);
  Serial.println(SPACE);
    
  //Устанавливаем наши значения, определённые ранее в структуре config_radio1
  radio_setSettings(radio1, config_radio1, "1");
  #ifdef RADIO_2
  radio_setSettings(radio2, config_radio2, "2");
  #endif
  
  
  
  
  
  
  
  #ifdef RECEIVER   //Если определена работа модуля как приёмника

    // //Устанавливаем функцию, которая будет вызываться при получении пакета данных
    // radio1.setPacketReceivedAction(setFlag);

    // //Начинаем слушать есть ли пакеты
    // Serial.print(F("[SX1278] Starting to listen ... "));
    // state = radio1.startReceive();
  
    // if (state == RADIOLIB_ERR_NONE) {
    //   Serial.println(F("success!"));
    //   digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    // } else {
    //   Serial.print(F("failed, code: "));
    //   Serial.println(state);
    //   while (true);
    // }

    // //получаем данные
    // receive_and_print_data();

       
  #endif





  #ifdef TRANSMITTER   //Если определена работа модуля как передатчика

    //Устанавливаем функцию, которая будет вызываться при отправке пакета данных модемом №1
    radio1.setPacketSentAction(setFlag_1);
    #ifdef RADIO_2
    //Устанавливаем функцию, которая будет вызываться при отправке пакета данных модемом №2
    radio2.setPacketSentAction(setFlag_2);
    #endif


    //Начинаем передачу пакетов
    Serial.print(TABLE_LEFT);
    Serial.print(F("SENDING FIRST PACKET"));
    Serial.println(TABLE_RIGHT);

    String str = F("START!");
    transmit_and_print_data(str);
    
    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    delay(1000);

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
    // //Устанавливаем функцию, которая будет вызываться при получении пакета данных
    // radio1.setPacketReceivedAction(setFlag);

    // //Начинаем слушать есть ли пакеты
    // Serial.print(F("[SX1278] Starting to listen ... "));
    // state = radio1.startReceive();

    // if (state == RADIOLIB_ERR_NONE) {
    //   Serial.println(F("success!"));
    //   digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    //   test_is_ok = true;
    // } else {
    //   test_is_ok = false;
    //   Serial.print(F("failed, code: "));
    //   Serial.println(state);
    //   Serial.print(F("test_is_ok = "));
    //   Serial.println(test_is_ok);
    //   while (true);
    // }
  #endif


  
  Serial.println(" ");
}

 






void loop() {
  delay(500);
  digitalWrite(LED_PIN, HIGH); //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета
  


  #ifdef RECEIVER   //Если определен модуль как приёмник
    // //проверяем, была ли предыдущая передача успешной
    // Serial.println("..................................................");
    // if(operationDone) {
    //   //Сбрасываем сработавший флаг прерывания
    //   operationDone = false;
    //   receive_and_print_data();
    // }
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
    // if(test_is_ok)
    // {
    //   delay(1000);
    //   digitalWrite(LED_PIN, LOW); //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета
    // }
  #endif


}



