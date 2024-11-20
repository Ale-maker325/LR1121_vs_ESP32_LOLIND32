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
#include <RadioLib.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RADIO_1             //Раскомментировать если будет использован только один модем LR1121
//#define RADIO_2             //Раскомментировать если будет использовано два модема LR1121


//#define TEST                    //раскомментировать, если модуль будет прошиваться для общей проверки работоспособности
//#define RECEIVER                //раскомментировать, если модуль будет использоваться как простой приёмник
#define TRANSMITTER             //раскомментировать, если модуль будет использоваться как простой передатчик



byte test_is_ok = false;              //Переменная, хранящая успешность теста при старте передатчика. Значение либо true, либо false


#define SCREEN_WIDTH 128              // Ширина дисплея в пикселах
#define SCREEN_HEIGHT 64              // Высота дисплея в пикселах
#define OLED_RESET    -1              // Пин сброса # ( -1 если для сброса используется стандартный пин ардуино)
#define SCREEN_ADDRESS 0x3C           // Стандартный адрес I2C для дисплея (в моём случае такой адрес дал I2C-сканнер)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Создаём объект дисплея





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




boolean FUN_IS_ON = false;    //Логический флаг включения/отключения вентилятора охлаждения



//Счётчик для сохранения количества отправленных/полученных пакетов
uint64_t count = 0;

// Подключение радиотрансивера SX127.. в соответствии с разводкой  модуля для ESP32:
// const uint8_t FUN = 15;       //Пин управлением вентилятором охлаждения
// const uint32_t NSS_1 = 27;
// const uint32_t NSS_2 = 13;
// const uint32_t RST_1 = 26;
// const uint32_t RST_2 = 17;
// const uint32_t DIO_0_busy_1 = 36;
// const uint32_t DIO_0_busy_2 = 39;
// const uint32_t DIO_1_1 = 16;
// const uint32_t DIO_1_2 = 34;



const uint8_t FUN = 15;       //Пин управлением вентилятором охлаждения

const uint32_t NSS_PIN_1 = 27;
const uint32_t IRQ_PIN_1 = 16;
const uint32_t NRST_PIN_1 = 26;
const uint32_t BUSY_PIN_1 = 36;

LR1121 radio1 = new Module(NSS_PIN_1, IRQ_PIN_1, NRST_PIN_1, BUSY_PIN_1); //Инициализируем экземпляр радио

#ifdef RADIO_2
const uint32_t NSS_PIN_2 = 13;
const uint32_t IRQ_PIN_2 = 34;
const uint32_t NRST_PIN_2 = 17;
const uint32_t BUSY_PIN_2 = 39;
LR1121 radio2 = new Module(NSS_PIN_2, IRQ_PIN_2, NRST_PIN_2, BUSY_PIN_2); //Инициализируем экземпляр радио
#endif





 

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



int state_1 = RADIOLIB_ERR_NONE; // Переменная, хранящая код состояния передачи/приёма
int state_2 = RADIOLIB_ERR_NONE; // Переменная, хранящая код состояния передачи/приёма
uint8_t LED_PIN = 5;           // Пин для управления индикацией светодиодом


/**
* @brief Функция инициализации дисплея 
* 
*/
void displayInit()
{

  //Инициализируем дисплей
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {// SSD1306_SWITCHCAPVCC = напряжение дисплея от 3.3V
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Показываем содержимое буфера дисплея, созданное по-умолчанию
  // библиотека по-умолчанию использует эмблему Adafruit.
  //display.display();
  //delay(1000); // Pause for 2 seconds

  //Очищаем буффер дисплея
  display.clearDisplay();

  display.setTextSize(1);                 // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);    // Draw white text
  display.cp437(true);                    // Use full 256 char 'Code Page 437' font
}




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
  int8_t outputPower = 10;        //Установить выходную мощность (по-умолчанию 10 дБм) (допустимый диапазон -3 - 17 дБм) ПРИМЕЧАНИЕ: значение 20 дБм позволяет работать на большой мощности, но передача рабочий цикл НЕ ДОЛЖЕН ПРЕВЫШАТЬ 1
  uint8_t currentLimit = 80;      //Установить предел защиты по току (по-умолчанию до 80 мА) (допустимый диапазон 45 - 240 мА) ПРИМЕЧАНИЕ: установить значение 0 для отключения защиты от перегрузки по току
  int16_t preambleLength = 8;    //Установить длину преамбулы (по-умолчанию в 8 символов) (допустимый диапазон 6 - 65535)
  uint8_t gain = 0;               //Установить регулировку усилителя (по-умолчанию 1) (допустимый диапазон 1 - 6, где 1 - максимальный рост) ПРИМЕЧАНИЕ: установить значение 0, чтобы включить автоматическую регулировку усиления оставьте в 0, если вы не знаете, что вы делаете


};

//Экземпляр структуры для настройки параметров радиотрансивера 1
LORA_CONFIGURATION config_radio1;
//Экземпляр структуры для настройки параметров радиотрансивера 1
LORA_CONFIGURATION config_radio2;



