dnl config.m4 for extension pcm_client

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([pcm_client],
dnl   [for pcm_client support],
dnl   [AS_HELP_STRING([--with-pcm_client],
dnl     [Include pcm_client support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([pcm_client],
  [whether to enable pcm_client support],
  [AS_HELP_STRING([--enable-pcm_client],
    [Enable pcm_client support])],
  [no])

if test "$PHP_PCM_CLIENT" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, PCM_CLIENT_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-pcm_client -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/pcm_client.h"  # you most likely want to change this
  dnl if test -r $PHP_PCM_CLIENT/$SEARCH_FOR; then # path given as parameter
  dnl   PCM_CLIENT_DIR=$PHP_PCM_CLIENT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for pcm_client files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PCM_CLIENT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PCM_CLIENT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the pcm_client distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-pcm_client -> add include path
  dnl PHP_ADD_INCLUDE($PCM_CLIENT_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-pcm_client -> check for lib and symbol presence
  dnl LIBNAME=PCM_CLIENT # you may want to change this
  dnl LIBSYMBOL=PCM_CLIENT # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_PCM_CLIENT_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your pcm_client library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PCM_CLIENT_DIR/$PHP_LIBDIR, PCM_CLIENT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PCM_CLIENT_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your pcm_client library.])
  dnl ],[
  dnl   -L$PCM_CLIENT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PCM_CLIENT_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_PCM_CLIENT, 1, [ Have pcm_client support ])

  PHP_NEW_EXTENSION(pcm_client, pcm_client.c, $ext_shared)
fi
