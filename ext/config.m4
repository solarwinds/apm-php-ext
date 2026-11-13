dnl config.m4 for extension apm_ext

PHP_ARG_ENABLE([apm_ext],
  [whether to enable apm_ext support],
  [AS_HELP_STRING([--enable-apm_ext],
    [Enable apm_ext support])],
  [no])

if test "$PHP_APM_EXT" != "no"; then
  AC_DEFINE(HAVE_APM_EXT, 1, [ Have apm_ext support ])
  PHP_NEW_EXTENSION(apm_ext, [apm_ext.c cache.c], $ext_shared,, "-Wall -Wextra -Werror -Wno-unused-parameter")
fi
