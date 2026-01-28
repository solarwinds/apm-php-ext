/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 109c874d71ea223fc4aa34c5a131dd8ac8e46c8b */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_Solarwinds_Cache_get, 0, 3,
                                        MAY_BE_STRING | MAY_BE_FALSE)
ZEND_ARG_TYPE_INFO(0, collector, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, token, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, serviceName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_Solarwinds_Cache_put, 0, 4,
                                        _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, collector, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, token, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, serviceName, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, settings, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(Solarwinds_Cache_get);
ZEND_FUNCTION(Solarwinds_Cache_put);

static const zend_function_entry ext_functions[] = {
    ZEND_NS_FALIAS("Solarwinds\\Cache", get, Solarwinds_Cache_get,
                   arginfo_Solarwinds_Cache_get)
        ZEND_NS_FALIAS("Solarwinds\\Cache", put, Solarwinds_Cache_put,
                       arginfo_Solarwinds_Cache_put) ZEND_FE_END};
