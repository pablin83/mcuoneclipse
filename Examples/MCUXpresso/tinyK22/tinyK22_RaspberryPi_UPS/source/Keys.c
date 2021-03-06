/**
 * \file
 * \brief Key/Switch driver implementation.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This module implements a generic keyboard driver for up to 7 Keys.
 * It is using macros for maximum flexibility with minimal code overhead.
 */

#include "platform.h"
#if PL_CONFIG_HAS_KEYS
#include "Keys.h"
#include "McuWait.h"
#include "McuGPIO.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "leds.h"
#include "lv.h"
#include "Debounce.h"
#include "KeyDebounce.h"
#include "Event.h"
#include "McuRTOS.h"
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  #include "McuSystemView.h"
#endif

static McuGPIO_Handle_t navPinUp, navPinDown, navPinLeft, navPinRight, navPinCenter;

DBNC_KeySet Key_GetKeysBits(void) {
  DBNC_KeySet keys = 0;

#if PL_CONFIG_NOF_KEYS >= 1
  if (McuGPIO_Get(navPinUp)==0) {
    keys |= (1<<0);
  }
#endif
#if PL_CONFIG_NOF_KEYS >= 2
  if (McuGPIO_Get(navPinDown)==0) {
    keys |= (1<<1);
  }
#endif
#if PL_CONFIG_NOF_KEYS >= 3
  if (McuGPIO_Get(navPinLeft)==0) {
    keys |= (1<<2);
  }
#endif
#if PL_CONFIG_NOF_KEYS >= 4
  if (McuGPIO_Get(navPinRight)==0) {
    keys |= (1<<3);
  }
#endif
#if PL_CONFIG_NOF_KEYS >= 5
  if (McuGPIO_Get(navPinCenter)==0) {
    keys |= (1<<4);
  }
#endif
#if PL_CONFIG_NOF_KEYS >= 6
  if (KEY6_Get()) {
    keys |= (1<<5);
  }
#endif
#if PL_CONFIG_NOF_KEYS >= 7
  if (KEY7_Get()) {
    keys |= (1<<6);
  }
#endif
  return keys;
}

void KEY_Scan(void) {
#if PL_CONFIG_NOF_KEYS>=1 && !PL_CONFIG_KEY_1_ISR
  if (McuGPIO_Get(navPinUp)==0) {
    EVNT_SetEvent(EVNT_SW1_PRESSED);
  }
#endif
#if PL_CONFIG_NOF_KEYS>=2 && !PL_CONFIG_KEY_2_ISR
  if (McuGPIO_Get(navPinDown)==0) {
    EVNT_SetEvent(EVNT_SW2_PRESSED);
  }
#endif
#if PL_CONFIG_NOF_KEYS>=3 && !PL_CONFIG_KEY_3_ISR
  if (McuGPIO_Get(navPinLeft)==0) {
    EVNT_SetEvent(EVNT_SW3_PRESSED);
  }
#endif
#if PL_CONFIG_NOF_KEYS>=4 && !PL_CONFIG_KEY_4_ISR
  if (McuGPIO_Get(navPinRight)==0) {
    EVNT_SetEvent(EVNT_SW4_PRESSED);
  }
#endif
#if PL_CONFIG_NOF_KEYS>=5 && !PL_CONFIG_KEY_5_ISR
  if (McuGPIO_Get(navPinCenter)==0) {
    EVNT_SetEvent(EVNT_SW5_PRESSED);
  }
#endif
#if PL_CONFIG_NOF_KEYS>=6 && !PL_CONFIG_KEY_6_ISR
  if (KEY6_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW6_PRESSED);
  }
#endif
#if PL_CONFIG_NOF_KEYS>=7 && !PL_CONFIG_KEY_7_ISR
  if (KEY7_Get()) { /* key pressed */
    EVNT_SetEvent(EVNT_SW7_PRESSED);
  }
#endif
}

#if PL_CONFIG_HAS_KBI
void KEY_EnableInterrupts(void) {
#if PL_CONFIG_KEY_1_ISR
  EnableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_2_ISR
  EnableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_3_ISR
  EnableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_4_ISR
  EnableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_5_ISR
  EnableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_6_ISR
  SW6_Enable();
#endif
#if PL_CONFIG_KEY_7_ISR
  SW7_Enable();
#endif
}
#endif /* PL_CONFIG_HAS_KBI */

#if PL_CONFIG_HAS_KBI
void KEY_DisableInterrupts(void) {
#if PL_CONFIG_KEY_1_ISR
  DisableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_2_ISR
  DisableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_3_ISR
  DisableIRQ(PORTB_IRQn);;
#endif
#if PL_CONFIG_KEY_4_ISR
  DisableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_5_ISR
  DisableIRQ(PORTB_IRQn);
#endif
#if PL_CONFIG_KEY_6_ISR
  SW6_Disable();
#endif
#if PL_CONFIG_KEY_7_ISR
  SW7_Disable();
#endif
}
#endif /* PL_CONFIG_HAS_KBI */

