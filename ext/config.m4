dnl config.m4 for extension apm_ext

PHP_ARG_ENABLE([apm_ext],
  [whether to enable apm_ext support],
  [AS_HELP_STRING([--enable-apm_ext],
    [Enable apm_ext support])],
  [no])

if test "$PHP_APM_EXT" != "no"; then
  dnl Enable C++
  PHP_REQUIRE_CXX()

  AC_DEFINE(HAVE_APM_EXT, 1, [ Have apm_ext support ])
  PHP_NEW_EXTENSION(apm_ext, [apm_ext.c cache_c_wrapper.cpp cache.cpp], $ext_shared,, "-Wall -Wextra -Werror -Wno-unused-parameter -std=c++17",cxx)
fi
