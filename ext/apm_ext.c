/* apm_ext extension for PHP */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_apm_ext.h"
#include "cache_c_wrapper.h"
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
STD_PHP_INI_ENTRY("apm_ext.cache_max_size", "10000", PHP_INI_ALL,
                  OnUpdateLongGEZero, cache_max_size, zend_apm_ext_globals,
                  apm_ext_globals)
STD_PHP_INI_ENTRY("apm_ext.settings_max_length", "4096", PHP_INI_ALL,
                  OnUpdateLongGEZero, settings_max_length, zend_apm_ext_globals,
                  apm_ext_globals)
PHP_INI_END()

/* {{{ void Solarwinds\\Cache\\get() */
PHP_FUNCTION(Solarwinds_Cache_get) {
  char *collector;
  size_t collector_len;
  char *token;
  size_t token_len;
  char *service_name;
  size_t service_name_len;

  ZEND_PARSE_PARAMETERS_START(3, 3)
  Z_PARAM_STRING(collector, collector_len)
  Z_PARAM_STRING(token, token_len)
  Z_PARAM_STRING(service_name, service_name_len)
  ZEND_PARSE_PARAMETERS_END();

  if (!collector_len || !token_len || !service_name_len) {
    RETURN_FALSE;
  }

  zend_string *settings =
      Cache_Get(APM_EXT_G(cache), collector, token, service_name);
  if (settings != NULL) {
    RETURN_NEW_STR(settings);
  }
  RETURN_FALSE;
}
/* }}} */

/* {{{ void Solarwinds\\Cache\\put() */
PHP_FUNCTION(Solarwinds_Cache_put) {
  char *collector;
  size_t collector_len;
  char *token;
  size_t token_len;
  char *service_name;
  size_t service_name_len;
  char *settings;
  size_t settings_len;
  ZEND_PARSE_PARAMETERS_START(4, 4)
  Z_PARAM_STRING(collector, collector_len)
  Z_PARAM_STRING(token, token_len)
  Z_PARAM_STRING(service_name, service_name_len)
  Z_PARAM_STRING(settings, settings_len)
  ZEND_PARSE_PARAMETERS_END();

  if (collector_len && token_len && service_name_len && settings_len) {
    if (settings_len <= (size_t)APM_EXT_G(settings_max_length)) {
      Cache_Put(APM_EXT_G(cache), collector, token, service_name, settings);
      RETURN_TRUE;
    }
    fprintf(stderr, "apm_ext: settings length %zu exceeds max length %ld\n",
            settings_len, (long)APM_EXT_G(settings_max_length));
  }
  RETURN_FALSE;
}
/* }}} */

#ifndef _WIN32
void prefork() {
  Cache_Free(APM_EXT_G(cache));
  APM_EXT_G(cache) = NULL;
}

void postfork() {
  APM_EXT_G(cache) = Cache_Allocate(APM_EXT_G(cache_max_size));
}
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(apm_ext) {
#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif
  REGISTER_INI_ENTRIES();

  APM_EXT_G(cache) = Cache_Allocate(APM_EXT_G(cache_max_size));

#ifndef _WIN32
  pthread_atfork(prefork, postfork, postfork);
#endif

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(apm_ext) {
  Cache_Free(APM_EXT_G(cache));
  APM_EXT_G(cache) = NULL;

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
PHP_GINIT_FUNCTION(apm_ext) {
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
