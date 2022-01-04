/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        Bettas_House.ino                             Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2020/08/02 22:28:24 by rFeijo                                              */
/*        Updated: 2022/01/04 16:11:13 by rFeijo                                              */
/*                                                                       All rights reserved  */
/**********************************************************************************************/

#include <user_interface.h>
#include "config.h"
#include "system.h"

bettas_house_analog_outputs_t led_wifi   = {.ch = 2,  .tag = "WiFi Status LED",     .output = 0, .pull_up_enable = true};
bettas_house_digital_inputs_t sw_boot    = {.ch = 0,  .tag = "Boot Switch",         .timer  = 0, .pull_up_enable = true, .debounce_interval = 20};

bettas_house_system_t  sys(&sw_boot, &led_wifi);

ICACHE_RAM_ATTR void ISR_reboot_switch(void) {
  sw_boot.event_flag = true;
}

void setup(void) {
  sys.initialize_serial_console();

  sys.print_system_info();

  if (sys.WiFi_connect("", "") != WL_CONNECTED)
    sys.AP_start_portal(false);

  sys.initialize_analog_output(&led_wifi);

  sys.initialize_reboot_switch(&sw_boot);
  attachInterrupt(digitalPinToInterrupt(sw_boot.ch), ISR_reboot_switch, CHANGE);
}

void loop(void) {
  sys.WiFi_get_status();

  if (sw_boot.event_flag) {
    sys.reboot_switch_routine();
  }
}