/**
* @brief Функция установки настроек передатчика
* 
* @param radio - экземпляр класса передатчика
* @param config - экземпляр структуры для настройки модуля
*/
void radio_setSettings(LR1121 radio, String radio_name)
{
  Serial.print(F("Set LoRa settings of radio "));
  Serial.println(radio_name);


  // Устанавливаем необходимую нам частоту работы трансивера
  if (radio.setFrequency(config_radio1.frequency) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set frequency = "));
  Serial.println(config_radio1.frequency);


  // установить полосу пропускания до 250 кГц
  if (radio.setBandwidth(config_radio1.bandwidth) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set bandWidth = "));
  Serial.println(config_radio1.bandwidth);

  // коэффициент расширения 
  if (radio.setSpreadingFactor(config_radio1.spreadingFactor) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set spreadingFactor = "));
  Serial.println(config_radio1.spreadingFactor);

  // установить скорость кодирования
  if (radio.setCodingRate(config_radio1.codingRate) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set codingRate = "));
  Serial.println(config_radio1.codingRate);

  // Устанавливаем слово синхронизации
  if (radio.setSyncWord(config_radio1.syncWord) != RADIOLIB_ERR_NONE) {
    Serial.println(F("Unable to set sync word!"));
    while (true);
  }
  Serial.print(F("Set syncWord = "));
  Serial.println(config_radio1.syncWord);

  // Устанавливаем выходную мощность трансивера
  if (radio.setOutputPower(config_radio1.outputPower) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
    Serial.println(F("Selected output power is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set setOutputPower = "));
  Serial.println(config_radio1.outputPower); 

  // установить длину преамбулы (допустимый диапазон 6 - 65535)
  if (radio.setPreambleLength(config_radio1.preambleLength) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println(F("Selected preamble length is invalid for this module!"));
    while (true);
  }
  Serial.print(F("Set preambleLength = "));
  Serial.println(config_radio1.preambleLength);

  

  Serial.println(F("All settings successfully changed!"));
}







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

  //state_1 = radio1.finishTransmit();



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
  RADIOLIB_NC, RADIOLIB_NC,
  RADIOLIB_LR11X0_DIO7, RADIOLIB_LR11X0_DIO8, RADIOLIB_NC
};

static const Module::RfSwitchMode_t rfswitch_table[] = {
  // mode                  DIO5  DIO6 
  { LR11x0::MODE_STBY,   { LOW,  LOW  } },
  { LR11x0::MODE_RX,     { LOW, HIGH  } },
  { LR11x0::MODE_TX,     { HIGH, LOW } },
  { LR11x0::MODE_TX_HP,  { HIGH,  LOW } },
  { LR11x0::MODE_TX_HF,  { LOW,  LOW  } },
  { LR11x0::MODE_GNSS,   { LOW,  LOW  } },
  { LR11x0::MODE_WIFI,   { LOW,  LOW  } },
  END_OF_MODE_TABLE,
};






// #include "esp_clk.h"

// void displaySlowClockCalibration() { uint32_t slow_clk_cal = esp_clk_slowclk_cal_get(); Serial.print("Slow Clock Calibration Value: "); Serial.print(slow_clk_cal); Serial.println(" microseconds"); }
// void displayCpuFrequency() { int cpu_freq = esp_clk_cpu_freq(); Serial.print("CPU Frequency: "); Serial.print(cpu_freq); Serial.println(" Hz"); }
// void displayApbFrequency() { int apb_freq = esp_clk_apb_freq(); Serial.print("APB Frequency: "); Serial.print(apb_freq); Serial.println(" Hz"); }
// void displayRtcTime() { uint64_t rtc_time = esp_clk_rtc_time(); Serial.print("RTC Time: "); Serial.print(rtc_time); Serial.println(" microseconds"); }



void setup() {
  //Инициализируем сериал-монитор со скоростью 115200
  Serial.begin(9600);
  pinMode(15, OUTPUT);

  //Serial.printf("Chip Model %s, ChipRevision %d, Cpu Freq %d, SDK Version %s\n", ESP.getChipModel(), ESP.getChipRevision(), ESP.getCpuFreqMHz(), ESP.getSdkVersion());
    
  //инициализируем дисплей
  displayInit();
  Serial.println("...display is init");
  
  pinMode(LED_PIN, OUTPUT);      //Контакт управления светодиодом
  pinMode(FUN, OUTPUT);          //Контакт управления вентилятором охлаждения

  
  
  
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

  Serial.println(F("........................................................."));
    
  //Устанавливаем наши значения, определённые ранее в структуре config_radio1
  radio_setSettings(radio1, "1");
  #ifdef RADIO_2
  radio_setSettings(radio2, "2");
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
    Serial.println(F("Sending first packet ... "));

    String str = F("START!");
    transmit_and_print_data(str);
    digitalWrite(LED_PIN, LOW);     //Включаем светодиод, сигнализация об передаче/приёма пакета
    delay(2000);

  #endif
  

  Serial.println(" ");

  digitalWrite(LED_PIN, HIGH);      //Выключаем светодиод, сигнализация об окончании передачи/приёма пакета

  #ifdef TRANSMITTER
  //Если мощность усилителя передатчика E32 900M30S больше 200 милливат (вы можете установить своё значение),
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
  delay(100);
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



