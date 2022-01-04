/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        gpios.h                                      Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2021/04/25 18:46:02 by rFeijo                                              */
/*        Updated: 2022/01/04 16:11:13 by rFeijo                                              */
/*                                                                       All rights reserved  */
/**********************************************************************************************/

#ifndef _GPIOS_H_
  #define _GPIOS_H

/**
 * Definição da estrutura de GPIOs do tipo 'Analog Outputs'.
 */
struct bettas_house_analog_outputs_t {
  uint8_t ch;
  
  String tag;

  int output;

  bool pull_up_enable;
};

/**
 * Definição da estrutura de GPIOs do tipo 'Digital Inputs'.
 */
struct bettas_house_digital_inputs_t {
  uint8_t ch;

  String tag;

  unsigned long timer;

  bool pull_up_enable;
  bool event_flag;

  uint8_t debounce_interval;
};

#endif /*_GPIOS_H_*/
