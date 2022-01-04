/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        system.cpp                                   Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2020/08/02 22:28:24 by rFeijo                                              */
/*        Updated: 2022/01/04 16:11:13 by rFeijo                                              */
/*                                                                       All rights reserved  */
/**********************************************************************************************/

#include "system.h"

bettas_house_system_t::bettas_house_system_t(bettas_house_digital_inputs_t *sw, bettas_house_analog_outputs_t *l) {
  button = sw;
  led    = l;

  WiFi_status_time_tracker = millis();

  SYSTEM_MAC_ADDR = get_mac_addr();
}

/** 
 * @brief Inicializa o Serial Console, quando com o modo DEBUG ativado.
 */
void bettas_house_system_t::initialize_serial_console(void) {
  #ifdef SYSTEM_DEBUG_MODE_ON
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial)
      ;

    Serial.print(SERIAL_LINE_BREAK);

    serial_print("Main - Debug Mode On", 1);
  #endif /*SYSTEM_DEBUG_MODE_ON*/
}

/** 
 * @brief Inicializa a conexão WiFi com o ponto de acesso passado como referência.
 * 
 * @param SSID   SSID do ponto de acesso passado como referência.
 * @param PASS   Senha do ponto de acesso passado como referência.
 * @param return Status atual de conexão WiFi.
 */
int bettas_house_system_t::WiFi_connect(String SSID, String PASS) {
  serial_print("WiFi - Initializing station...", 2);

  if (WiFi.status() == WL_CONNECTED && (WiFi.SSID() == SSID)) {
    serial_print("WiFi - Already connected", 1);
    
    return (WL_CONNECTED);
  }
  
  if (SSID != "") {
    ETS_UART_INTR_DISABLE();
    
    wifi_station_disconnect();
    
    ETS_UART_INTR_ENABLE();
    
    WiFi.begin(SSID.c_str(), PASS.c_str(), 0, NULL, true);
  }
  else
    WiFi_reconnect_to_saved_credentials();

  int wifi_connection_result = WiFi_wait_for_connection_result();
  
  return (wifi_connection_result);
}

/** 
 * @brief Seta o modo de funcionamento do módulo WiFi.
 */
void bettas_house_system_t::WiFi_set_op_mode(void) {
  WiFi.persistent(false);
  WiFi.disconnect();
  
  WiFi.mode(WIFI_AP);
  WiFi.persistent(true);

  serial_print("WiFi - Mode: Access Point", 2);
}

/** 
 * @brief Busca por credenciais WiFi conectadas anteriormente.
 */
void bettas_house_system_t::WiFi_reconnect_to_saved_credentials(void) {
  if (WiFi.SSID() != "") {
    serial_print("WiFi - Using Saved Credentials", 2);
    
    ETS_UART_INTR_DISABLE();
    wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();
    
    WiFi.begin();
  }
  else
    serial_print("WiFi - No Saved Credentials", 2);
}

/** 
 * @brief Tenta se conectar a rede WiFi utilizando as credenciais fornecidas pelo Portal de Configuração do Sistema.
 * 
 * @param return Status da conexão WiFi.
 */
bool bettas_house_system_t::WiFi_test_credentials(void) {
  serial_print("WiFi - Connecting to \"" + WiFi_SSID + "\"...", 1);
  
  delay(1000);

  if (WiFi_connect(WiFi_SSID, WiFi_PASSWORD) != WL_CONNECTED) {
    serial_print("WiFi - Failed to connect", 1);
    
    delay(2000);
  }
  else {
    serial_print("WiFi - Connected", 1);
    
    WiFi.softAPdisconnect();
    
    WiFi.mode(WIFI_STA);
    
    if (_savecallback != NULL)
      _savecallback();

    return (true);
  }

  return (false);
}

/** 
 * @brief Busca por credenciais WiFi armazenadas na ROM.
 * 
 * @param return Status da conexão WiFi.
 */
