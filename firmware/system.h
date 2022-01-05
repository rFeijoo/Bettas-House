/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        system.h                                     Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2020/08/02 22:28:24 by rFeijo                                              */
/*        Updated: 2022/01/05 20:10:59 by rFeijo                         All rights reserved  */
/**********************************************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "webserver.h"
#include "config.h"
#include "structs.h"
#include "password.h"

#ifndef _SYSTEM_H_
  #define _SYSTEM_H_

/**
 * Diretivas de configuração do Serial Console
 */
#define SERIAL_BAUD_RATE        115200
#define SERIAL_LINE_BREAK       "\n"

/**
 * Diretivas de configuração do módulo WiFi
 */
#define WIFI_WAIT_FOR_CONNECTION_TIMEOUT  10000
#define WIFI_GET_STATUS_INTERVAL          2500
#define WIFI_MINIMUM_RSSI                 20

/**
 * Diretivas de configuração do módulo AP (LAN)
 */
#define AP_WEBSERVER_PORT 80
#define AP_DNS_PORT       53
#define AP_PORTAL_TIMEOUT 120000
#define AP_RESTART_DELAY  5000
#define AP_REBOOT_DELAY   5000
#define AP_EXIT_DELAY     5000

/**
 * Diretivas de configuração do SWITCH integrado
 */
#define REBOOT_SW_REBOOT_INTERVAL   5000  // 10 segundos
#define REBOOT_SW_START_AP_INTERVAL 1000  // 1 segundo

class bettas_house_system_t {
  public:
    String SYSTEM_MAC_ADDR;
    
    bool   wifi_connected = false;

    bettas_house_system_t(bettas_house_digital_inputs_t *sw, bettas_house_analog_outputs_t *l);
    
    void   initialize_serial_console(void);
    void   initialize_analog_output(bettas_house_analog_outputs_t *gpio);
    void   initialize_reboot_switch(bettas_house_digital_inputs_t *gpio);
    void   set_analog_output(bettas_house_analog_outputs_t *ptr, int value);
    bool   get_switch_state(bettas_house_digital_inputs_t *sw);
    String get_mac_address(void);

    int    WiFi_connect(String SSID, String PASSWORD);
    void   WiFi_get_status(void);
    bool   AP_start_portal(bool timeout);
    
    String get_mac_addr(void);
    void   print_system_info(void);
    void   serial_print(String msg, uint8_t level);

    void   system_restart(void);
    void   system_reboot(void);

    void   reboot_switch_routine(void);

  private:
    std::unique_ptr<DNSServer>        dns_server;
    std::unique_ptr<ESP8266WebServer> web_server;

    bettas_house_digital_inputs_t *button;
    bettas_house_analog_outputs_t *led;

    bool   WiFi_has_saved_credentials;
    String WiFi_SSID, WiFi_PASSWORD;

    bool AP_force_exit            = false;
    bool AP_advanced_login_failed = false;
    
    unsigned long WiFi_status_time_tracker;
    unsigned long AP_timeout_time_tracker;

    void   WiFi_set_op_mode(void);
    void   WiFi_reconnect_to_saved_credentials(void);
    bool   WiFi_test_credentials(void);
    int    WiFi_wait_for_connection_result(void);
    void   WiFi_print_status(int status);
    void   WiFi_get_rssi(void);

    bool   AP_portal_is_timeouted(void);
    void   AP_setup_portal(void);
    void   AP_push_webserver(void);
    void   AP_handle_root(void);
    void   AP_handle_scan_WiFi(void);
    void   AP_handle_save_credentials(void);
    void   AP_handle_about(void);
    void   AP_handle_restart(void);
    void   AP_handle_exit(void);
    void   AP_handle_login_advanced(void);
    void   AP_handle_auth_user_advanced(void);
    void   AP_handle_advanced(void);
    void   AP_handle_confirm_reboot(void);
    void   AP_handle_reboot(void);
    void   AP_handle_not_found(void);
    bool   AP_captive_portal(void);
    bool   AP_has_valid_IP(String str);
    String AP_convert_IP_to_string(IPAddress ip);
    void   AP_order_SSIDs_by_rssi(int *list, int size);
    void   AP_remove_duplicated_SSIDs(int *list, int size);
    String AP_remove_by_RSSI(int *list, int size);
    int    AP_RSSI_to_percentage(int RSSI);
    String AP_mili_seconds_to_minutes(int ms);

    void   (*AP_callback)(bettas_house_system_t*) = NULL;
    void   (*_savecallback)(void) = NULL;
};

#endif /*_SYSTEM_H_*/
