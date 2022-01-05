/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        webserver.h                                  Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2021/04/07 10:25:33 by rFeijo                                              */
/*        Updated: 2022/01/05 20:10:59 by rFeijo                         All rights reserved  */
/**********************************************************************************************/

// Diretivas de configuração do 'Header'
const char HTTP_HEADER[]            PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";
const char HTTP_STYLE[]             PROGMEM = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url(\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==\") no-repeat left center;background-size: 1em;}</style>";
const char HTTP_SCRIPT[]            PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const char HTTP_HEADER_END[]        PROGMEM = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

// Diretivas de configuração da página 'Root'
const char HTTP_PORTAL_CONFIG_WIFI[]PROGMEM = "<form action=\"/setwifi\"method=\"get\"><button>Configure WiFi</button></form><br/>";
const char HTTP_PORTAL_ABOUT[]      PROGMEM = "<form action=\"/about\"method=\"get\"><button>About</button></form><br/>";
const char HTTP_PORTAL_RESTART[]    PROGMEM = "<form action=\"/restart\"method=\"post\"><button>Restart</button></form><br>";
const char HTTP_PORTAL_EXIT[]       PROGMEM = "<form action=\"/exit\"method=\"post\"><button>Exit</button></form><br>";
const char HTTP_PORTAL_ADVANCED[]   PROGMEM = "<center><a href=\"/auth_advanced\">Advanced</a><br/></center>";
const char HTTP_ITEM[]              PROGMEM = "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";

// Diretivas de configuração da página 'Configure WiFi'
const char HTTP_FORM_START[]        PROGMEM = "<form method='get' action='save_credentials'><input id='s' name='s' length=32 placeholder='SSID'><br/><input id='p' name='p' length=64 type='password' placeholder='Password'><br/>";
const char HTTP_FORM_PARAM[]        PROGMEM = "<br/><input id='{i}' name='{n}' maxlength={l} placeholder='{p}' value='{v}' {c}>";
const char HTTP_FORM_END[]          PROGMEM = "<br/><button type='submit'>save</button></form>";
const char HTTP_SCAN_LINK[]         PROGMEM = "<div class=\"c\"><a href=\"/setwifi\">SCAN</a></div><br/>";
const char HTTP_SAVED[]             PROGMEM = "<center><div>Trying to connect to WiFi...<br/>If it fails, reconnect to AP and try again.</div></center>";

// Diretivas de configuração da página 'Advanced'
const char HTTP_ADVANCED_FORM[]     PROGMEM = "<form method='get' action='auth_user_advanced'><input id='u' name='u' length=32 placeholder='Username'><br/><input id='p' name='p' length=64 type='password' placeholder='Password'><br/><br/><button type='submit'>Login</button></form>";
const char HTTP_ADVANCED_OPTIONS[]  PROGMEM = "<form action=\"/confirm_reboot\"method=\"get\"><button>Reboot</button></form>";

// Diretivas de configuração da página 'Reboot'
const char HTTP_REBOOT_MSG[]        PROGMEM = "<center>Resetting the system to its default configuration restarts the system as new, and all of its configuration is lost.</center><br>";
const char HTTP_CONFIRM_REBOOT[]    PROGMEM = "<br><form method='get' action='/reboot'><button type='submit'>Reboot</button></form>";

// Diretiva de configuração do webserver HTTP
const char HTTP_BACK_BTN[]          PROGMEM = "<br><center><a href=\"/fwlink\">back</a><br/></center>";
const char HTTP_END[]               PROGMEM = "</div></body></html>";