int bettas_house_system_t::WiFi_wait_for_connection_result(void) {
  serial_print("WiFi - Waiting for Connection Result", 3);
  
  unsigned long timer = millis();
  uint8_t status;
  
  while (1) {
    status = WiFi.status();
    
    if (millis() > timer + WIFI_WAIT_FOR_CONNECTION_TIMEOUT) {
      serial_print("WiFi - Wait for Connection Timeout Reached", 2);

      break;
    }
    else if (status == WL_CONNECTED) {
      break;
    }
    
    delay(100);
  }

  WiFi_print_status(status);
  
  return (status);
}

/** 
 * @brief Obtém a intensidade do sinal (RSSI) e seta o resultado no LED.
 */
void bettas_house_system_t::WiFi_get_rssi(void) {
  long RSSI = WiFi.RSSI();

  int PWM = int((AP_RSSI_to_percentage(RSSI) / 100.0) * ANALOG_GPIO_MAX_OUTPUT);

  serial_print("WiFi - Status: Connected, RSSI: " + String(RSSI) + " dBm", 3);

  set_analog_output(led, PWM);
}

/** 
 * @brief Gerencia o status de conexão do WiFi.
 */
void bettas_house_system_t::WiFi_get_status(void) {
  if (millis() > WiFi_status_time_tracker + WIFI_GET_STATUS_INTERVAL) {
    int wifi_status = WiFi.status();
    
    if (wifi_status == WL_CONNECTED) {
      WiFi_get_rssi();

      wifi_connected = true;
    }
    else {
      set_analog_output(led, 0);
      
      wifi_connected = false;
    }

    WiFi_print_status(wifi_status);

    WiFi_status_time_tracker = millis();
  }
}

/** 
 * @brief Com o modo DEBUG ativado, aprenseta o status da conexão WiFi.
 * 
 * @param status Status da conexão WiFi.
 */
void bettas_house_system_t::WiFi_print_status(int status) {
  if (status == WL_CONNECTED) {
    WiFi_get_rssi();
  }
  else if (status == WL_IDLE_STATUS) {
    serial_print("WiFi - Status: Trying to Connect", 3);
  }
  else if (status == WL_NO_SSID_AVAIL) {
    serial_print("WiFi - Status: No SSIDs Available", 3);
  }
  else if (status == WL_CONNECT_FAILED) {
    serial_print("WiFi - Status: Connection Failed", 3);
  }
  else if (status == WL_CONNECTION_LOST) {
    serial_print("WiFi - Status: Connection Lost", 3);
  }
  else if (status == WL_DISCONNECTED) {
    serial_print("WiFi - Status: Disconnected", 3);
  }
  else {
    serial_print("WiFi - Status: Undefined", 3);
  }
}

/** 
 * @brief Configura e inicializa o Portal de Configuração do Sistema.
 * 
 * @param timeout Habilita/Desabilita o timeout do portal.
 * @param return  Status da conexão WiFi.
 */
bool bettas_house_system_t::AP_start_portal(bool timeout) {
  AP_setup_portal();

  AP_timeout_time_tracker = millis();

  while (true) {
    if (timeout && AP_portal_is_timeouted()) {
      serial_print("WebServer - AP Timeout Reached", 1);
      
      WiFi.mode(WIFI_STA);
      
      break;
    }
    
    dns_server->processNextRequest();
    web_server->handleClient();

    if (WiFi_has_saved_credentials) {
      if (WiFi_test_credentials()) {
        break;
      }
    }

    if (AP_force_exit) {
      WiFi_test_credentials();

      break;
    }
    
    yield();
  }

  web_server.reset();
  dns_server.reset();

  return (WiFi.status() == WL_CONNECTED);
}

/** 
 * @brief Verifica se o Portal de Configuração atingiu o tempo limite sem usuário conectado ao portal.
 * 
 * @param return Status do timeout do portal.
 */
bool bettas_house_system_t::AP_portal_is_timeouted(void) {
  if (wifi_softap_get_station_num() > 0) {
    AP_timeout_time_tracker = millis();
    
    return (false);
  }
    
  return (millis() > AP_timeout_time_tracker + AP_PORTAL_TIMEOUT);
}

/** 
 * @brief Configura o direcionamento do WebServer.
 */
