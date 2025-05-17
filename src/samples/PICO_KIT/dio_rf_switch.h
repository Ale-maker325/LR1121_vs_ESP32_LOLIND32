#ifndef ____dio_rf_switch_____
#define ____dio_rf_switch_____

#include <RadioLib.h>
#include <Arduino.h>

//Раскомментировать необходимое оборудование
#define EBYTE_E80_400M2213S
//#define GniceRF_LORA400

#ifdef EBYTE_E80_400M2213S

  #ifdef TRANSMITTER
    //********************************************************************************************/
    //**************************              TX                **********************************/
    //********************************************************************************************/
                      /** (Для ExpressLRS будут слещующие настройки: 15,0,0,0,8,12,0,0)
                       *  ДЛя модуля EBYTE_E80_400M2213S будут слещующие настройки
                       *   --------------------------------------------------
                       *  DIO5  *	DIO6  *                                  *
                       *   0	  *   0	  *    RX                            *
                       *   0	  *   1	  *    TX (Sub-1GHz low power mode)  *
                       *   1	  *   0	  *    TX (Sub-1GHz high power mode) *
                       *   1	  *   1	  *    TX（2.4GHz）                   *
                        ---------------------------------------------------
                      */

      static const Module::RfSwitchMode_t rfswitch_table_1[] = {
        // mode                  DIO5  DIO6  DIO7  DIO8
        { LR11x0::MODE_STBY,   { LOW,  LOW,  LOW,  LOW } },
        { LR11x0::MODE_RX,     { LOW,  LOW,  LOW,  LOW } },
        { LR11x0::MODE_TX,     { LOW,  LOW,  LOW,  LOW } },
        { LR11x0::MODE_TX_HP,  { HIGH, LOW,  LOW,  LOW } },
        { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
        { LR11x0::MODE_GNSS,   { LOW,  LOW,  LOW,  LOW  } },
        { LR11x0::MODE_WIFI,   { LOW,  LOW,  LOW,  LOW  } },
        END_OF_MODE_TABLE,
      };

      static const Module::RfSwitchMode_t rfswitch_table_2[] = {
        // mode                  DIO5  DIO6  DIO7  DIO8
        { LR11x0::MODE_STBY,   { LOW,  LOW,  LOW,  LOW } },
        { LR11x0::MODE_RX,     { LOW,  LOW,  LOW,  LOW } },
        { LR11x0::MODE_TX,     { LOW,  LOW,  LOW,  LOW } },
        { LR11x0::MODE_TX_HP,  { LOW,  LOW,  LOW,  LOW} },
        { LR11x0::MODE_TX_HF,  { HIGH, HIGH, LOW,  LOW  } },
        { LR11x0::MODE_GNSS,   { LOW,  LOW,  LOW,  LOW  } },
        { LR11x0::MODE_WIFI,   { LOW,  LOW,  LOW,  LOW  } },
        END_OF_MODE_TABLE,
      };
  #endif

  #ifdef RECEIVER
  //********************************************************************************************/
  //**************************              RX                **********************************/
  //********************************************************************************************/
  
    static const Module::RfSwitchMode_t rfswitch_table_1[] = {
      // mode                  DIO5  DIO6  DIO7  DIO8
      { LR11x0::MODE_STBY,   { LOW,  LOW,  LOW,  LOW } },
      { LR11x0::MODE_RX,     { LOW,  LOW,  LOW,  LOW } },
      { LR11x0::MODE_TX,     { LOW,  LOW,  LOW,  LOW } },
      { LR11x0::MODE_TX_HP,  { LOW, LOW,  LOW,  LOW } },
      { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
      { LR11x0::MODE_GNSS,   { LOW,  LOW,  LOW,  LOW  } },
      { LR11x0::MODE_WIFI,   { LOW,  LOW,  LOW,  LOW  } },
      END_OF_MODE_TABLE,
    };

    static const Module::RfSwitchMode_t rfswitch_table_2[] = {
      // mode                  DIO5  DIO6  DIO7  DIO8
      { LR11x0::MODE_STBY,   { LOW,  LOW,  LOW,  LOW } },
      { LR11x0::MODE_RX,     { LOW,  LOW,  LOW,  LOW } },
      { LR11x0::MODE_TX,     { LOW,  LOW,  LOW,  LOW } },
      { LR11x0::MODE_TX_HP,  { LOW,  LOW,  LOW,  LOW} },
      { LR11x0::MODE_TX_HF,  { LOW,  LOW,  LOW,  LOW  } },
      { LR11x0::MODE_GNSS,   { LOW,  LOW,  LOW,  LOW  } },
      { LR11x0::MODE_WIFI,   { LOW,  LOW,  LOW,  LOW  } },
      END_OF_MODE_TABLE,
    };
  #endif
#endif


//Настройка для модулей GniceRF_LORA400

#ifdef  GniceRF_LORA400
  static const Module::RfSwitchMode_t rfswitch_table_1[] = {
    // mode                  DIO5  DIO6  DIO7  DIO8
    { LR11x0::MODE_STBY,   { LOW, LOW, LOW, LOW } },
    { LR11x0::MODE_RX,     { LOW, LOW, LOW, LOW } },
    { LR11x0::MODE_TX,     { LOW, LOW, LOW, LOW } },
    { LR11x0::MODE_TX_HP,  { LOW, LOW, LOW, LOW } },
    
    
    { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
    { LR11x0::MODE_GNSS,   { LOW,  LOW, LOW,  LOW  } },
    { LR11x0::MODE_WIFI,   { LOW,  LOW, LOW,  LOW  } },
    END_OF_MODE_TABLE,
  };

  static const Module::RfSwitchMode_t rfswitch_table_2[] = {
    // mode                  DIO5  DIO6  DIO7  DIO8
    { LR11x0::MODE_STBY,   { LOW, LOW, LOW, LOW } },
    { LR11x0::MODE_RX,     { LOW, LOW, LOW, LOW } },
    { LR11x0::MODE_TX,     { LOW, LOW, LOW, LOW } },
    { LR11x0::MODE_TX_HP,  { LOW, LOW, LOW, LOW } },
    
    
    { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
    { LR11x0::MODE_GNSS,   { LOW,  LOW, LOW,  LOW  } },
    { LR11x0::MODE_WIFI,   { LOW,  LOW, LOW,  LOW  } },
    END_OF_MODE_TABLE,
  };
#endif


#endif