#if PL_CONFIG_HAS_KBI
void KEY_OnInterrupt(KEY_Buttons button) {
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  McuSystemView_RecordEnterISR();
#endif
#if PL_CONFIG_HAS_DEBOUNCE
  (void)button;
  KEYDBNC_Process(); /* debounce key(s) */
#else
  /* no debounce, only setting event */
  switch(button) {
#if PL_CONFIG_NOF_KEYS>=1
    case KEY_BTN1:
      EVNT_SetEvent(EVNT_SW1_PRESSED);
      break;
#endif
#if PL_CONFIG_NOF_KEYS>=2
    case KEY_BTN2:
      EVNT_SetEvent(EVNT_SW2_PRESSED);
      break;
#endif
#if PL_CONFIG_NOF_KEYS>=3
    case KEY_BTN3:
      EVNT_SetEvent(EVNT_SW3_PRESSED);
      break;
#endif
#if PL_CONFIG_NOF_KEYS>=4
    case KEY_BTN4:
      EVNT_SetEvent(EVNT_SW4_PRESSED);
      break;
#endif
#if PL_CONFIG_NOF_KEYS>=5
    case KEY_BTN5:
      EVNT_SetEvent(EVNT_SW5_PRESSED);
      break;
#endif
#if PL_CONFIG_NOF_KEYS>=6
    case KEY_BTN6:
      EVNT_SetEvent(EVNT_SW6_PRESSED);
      break;
#endif
#if PL_CONFIG_NOF_KEYS>=7
    case KEY_BTN7:
      EVNT_SetEvent(EVNT_SW7_PRESSED);
      break;
#endif
    default:
      break;
  }
#endif
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  McuSystemView_RecordExitISR();
#endif
}
#endif /* PL_CONFIG_HAS_KBI */

void PORTB_IRQHandler(void) {
  /* Clear external interrupt flag. */
  uint32_t flags;

  flags = GPIO_PortGetInterruptFlags(GPIOB);
  if (flags&(1U<<PINS_HATNAVLEFT_PIN)) {
    GPIO_PortClearInterruptFlags(PINS_HATNAVLEFT_GPIO, 1U<<PINS_HATNAVLEFT_PIN);
    KEY_OnInterrupt(KEY_BTN1);
  }
  if (flags&(1U<<PINS_HATNAVRIGHT_PIN)) {
    GPIO_PortClearInterruptFlags(PINS_HATNAVRIGHT_GPIO, 1U<<PINS_HATNAVRIGHT_PIN);
    KEY_OnInterrupt(KEY_BTN2);
  }
  if (flags&(1U<<PINS_HATNAVUP_PIN)) {
    GPIO_PortClearInterruptFlags(PINS_HATNAVUP_GPIO, 1U<<PINS_HATNAVUP_PIN);
    KEY_OnInterrupt(KEY_BTN3);
  }
  if (flags&(1U<<PINS_HATNAVDOWN_PIN)) {
    GPIO_PortClearInterruptFlags(PINS_HATNAVDOWN_GPIO, 1U<<PINS_HATNAVDOWN_PIN);
    KEY_OnInterrupt(KEY_BTN4);
  }
  if (flags&(1U<<PINS_HATNAVPUSH_PIN)) {
    GPIO_PortClearInterruptFlags(PINS_HATNAVPUSH_GPIO, 1U<<PINS_HATNAVPUSH_PIN);
    KEY_OnInterrupt(KEY_BTN5);
  }
  __DSB();
}

void KEY_Init(void) {
  McuGPIO_Config_t config;

  McuGPIO_GetDefaultConfig(&config);
  config.isInput = true;

  config.hw.gpio = PINS_HATNAVUP_GPIO;
  config.hw.port = PINS_HATNAVUP_PORT;
  config.hw.pin = PINS_HATNAVUP_PIN;
  navPinUp = McuGPIO_InitGPIO(&config);

  config.hw.gpio = PINS_HATNAVDOWN_GPIO;
  config.hw.port = PINS_HATNAVDOWN_PORT;
  config.hw.pin = PINS_HATNAVDOWN_PIN;
  navPinDown = McuGPIO_InitGPIO(&config);

  config.hw.gpio = PINS_HATNAVLEFT_GPIO;
  config.hw.port = PINS_HATNAVLEFT_PORT;
  config.hw.pin = PINS_HATNAVLEFT_PIN;
  navPinLeft = McuGPIO_InitGPIO(&config);

  config.hw.gpio = PINS_HATNAVRIGHT_GPIO;
  config.hw.port = PINS_HATNAVRIGHT_PORT;
  config.hw.pin = PINS_HATNAVRIGHT_PIN;
  navPinRight = McuGPIO_InitGPIO(&config);

  config.hw.gpio = PINS_HATNAVPUSH_GPIO;
  config.hw.port = PINS_HATNAVPUSH_PORT;
  config.hw.pin = PINS_HATNAVPUSH_PIN;
  navPinCenter = McuGPIO_InitGPIO(&config);
#if PL_CONFIG_HAS_KBI
  PORT_SetPinInterruptConfig(PINS_HATNAVUP_PORT, PINS_HATNAVUP_PIN, kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(PINS_HATNAVDOWN_PORT, PINS_HATNAVDOWN_PIN, kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(PINS_HATNAVLEFT_PORT, PINS_HATNAVLEFT_PIN, kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(PINS_HATNAVRIGHT_PORT, PINS_HATNAVRIGHT_PIN, kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(PINS_HATNAVPUSH_PORT, PINS_HATNAVPUSH_PIN, kPORT_InterruptFallingEdge);
  EnableIRQ(PORTB_IRQn);
#endif
}

/*! \brief Key driver de-initialization */
void KEY_Deinit(void) {
#if PL_CONFIG_HAS_KBI
  DisableIRQ(PORTB_IRQn);
#endif
  navPinDown = McuGPIO_DeinitGPIO(navPinDown);
  navPinUp = McuGPIO_DeinitGPIO(navPinUp);
  navPinLeft = McuGPIO_DeinitGPIO(navPinLeft);
  navPinRight = McuGPIO_DeinitGPIO(navPinRight);
  navPinCenter = McuGPIO_DeinitGPIO(navPinCenter);
}

#endif /* PL_CONFIG_HAS_KEYS */
