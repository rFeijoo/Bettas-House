/**********************************************************************************************/
/*                                                                                            */
/*                                                                                            */
/*        config.h                                     Author  : Rafael Feijó Leonardo        */
/*                                                     Email   : goldcard99@hotmail.com       */
/*                                                     Address : DF, BRAZIL, 70670-403        */
/*        Created: 2020/08/02 22:28:24 by rFeijo                                              */
/*        Updated: 2022/01/04 16:11:13 by rFeijo                                              */
/*                                                                       All rights reserved  */
/**********************************************************************************************/

#ifndef _CONFIG_H_
  #define _CONFIG_H_

/**
 * Diretivas de Identificação do Sistema
 */
#define SYSTEM_NAME           "Betta's House"
#define SYSTEM_MODEL          "Null"
#define SYSTEM_MANUFACTURER   "@rFeijoo"
#define SYSTEM_VERSION        "1.0.0"
#define SYSTEM_PASSWORD       "216-00-611"
#define SYSTEM_ADMIN_USER     "Admin"
#define SYSTEM_ADMIN_PASSWORD "password"

/**
 * Diretivas de configuração do modo DEBUG
 * Level:   0 - Desativado
 *          1 - Mais Relevantes
 *          2 - Relevantes
 *          3 - Todas
 */
#define SYSTEM_DEBUG_MODE_ON
#ifdef SYSTEM_DEBUG_MODE_ON
  #define DBG_SYSTEM_LEVEL  3
#endif /*SYSTEM_DEBUG_MODE_ON*/

/**
 * Diretiva de configuração de flags do sistema
 */
#define ERROR_FLAG 404.0

/**
 * Diretivas de configaração das saídas PWM.
 */
#define ANALOG_GPIO_MIN_OUTPUT  0.00
#define ANALOG_GPIO_MAX_OUTPUT  1023.00

/**
 * Diretivas de configuração do Timer 0
 */
#define TIMER_INTERVAL      2000
#define TIMER_AUTO_RESET_ON 1

#endif /*_CONFIG_H_*/