void bettas_house_system_t::AP_setup_portal(void) {
  WiFi_set_op_mode();

  if (AP_callback != NULL) {
    AP_callback(this);
  }

  WiFi_has_saved_credentials = false;

  AP_force_exit = false;

  AP_push_webserver();
  
  web_server->on(String(F("/")).c_str(),                  std::bind(&bettas_house_system_t::AP_handle_root, this));
  web_server->on(String(F("/fwlink")).c_str(),            std::bind(&bettas_house_system_t::AP_handle_root, this));
  web_server->on(String(F("/setwifi")).c_str(),           std::bind(&bettas_house_system_t::AP_handle_scan_WiFi, this));
  web_server->on(String(F("/save_credentials")).c_str(),  std::bind(&bettas_house_system_t::AP_handle_save_credentials, this));
  web_server->on(String(F("/about")).c_str(),             std::bind(&bettas_house_system_t::AP_handle_about, this));
  web_server->on(String(F("/restart")).c_str(),           std::bind(&bettas_house_system_t::AP_handle_restart, this));
  web_server->on(String(F("/exit")).c_str(),              std::bind(&bettas_house_system_t::AP_handle_exit, this));
  web_server->on(String(F("/auth_advanced")).c_str(),     std::bind(&bettas_house_system_t::AP_handle_login_advanced, this));
  web_server->on(String(F("/auth_user_advanced")).c_str(),std::bind(&bettas_house_system_t::AP_handle_auth_user_advanced, this));
  web_server->on(String(F("/confirm_reboot")).c_str(),    std::bind(&bettas_house_system_t::AP_handle_confirm_reboot, this));
  web_server->on(String(F("/reboot")).c_str(),            std::bind(&bettas_house_system_t::AP_handle_reboot, this));
  
  web_server->onNotFound(                                 std::bind(&bettas_house_system_t::AP_handle_not_found, this));
  
  web_server->begin();
  
  serial_print("WiFi - HTTP Server Started", 1);
}

/** 
 * @brief Torna o Portal de Configuração do Sistema visível.
 */
void bettas_house_system_t::AP_push_webserver(void) {
  dns_server.reset(new DNSServer());
  web_server.reset(new ESP8266WebServer(AP_WEBSERVER_PORT));

  String AP_NAME = SYSTEM_NAME + String(" ") + get_mac_addr().substring(9, 17);

  WiFi.softAP(AP_NAME, SYSTEM_PASSWORD);

  delay(500);

  serial_print("WiFi - SoftAP IP: " + WiFi.softAPIP().toString(), 1);

  dns_server->setErrorReplyCode(DNSReplyCode::NoError);
  dns_server->start(AP_DNS_PORT, "*", WiFi.softAPIP());
}

