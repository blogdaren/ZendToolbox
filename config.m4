dnl $Id$
dnl config.m4 for extension toolbox

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(toolbox, for toolbox support,
dnl Make sure that the comment is aligned:
dnl [  --with-toolbox             Include toolbox support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(toolbox, whether to enable toolbox support,
[  --enable-toolbox           Enable toolbox support])

if test "$PHP_TOOLBOX" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-toolbox -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/toolbox.h"  # you most likely want to change this
  dnl if test -r $PHP_TOOLBOX/$SEARCH_FOR; then # path given as parameter
  dnl   TOOLBOX_DIR=$PHP_TOOLBOX
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for toolbox files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TOOLBOX_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TOOLBOX_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the toolbox distribution])
  dnl fi

  dnl # --with-toolbox -> add include path
  dnl PHP_ADD_INCLUDE($TOOLBOX_DIR/include)

  dnl # --with-toolbox -> check for lib and symbol presence
  dnl LIBNAME=toolbox # you may want to change this
  dnl LIBSYMBOL=toolbox # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TOOLBOX_DIR/$PHP_LIBDIR, TOOLBOX_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_TOOLBOXLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong toolbox lib version or lib not found])
  dnl ],[
  dnl   -L$TOOLBOX_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(TOOLBOX_SHARED_LIBADD)

  PHP_NEW_EXTENSION(toolbox, toolbox.c, $ext_shared)
fi
