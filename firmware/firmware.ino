/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        firmware.ino                                 Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2020/08/02 22:28:24 by rFeijo                                              */
/*        Updated: 2022/01/05 20:10:59 by rFeijo                                              */
/*                                                                       All rights reserved  */
/**********************************************************************************************/

#include <user_interface.h>
#include <SinricPro.h>
#include <SinricProLight.h>
#include "config.h"
#include "system.h"
#include "password.h"

bettas_house_analog_outputs_t led_wifi   = {.ch = 2,  .tag = "WiFi Status LED",     .output = 0, .pull_up_enable = true};
bettas_house_digital_inputs_t sw_boot    = {.ch = 0,  .tag = "Boot Switch",         .timer  = 0, .pull_up_enable = true, .debounce_interval = 20};

bettas_house_analog_outputs_t r_led_pin = {.ch = 12,  .tag = "Red LED Pin",   .output = 0, .pull_up_enable = true};
bettas_house_analog_outputs_t g_led_pin = {.ch = 13,  .tag = "Green LED Pin", .output = 0, .pull_up_enable = true};
bettas_house_analog_outputs_t b_led_pin = {.ch = 14,  .tag = "Blue LED Pin",  .output = 0, .pull_up_enable = true};

bettas_house_system_t  sys(&sw_boot, &led_wifi);
SinricProLight &RGB_LED_Strip = SinricPro[SINRICPRO_DEVICE_ID];

using ColorTemperatures = std::map<uint16_t, bettas_house_colors>;
ColorTemperatures colorTemperatures{
//  {Temp, {R,   G,   B}}
    {2000, {255, 138, 18}},
    {2200, {255, 147, 44}},
    {2700, {255, 169, 87}},
    {3000, {255, 180, 107}},
    {4000, {255, 209, 163}},
    {5000, {255, 228, 206}},
    {5500, {255, 236, 224}},
    {6000, {255, 243, 239}},
    {6500, {255, 249, 253}},
    {7000, {245, 243, 255}},
    {7500, {235, 238, 255}},
    {9000, {214, 225, 255}}};

bettas_house_sinricpro_device LED_Strip = {.state = false, .color = colorTemperatures[9000], .color_temp = 9000, .brightness = 100};

IRAM_ATTR void ISR_reboot_switch(void) {
  sw_boot.event_flag = true;
}

void set_RGB_LED_Strip(void) {
  int rValue = map(LED_Strip.color.r * LED_Strip.brightness, 0, 255 * 100, ANALOG_GPIO_MIN_OUTPUT, ANALOG_GPIO_MAX_OUTPUT);
  int gValue = map(LED_Strip.color.g * LED_Strip.brightness, 0, 255 * 100, ANALOG_GPIO_MIN_OUTPUT, ANALOG_GPIO_MAX_OUTPUT);
  int bValue = map(LED_Strip.color.b * LED_Strip.brightness, 0, 255 * 100, ANALOG_GPIO_MIN_OUTPUT, ANALOG_GPIO_MAX_OUTPUT);

  if (LED_Strip.state == false) {
    sys.set_analog_output(&r_led_pin, ANALOG_GPIO_MIN_OUTPUT);
    sys.set_analog_output(&g_led_pin, ANALOG_GPIO_MIN_OUTPUT);
    sys.set_analog_output(&b_led_pin, ANALOG_GPIO_MIN_OUTPUT);
  }
  else {
    sys.set_analog_output(&r_led_pin, rValue);
    sys.set_analog_output(&g_led_pin, gValue);
    sys.set_analog_output(&b_led_pin, bValue);
  }
}

bool ISR_LED_STRIP_onPower(const String &deviceId, bool &state) {
  LED_Strip.state = state;

  set_RGB_LED_Strip();

  return (true);
}

bool ISR_LED_STRIP_onBrightness(const String &deviceId, int &brightness) {
  LED_Strip.brightness = brightness;

  set_RGB_LED_Strip();

  return (true);
}

bool ISR_LED_STRIP_onAdjustBrightness(const String &deviceId, int &brightnessDelta) {
  LED_Strip.brightness += brightnessDelta;

  brightnessDelta = LED_Strip.brightness;

  set_RGB_LED_Strip();

  return (true);
}

bool ISR_LED_STRIP_onColor(const String &deviceId, byte &r, byte &g, byte &b) {
  LED_Strip.color.r = r;
  LED_Strip.color.g = g;
  LED_Strip.color.b = b;

  set_RGB_LED_Strip();

  return (true);
}

bool ISR_LED_STRIP_onColorTemperature(const String &deviceId, int &colorTemperature) {
  LED_Strip.color = colorTemperatures[colorTemperature];

  LED_Strip.color_temp = colorTemperature;

  set_RGB_LED_Strip();

  return (true);
}

bool ISR_LED_STRIP_onDecreaseColorTemperature(const String &deviceId, int &colorTemperature) {
  auto current = colorTemperatures.find(LED_Strip.color_temp);
  auto next    = std::prev(current);

  if (next == colorTemperatures.end())
    next = current;

  LED_Strip.color = next->second;
  LED_Strip.color_temp = next->first;
  colorTemperature = LED_Strip.color_temp;

  set_RGB_LED_Strip();
  
  return (true);
}

bool ISR_LED_STRIP_onIncreaseColorTemperature(const String &deviceId, int &colorTemperature) {
  auto current = colorTemperatures.find(LED_Strip.color_temp);
  auto next    = std::next(current);

  if (next == colorTemperatures.end())
    next = current;

  LED_Strip.color = next->second;
  LED_Strip.color_temp = next->first;
  colorTemperature = LED_Strip.color_temp;

  set_RGB_LED_Strip();

  return (true);
}

void setup(void) {
  sys.initialize_serial_console();

  sys.print_system_info();

  sys.initialize_analog_output(&led_wifi);

  sys.initialize_analog_output(&r_led_pin);
  sys.initialize_analog_output(&g_led_pin);
  sys.initialize_analog_output(&b_led_pin);

  if (sys.WiFi_connect("", "") != WL_CONNECTED)
    sys.AP_start_portal(false);

  sys.initialize_reboot_switch(&sw_boot);
  attachInterrupt(digitalPinToInterrupt(sw_boot.ch), ISR_reboot_switch, CHANGE);

  RGB_LED_Strip.onPowerState(ISR_LED_STRIP_onPower);
  RGB_LED_Strip.onBrightness(ISR_LED_STRIP_onBrightness);
  RGB_LED_Strip.onAdjustBrightness(ISR_LED_STRIP_onAdjustBrightness);
  RGB_LED_Strip.onColor(ISR_LED_STRIP_onColor);
  RGB_LED_Strip.onColorTemperature(ISR_LED_STRIP_onColorTemperature);
  RGB_LED_Strip.onDecreaseColorTemperature(ISR_LED_STRIP_onDecreaseColorTemperature);
  RGB_LED_Strip.onIncreaseColorTemperature(ISR_LED_STRIP_onIncreaseColorTemperature);

  SinricPro.begin(SINRICPRO_APP_KEY, SINRICPRO_APP_PASS);
}

void loop(void) {
  sys.WiFi_get_status();

  if (sw_boot.event_flag) {
    sys.reboot_switch_routine();
  }

  if (sys.wifi_connected) {
    SinricPro.handle();
  }
}