/** 
 * @brief Gerencia a página 'Root' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_root(void) {
  serial_print("WebServer - AP Handle Root", 3);
  
  if (AP_captive_portal()) {
    return;
  }

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Options");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  
  page += String(F("<h1>"));
  page += SYSTEM_NAME;
  page += String(F("</h1>"));
  
  page += String(F("<center><h3>System Manager</h3></center>"));
  
  page += FPSTR(HTTP_PORTAL_CONFIG_WIFI);
  page += FPSTR(HTTP_PORTAL_ABOUT);
  page += FPSTR(HTTP_PORTAL_RESTART);
  if (wifi_connected)
    page += FPSTR(HTTP_PORTAL_EXIT);
  page += FPSTR(HTTP_PORTAL_ADVANCED);      
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);
}

/** 
 * @brief Gerencia a página 'Configure WiFi' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_scan_WiFi(void) {
  serial_print("WebServer - AP Handle Scan WiFi", 3);
  
  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Configure WiFi");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);

  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    serial_print("WebServer - No Networks Found", 1);
    
    page += F("No networks found. Refresh the page and try again.");
  }
  else {
    int list[n];
    
    for (int i = 0; i < n; i++) {
      list[i] = i;
    }
    
    AP_order_SSIDs_by_rssi(list, n);

    AP_remove_duplicated_SSIDs(list, n);

    page += AP_remove_by_RSSI(list, n);

    page += "<br/>";
  }

  page += FPSTR(HTTP_SCAN_LINK);
  
  page += FPSTR(HTTP_FORM_START);
  page += FPSTR(HTTP_FORM_END);

  page += FPSTR(HTTP_BACK_BTN);

  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);
}

/** 
 * @brief Gerencia a página 'Save WiFi Credentials' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_save_credentials(void) {
  serial_print("WebServer - AP Handle Save Credentials", 3);

  WiFi_SSID     = web_server->arg("s").c_str();
  WiFi_PASSWORD = web_server->arg("p").c_str();

  serial_print("WebServer - Received Credentials: [SSID=" + WiFi_SSID + "] [PASSWORD=" + WiFi_PASSWORD + "]", 2);

  if (WiFi_SSID == "") {
    AP_handle_scan_WiFi();
  }

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Credentials Saved");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  
  page += String(F("<center><h1>"));
  page += SYSTEM_NAME;
  page += String(F("</h1></center>"));
  
  page += String(F("<center><h3>WiFi Credentials Saved</h3></center>"));
  
  page += FPSTR(HTTP_SAVED);
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);

  WiFi_has_saved_credentials = true;
}

/** 
 * @brief Gerencia a página 'About' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_about(void) {
  serial_print("WebServer - AP Handle About", 3);

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Info");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  
  page += F("<center><h1>");
  page += SYSTEM_NAME;
  page += F("</h1></center>");
  
  page += F("<center><h2>");
  page += SYSTEM_MODEL;
  page += F("</h2></center>");
  
  page += F("<dl>");
  page += F("<dt>Manufacturer:&emsp;&emsp;");
  page += SYSTEM_MANUFACTURER;
  page += F("</dt><br>");
  page += F("<dt>Serial Number:&emsp;   ");
  page += get_mac_addr();
  page += F("</dt><br>");
  page += F("<dt>Firmware:&emsp;&emsp;&emsp;&emsp; ");
  page += SYSTEM_VERSION;
  page += F("</dt></dl>");
  
  page += FPSTR(HTTP_BACK_BTN);
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);
}

/** 
 * @brief Gerencia a página 'Restart' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_restart(void) {
  serial_print("WebServer - AP Handle Restart", 3);

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Restart");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);

  page += F("<center><h1>");
  page += SYSTEM_NAME;
  page += F("</h1></center>");
  
  page += F("<center>Restarting...</center><br>");
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);

  delay(AP_RESTART_DELAY);

  system_restart();
}

/** 
 * @brief Gerencia a página 'Exit' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_exit(void) {
  serial_print("WebServer - AP Handle Exit Portal", 3);

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Exit Portal");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);

  page += F("<center><h1>");
  page += SYSTEM_NAME;
  page += F("</h1></center>");
  
  page += F("<center>Closing Portal...</center><br>");
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);

  delay(AP_EXIT_DELAY);

  AP_force_exit = true;
}

/** 
 * @brief Gerencia a página 'Login Advanced' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_login_advanced(void) {
  serial_print("WebServer - AP Handle Login Advanced", 3);

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Advanced Login");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  
  page += F("<center><h1>");
  page += SYSTEM_NAME;
  page += F("</h1></center>");
  
  page += F("<center><h3>User Authentication</h3></center>");

  if (AP_advanced_login_failed)
    page += F("<center><p>Incorrect username or password.</p></center>");
  
  page += FPSTR(HTTP_ADVANCED_FORM);
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);
}

/** 
 * @brief Gerencia a página 'Auth User Advanced' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_auth_user_advanced(void) {
  serial_print("WebServer - AP Handle Authenticate User Advanced", 3);

  String username = web_server->arg("u").c_str();
  String password = web_server->arg("p").c_str();

  serial_print("WebServer - Received Credentials: [USER=" + username + "] [PASSWORD=" + password + "]", 2);

  if (username != SYSTEM_ADMIN_USER || password != SYSTEM_ADMIN_PASSWORD) {
    AP_advanced_login_failed = true;
    
    AP_handle_login_advanced();
  }
  else
    AP_advanced_login_failed = false;

  AP_handle_advanced();
}

/** 
 * @brief Gerencia a página 'Advanced' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_advanced(void) {
  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Advanced");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  
  page += F("<center><h1>");
  page += SYSTEM_NAME;
  page += F("</h1></center>");

  page += String(F("<center><h3>Advanced</h3></center>"));
  
  page += FPSTR(HTTP_ADVANCED_OPTIONS);
  
  page += FPSTR(HTTP_BACK_BTN);
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);
}

/** 
 * @brief Gerencia a página 'Confirm Reboot' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_confirm_reboot(void) {
  serial_print("WebServer - AP Handle Confirm Reboot", 3);

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Confirm Reboot");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);
  
  page += FPSTR(HTTP_REBOOT_MSG);
  
  page += FPSTR(HTTP_CONFIRM_REBOOT);
  
  page += FPSTR(HTTP_BACK_BTN);
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);
}

/** 
 * @brief Gerencia a página 'Reboot' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_reboot(void) {
  serial_print("WebServer - AP Handle Reboot", 3);

  String page = FPSTR(HTTP_HEADER);
  
  page.replace("{v}", "Reboot");
  
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += FPSTR(HTTP_HEADER_END);

  page += F("<center><h1>");
  page += SYSTEM_NAME;
  page += F("</h1></center>");
  
  page += F("<center>Rebooting...</center><br>");
  
  page += FPSTR(HTTP_END);

  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send(200, "text/html", page);

  delay(AP_REBOOT_DELAY);

  system_reboot();
}

/** 
 * @brief Gerencia a página 'Page Not Found' do Portal de Configuração do Sistema.
 */
