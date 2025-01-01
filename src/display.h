#ifndef ______DISPLAY__DATA_______
#define ______DISPLAY__DATA_______

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128              // Ширина дисплея в пикселах
#define SCREEN_HEIGHT 64              // Высота дисплея в пикселах
#define OLED_RESET    -1              // Пин сброса # ( -1 если для сброса используется стандартный пин ардуино)
#define SCREEN_ADDRESS 0x3C           // Стандартный адрес I2C для дисплея (в моём случае такой адрес дал I2C-сканнер)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Создаём объект дисплея






/**
* @brief Функция инициализации дисплея 
* 
*/
void displayInit()
{

  //Инициализируем дисплей
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {// SSD1306_SWITCHCAPVCC = напряжение дисплея от 3.3V
    Serial.println(F("SSD1306 ERROR INIT"));
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

  display.setCursor(20, 5);
  display.print(F("DISPLAY INIT"));
  display.display();
  delay(2000);
  display.clearDisplay();
  
}




/**
 * @brief Функция для вывода на экран дисплея состояния
 * работы текущего радио
 * 
 * @param x - координата экрана по Х
 * @param y - координата экрана по У
 * @param RadioName - наименование радио
 * @param state - текущее состояние в виде строки STRING
 */
void displayPrintState(int16_t x, int16_t y, String &RadioName, String state)
{
  String str = RadioName + " : " + state;
  display.setCursor(x, y);
  display.print(str);
  display.display();
}





#endif