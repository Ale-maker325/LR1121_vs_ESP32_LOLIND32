#ifndef _____PINS_____
#define _____PINS_____

#include <Arduino.h>
#include <samples/LORA_TRANSMIT_RECEIVE/precompiler_flags.h>

#define UNDEF_PIN (uint32_t)1001

const uint8_t FUN = 15;             // Пин управлением вентилятором охлаждения
uint8_t LED_PIN = 5;                // Контакт управления светодиодом (на плате LOLIN D32 он на пине 5)

const uint32_t NSS_PIN_1 = 27;
const uint32_t IRQ_PIN_1 = 16;
const uint32_t NRST_PIN_1 = 14;
const uint32_t BUSY_PIN_1 = 32;


const uint32_t NSS_PIN_2 = 13;
const uint32_t IRQ_PIN_2 = 34;
const uint32_t NRST_PIN_2 = 17;
const uint32_t BUSY_PIN_2 = 33;




#endif