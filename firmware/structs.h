/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        structs.h                                    Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2021/04/25 18:46:02 by rFeijo                                              */
/*        Updated: 2022/01/05 20:10:59 by rFeijo                                              */
/*                                                                       All rights reserved  */
/**********************************************************************************************/

#include <map>

#ifndef _GPIOS_H_
  #define _GPIOS_H

/**
 * Definição da estrutura de GPIOs do tipo 'Analog Outputs'
 */
struct bettas_house_analog_outputs_t {
  uint8_t ch;
  
  String tag;

  int output;

  bool pull_up_enable;
};

/**
 * Definição da estrutura de GPIOs do tipo 'Digital Inputs'
 */
struct bettas_house_digital_inputs_t {
  uint8_t ch;

  String tag;

  unsigned long timer;

  bool pull_up_enable;
  bool event_flag;

  uint8_t debounce_interval;
};

/**
 * Definição da estrutura de controle de cores
 */
struct bettas_house_colors {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

/**
 * Definição da estrutura do dispositivo 'RGB LED Strip' no SinricPro
 */
struct bettas_house_sinricpro_device {
    bool state;
    
    bettas_house_colors color;
    int color_temp;
    
    int brightness;
};

#endif /*_GPIOS_H_*/
