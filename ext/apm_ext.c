/* apm_ext extension for PHP */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_apm_ext.h"
#include "settings_service_c_wrapper.h"
#include "apm_ext_arginfo.h"
#ifndef _WIN32
#include <pthread.h>
#endif
#include <time.h>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE()                                           \
  ZEND_PARSE_PARAMETERS_START(0, 0)                                            \
  ZEND_PARSE_PARAMETERS_END()
#endif

ZEND_DECLARE_MODULE_GLOBALS(apm_ext)

PHP_INI_BEGIN()
PHP_INI_END()

/* {{{ void Solarwinds\\Sampler\\settings() */
PHP_FUNCTION(Solarwinds_Sampler_settings) {
  char *collector;
  size_t collctor_len;
  char *service_key;
  size_t service_key_len;
  ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_STRING(collector, collctor_len)
    Z_PARAM_STRING(service_key, service_key_len)
  ZEND_PARSE_PARAMETERS_END();

  char settings[SETTINGS_BUFFER_SIZE] = {0};

  if (!collctor_len || !service_key_len) {
    RETURN_STRING(settings);
  }

  if (APM_EXT_G(settings_service) == NULL) {
    APM_EXT_G(settings_service) = Settings_Service_Allocate(collector, service_key);
  }

  Settings_Service_Get_Settings(APM_EXT_G(settings_service), settings);
  RETURN_STRING(settings);
}
/* }}} */

#ifndef _WIN32
void prefork() {
  Settings_Service_Free(APM_EXT_G(settings_service));
  APM_EXT_G(settings_service) = NULL;
}

void postfork() {
}
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(apm_ext) {
#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  REGISTER_INI_ENTRIES();

#ifndef _WIN32
  pthread_atfork(prefork, postfork, postfork);
#endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(apm_ext) {
  Settings_Service_Free(APM_EXT_G(settings_service));
  APM_EXT_G(settings_service) = NULL;

  UNREGISTER_INI_ENTRIES();

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(apm_ext) {
#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(apm_ext) {
  php_info_print_table_start();
  php_info_print_table_header(2, "apm_ext support", "enabled");
  php_info_print_table_end();
  DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
PHP_GINIT_FUNCTION(apm_ext)
{
  ZEND_SECURE_ZERO(apm_ext_globals, sizeof(*apm_ext_globals));
}
/* }}} */

/* {{{ apm_ext_module_entry */
zend_module_entry apm_ext_module_entry = {
    STANDARD_MODULE_HEADER,
    "apm_ext",              /* Extension name */
    ext_functions,          /* zend_function_entry */
    PHP_MINIT(apm_ext),     /* PHP_MINIT - Module initialization */
    PHP_MSHUTDOWN(apm_ext), /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(apm_ext),     /* PHP_RINIT - Request initialization */
    NULL,                   /* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(apm_ext),     /* PHP_MINFO - Module info */
    PHP_APM_EXT_VERSION,    /* Version */
    PHP_MODULE_GLOBALS(apm_ext),
    PHP_GINIT(apm_ext),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX};
/* }}} */

#ifdef COMPILE_DL_APM_EXT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(apm_ext)
#endif