void bettas_house_system_t::AP_handle_not_found(void) {
  serial_print("WebServer - AP Handle Page Not Found", 3);
  
  if (AP_captive_portal()) {
    return;
  }
  
  String page = "Page Not Found\n";
  
  page += "\nURI: ";
  page += web_server->uri();
  page += "\nMethod: ";
  page += ((web_server->method() == HTTP_GET) ? "GET" : "POST");
  page += "\nArguments: ";
  page += web_server->args();
  page += "\n";

  for (int i = 0; i < web_server->args(); i++) {
    page += " " + web_server->argName(i) + ": " + web_server->arg(i) + "\n";
  }
  
  web_server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  web_server->sendHeader("Pragma", "no-cache");
  web_server->sendHeader("Expires", "-1");
  web_server->sendHeader("Content-Length", String(page.length()));
  web_server->send( 404, "text/plain", page);
}

/** 
 * @brief Verifica se o 'hostHeader()' é um endereço IP válido. Caso não seja,
 * redireciona o cliente para o fim da sessão.
 * 
 * @param return Resultado da verificação.
 */
bool bettas_house_system_t::AP_captive_portal(void) {
  if (!AP_has_valid_IP(web_server->hostHeader())) {
    serial_print("WebServer - Request Redirected to Captive Portal", 3);
    
    web_server->sendHeader("Location", String("http://") + AP_convert_IP_to_string(web_server->client().localIP()), true);
    web_server->send(302, "text/plain", "");
    web_server->client().stop();
    
    return (true);
  }
  
  return (false);
}

/** 
 * @brief Verifica se a String passada como parâmetro é um IP válido.
 * 
 * @param return Resultado da verificação.
 */
bool bettas_house_system_t::AP_has_valid_IP(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    
    if (c != '.' && (c < '0' || c > '9')) {
      return (false);
    }
  }
  
  return (true);
}

/** 
 * @brief Converte uma variável do tipo 'IPAddress' para 'String'.
 * 
 * @param return String contendo o IP passado como referência.
 */
String bettas_house_system_t::AP_convert_IP_to_string(IPAddress ip) {
  String str = "";
  
  for (int i = 0; i < 3; i++)
    str += String((ip >> (8 * i)) & 0xFF) + ".";
  
  str += String(((ip >> 8 * 3)) & 0xFF);
  
  return (str);
}

/** 
 * @brief Ordena uma lista de SSIDs por intensidade de sinal (RSSI).
 * 
 * @param *list Ponteiro para a lista de SSIDs escaneados.
 * @param size  Tamanho da lista passada como referência.
 */
