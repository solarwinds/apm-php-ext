/* apm_ext extension for PHP */

#ifndef PHP_APM_EXT_H
#define PHP_APM_EXT_H

extern zend_module_entry apm_ext_module_entry;
#define phpext_apm_ext_ptr &apm_ext_module_entry

ZEND_BEGIN_MODULE_GLOBALS(apm_ext)
char *collector;
char *service_key;
void *setting_service;
ZEND_END_MODULE_GLOBALS(apm_ext)

ZEND_EXTERN_MODULE_GLOBALS(apm_ext)

#define APM_EXT_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(apm_ext, v)

#define PHP_APM_EXT_VERSION "1.0.0"

#if defined(ZTS) && defined(COMPILE_DL_APM_EXT)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* PHP_APM_EXT_H */
