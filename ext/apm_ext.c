/* apm_ext extension for PHP */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_apm_ext.h"
#include "setting_service_c_wrapper.h"
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
STD_PHP_INI_ENTRY("apm_ext.collector", "apm.collector.na-01.cloud.solarwinds.com",
                  PHP_INI_ALL, OnUpdateString, collector, zend_apm_ext_globals,
                  apm_ext_globals)
STD_PHP_INI_ENTRY("apm_ext.service_key", "", PHP_INI_ALL, OnUpdateString,
                  service_key, zend_apm_ext_globals, apm_ext_globals)
PHP_INI_END()

/* {{{ void Solarwinds\\Sampler\\setting() */
PHP_FUNCTION(Solarwinds_Sampler_setting) {
  ZEND_PARSE_PARAMETERS_NONE();
  char setting[SETTING_BUFFER_SIZE] = {0};
  Setting_Service_Get_Setting(APM_EXT_G(setting_service), setting);
  RETURN_STRING(setting);
}
/* }}} */

#ifndef _WIN32
void prefork() {
  Setting_Service_Free(APM_EXT_G(setting_service));
}

void postfork() {
  APM_EXT_G(setting_service) = Setting_Service_Allocate(APM_EXT_G(collector), APM_EXT_G(service_key));
}
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(apm_ext) {
#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  REGISTER_INI_ENTRIES();

  APM_EXT_G(setting_service) = Setting_Service_Allocate(APM_EXT_G(collector), APM_EXT_G(service_key));

#ifndef _WIN32
  pthread_atfork(prefork, postfork, postfork);
#endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(apm_ext) {
  Setting_Service_Free(APM_EXT_G(setting_service));

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
PHP_GINIT_FUNCTION(apm_ext) { ZEND_SECURE_ZERO(apm_ext_globals, sizeof(*apm_ext_globals)); }
/* }}} */

/* {{{ apm_ext_module_entry */
zend_module_entry apm_ext_module_entry = {
    STANDARD_MODULE_HEADER,
    "apm_ext",              /* Extension name */
    ext_functions,          /* zend_function_entry */
    PHP_MINIT(apm_ext),     /* PHP_MINIT - Module initialization */
    PHP_MSHUTDOWN(apm_ext), /* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(apm_ext),     /* PHP_RINIT - Request initialization */
    NULL,               /* PHP_RSHUTDOWN - Request shutdown */
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