void bettas_house_system_t::AP_order_SSIDs_by_rssi(int *list, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (WiFi.RSSI(list[j]) > WiFi.RSSI(list[i])) {
        std::swap(list[i], list[j]);
      }
    }
  }
}

/** 
 * @brief Remove SSIDs duplicados da lista escaneada.
 * 
 * @param *list Ponteiro para a lista de SSIDs escaneados.
 * @param size  Tamanho da lista passada como referência.
 */
void bettas_house_system_t::AP_remove_duplicated_SSIDs(int *list, int size) {
  String ssid;
  
  for (int i = 0; i < size; i++) {
    if (list[i] == -1)
      continue;
  
    ssid = WiFi.SSID(list[i]);
  
    for (int j = i + 1; j < size; j++) {
      if (ssid == WiFi.SSID(list[j])) {
        list[j] = -1;
      }
    }
  }
}

/** 
 * @brief Remove APs com intensidade de sinal (RSSI) abaixo de mínimo.
 * 
 * @param *list Ponteiro para a lista de SSIDs escaneados.
 * @param size  Tamanho da lista passada como referência.
 */
String bettas_house_system_t::AP_remove_by_RSSI(int *list, int size) {
  String str = "";
  
  for (int i = 0; i < size; i++) {
    if (list[i] == -1)
      continue;
    
    int quality = AP_RSSI_to_percentage(WiFi.RSSI(list[i]));
    
    if (quality > WIFI_MINIMUM_RSSI) {
      String item = FPSTR(HTTP_ITEM);
      String rssi;
      
      rssi += quality;
      
      item.replace("{v}", WiFi.SSID(list[i]));
      item.replace("{r}", rssi);
      
      if (WiFi.encryptionType(list[i]) != ENC_TYPE_NONE) {
        item.replace("{i}", "l");
      }
      else {
        item.replace("{i}", "");
      }
      
      str += item;
      
      delay(0);
    }
  }

  return (str);
}

/** 
 * @brief Retorna a intensidade do sinal de um SSID em porcentagem (0 a 100%).
 * 
 * @param RSSI Valor da intensidade do sinal, em dBm.
 */
int bettas_house_system_t::AP_RSSI_to_percentage(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  }
  else if (RSSI >= -50) {
    quality = 100;
  }
  else {
    quality = 2 * (RSSI + 100);
  }
  
  return (quality);
}

/** 
 * @brief Retorna o tempo, em minutos, em relação ao tempo, em mili segundos, passado como referência.
 * 
 * @param ms     Tempo, em mili segundos.
 * @param return Tempo, em minutos.
 */
String bettas_house_system_t::AP_mili_seconds_to_minutes(int ms) {
  String time = "";
  
  // Minutes
  int minutes = int((ms / 1000.0) / 60.0);
  
  if (minutes < 10)
    time += F("0");
  time += String(minutes);

  // Time breaker
  time += ":";

  // Seconds
  int seconds = int((ms / 1000.0) - minutes * 60.0);

  if (seconds < 10)
    time += F("0");
  time += String(seconds);

  return (time);
}

/** 
 * @brief Inicializa os GPIOs do tipo 'Analog Outputs'.
 * 
 * @param *gpio Estrutura contendo uma estrutura do tipo 'Analog Output'.
 */
void bettas_house_system_t::initialize_analog_output(bettas_house_analog_outputs_t *gpio) {
  serial_print("Main - Initializing " + gpio->tag, 1);
  
  pinMode(gpio->ch, OUTPUT);

  set_analog_output(gpio, gpio->output);
}

/** 
 * @brief Inicializa o botão de reboot.
 * 
 * @param *gpio Estrutura contendo uma estrutura do tipo 'Digital Input'.
 */
void bettas_house_system_t::initialize_reboot_switch(bettas_house_digital_inputs_t *gpio) {
  serial_print("Main - Initializing " + gpio->tag, 1);
  
  pinMode(gpio->ch, INPUT_PULLUP);

  gpio->event_flag = false;
}

/**
 *  @brief Verifica a rotina a ser executada após interação com o SWITCH integrado.
 */
