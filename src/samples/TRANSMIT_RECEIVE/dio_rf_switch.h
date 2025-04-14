#ifndef ____dio_rf_switch_____
#define ____dio_rf_switch_____

#include <RadioLib.h>
#include <Arduino.h>

/**
 * @brief Это настроечный файл для задания режима работы пинов DIO LR1121
 * 
 */

//"radio_rfsw_ctrl":[15,0,4,12,4,2,0,1],

static const Module::RfSwitchMode_t rfswitch_table_1[] = {
  // mode                  DIO5  DIO6  DIO7  DIO8
  { LR11x0::MODE_STBY,   { LOW,  LOW,  LOW,  LOW } },
  { LR11x0::MODE_RX,     { HIGH,  LOW,  LOW,  LOW } },
  { LR11x0::MODE_TX,     { HIGH,  HIGH,  LOW,  LOW } },     //включает режим маломощного ТХ (ниже 17ДБм)
  { LR11x0::MODE_TX_HP,  { LOW,  HIGH,  LOW,  LOW} },       //вкллючает режим ТХ с усилением (выше 17ДБм)
  
  
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
  
  
  { LR11x0::MODE_TX_HF,  { LOW,  LOW, LOW,  LOW  } },
  { LR11x0::MODE_GNSS,   { LOW,  LOW,  LOW,  LOW  } },
  { LR11x0::MODE_WIFI,   { LOW,  LOW,  LOW,  LOW  } },
  END_OF_MODE_TABLE,
};



#endif