void bettas_house_system_t::reboot_switch_routine(void) {
  if (get_switch_state(button)) {
    if (button->timer > REBOOT_SW_REBOOT_INTERVAL) {
      serial_print(button->tag + " - Rebooting system...", 1);
        
      system_reboot();
    }
    else if (button->timer > REBOOT_SW_START_AP_INTERVAL) {
      serial_print(button->tag + " - Starting WebServer Portal...", 1);

      set_analog_output(led, 0);
      
      AP_start_portal(true);
    }
    else
      serial_print(button->tag + " - No action to perform", 1);
  }
  
  button->event_flag = false;
}
/** 
 * @brief Atribui um valor a saída analógica passada como referência.
 * 
 * @param *ptr  Estrutura contendo o objeto para escrita.
 * @param state Valor a ser escrito no objeto.
 */
void bettas_house_system_t::set_analog_output(bettas_house_analog_outputs_t *ptr, int value) {
  if (analogRead(ptr->ch) == value)
    return;
  
  if (ptr->pull_up_enable) {
    ptr->output = ANALOG_GPIO_MAX_OUTPUT - value;
    
    analogWrite(ptr->ch, ptr->output);
  }
  else {
    ptr->output = value;
    
    analogWrite(ptr->ch, ptr->output);
  }
  
  serial_print("GPIO - " + ptr->tag + F(" output: ") + String(ptr->output), 2);
}

/** 
 * @brief Verifica o estado da entrada digital passada como referência.
 * 
 * @param *sw Estrutura contendo o objeto SWITCH.
 */
bool bettas_house_system_t::get_switch_state(bettas_house_digital_inputs_t *sw) {
  bool sw_is_pressed = false, state = digitalRead(sw->ch);

  unsigned long start_time_tracker = 0;
  unsigned long end_time_tracker   = 0;

  if ((sw->pull_up_enable && !state) || (!sw->pull_up_enable && state)) {
    sw_is_pressed = true;
    
    start_time_tracker = millis();
    
    while (digitalRead(sw->ch) == state)
      delay(sw->debounce_interval);

    end_time_tracker = millis();
  }

  sw->timer = end_time_tracker - start_time_tracker;

  return (sw_is_pressed);
}

/** 
 * @brief Retorna o endereço MAC do sistema.
 * 
 * @param return Endereço MAC.
 */
String bettas_house_system_t::get_mac_address(void) {
  return (WiFi.macAddress());
}

/** 
 * @brief Obtém o endereço MAC do sistema.
 * 
 * @param return Endereço MAC.
 */
String bettas_house_system_t::get_mac_addr(void) {
  return (WiFi.macAddress());
}

/** 
 * @brief Apresenta informações sobre o sistema, quando com o modo DEBUG ativado.
 */
void bettas_house_system_t::print_system_info(void) {
  serial_print(F("Main - System Initialized"), 1);
  serial_print("Main - System Name: " + String(SYSTEM_NAME), 1);
  serial_print("Main - System Model: " + String(SYSTEM_MODEL), 1);
  serial_print("Main - System ID (MAC Address): " + get_mac_addr(), 1);
  serial_print("Main - System Version: " + String(SYSTEM_VERSION), 1);
  serial_print("Main - System Password: " + String(SYSTEM_PASSWORD), 1);
  serial_print("Main - System Manufacturer: " + String(SYSTEM_MANUFACTURER), 1);
}

/** 
 * @brief Realiza o restart do sistema.
 */
void bettas_house_system_t::system_restart(void) {
  ESP.reset();
}

/** 
 * @brief Realiza o reboot do sistema.
 */
void bettas_house_system_t::system_reboot(void) {
  WiFi.disconnect();
  
  ESP.reset();
}

/** 
 * @brief Apresenta mensagens do módulo 'System', quando com o modo DEBUG ativado.
 *
 * @param msg   Mensagem a ser apresentada no console.
 * @param level Nível de prioridade da mensagem enviada.
 */
void bettas_house_system_t::serial_print(String msg, uint8_t level) {
  #ifdef SYSTEM_DEBUG_MODE_ON
    if (DBG_SYSTEM_LEVEL >= level)
      Serial.println("[" + String(millis()) + "] " + msg);
  #endif /*SYSTEM_DEBUG_MODE_ON*/
}
