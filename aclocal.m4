dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl System libraries

AC_DEFUN(AC_LIB_Z, [
    AC_CHECK_LIB(z, compress)
])

AC_DEFUN(AC_FUNC_CRYPT, [
    AC_CHECK_LIB(crypt, crypt)
    AC_CHECK_FUNC(crypt, AC_DEFINE(HAVE_CRYPT))
])

dnl MySQL Libraries and Headers

AC_DEFUN(AC_LIB_MYSQL, [
    AC_ARG_WITH(mysql-lib,
    [  --with-mysql-lib=DIR    Look for MySQL client library in DIR],
    mysql_lib=$withval, mysql_lib="")

    if test "x$want_mysql" = "xyes"
    then
        AC_MSG_CHECKING([for libmysqlclient])
        AC_MSG_RESULT()

        mysql_ok=no

        SAVE_LIBS=$LIBS
    
        mysql_lib="$mysql_lib /usr/lib /usr/lib/mysql \
                   /usr/local/lib /usr/local/lib/mysql \
                   /usr/local/mysql/lib"
    
        for dir in $mysql_lib; do
            if test "x$mysql_found" != "xyes"
            then
                if [ test -f "$dir/libmysqlclient.a" ] || test -f "$dir/libmysqlclient.so" ;
                then
                    LIBS="-L$dir $SAVE_LIBS $LIBZ_LIB"
                    MYSQL_LIB="-L$dir -lmysqlclient $LIBZ_LIB"
                    AC_SUBST(MYSQL_LIB)
                    AC_CHECK_LIB(mysqlclient, mysql_real_connect,
                                 mysql_ok=yes, mysql_ok=no)
                fi
            fi
        done

        if test "x$mysql_ok" != "xyes"
        then
            AC_MSG_ERROR([Could not find libmysqlclient in '$mysql_lib'])
        fi
    fi
])


AC_DEFUN(AC_HEADER_MYSQL, [
    AC_ARG_WITH(mysql-include,
    [  --with-mysql-include=DIR
                          Look for MySQL include files in DIR],
    mysql_include=$withval, mysql_include="")

    if test "x$want_mysql" = "xyes"
    then
        AC_MSG_CHECKING([for mysql.h])
        AC_MSG_RESULT()

        mysql_found=no

        mysql_include="$mysql_include /usr/include /usr/include/mysql \
                       /usr/local/include /usr/local/include/mysql \
                       /usr/local/mysql/include"
    
        for dir in $mysql_include; do
            if test "x$mysql_found" != "xyes"
            then
                if test -f "$dir/mysql.h" 
                then
                    MYSQL_INCLUDE="-I$dir"
                    AC_SUBST(MYSQL_INCLUDE)
                    mysql_found=yes
                fi
            fi
        done

        if test "x$mysql_found" != "xyes"
        then
            AC_MSG_ERROR([Could not find mysql.h in '$mysql_include'])
        fi
    fi
])


dnl PostgreSQL Libraries and Includes

AC_DEFUN(AC_LIB_PGSQL, [
    AC_ARG_WITH(pgsql-lib,
    [  --with-pgsql-lib=DIR    Look for PostgreSQL client library in DIR],
    pgsql_lib=$withval, pgsql_lib="")

    if test "x$want_pgsql" = "xyes"
    then
        AC_MSG_CHECKING([for libpq])
        AC_MSG_RESULT()

        pgsql_ok=no

        SAVE_LIBS=$LIBS
    
        pgsql_lib="$pgsql_lib /usr/lib /usr/lib/pgsql \
                   /usr/local/lib /usr/local/lib/pgsql \
                   /usr/local/pgsql/lib"
    
        for dir in $pgsql_lib; do
            if test "x$pgsql_found" != "xyes"
            then
                if test -f "$dir/libpq.a"
                then
                    LIBS="-L$dir $SAVE_LIBS"
                    PGSQL_LIB="-L$dir -lpq"
                    AC_SUBST(PGSQL_LIB)
                    AC_CHECK_LIB(pq, PQconnectdb,
                                 pgsql_ok=yes, pgsql_ok=no)
                fi
            fi
        done

        if test "x$pgsql_ok" != "xyes"
        then
            AC_MSG_ERROR([Could not find libpq in '$pgsql_lib'])
        fi
    fi
])


AC_DEFUN(AC_HEADER_PGSQL, [
    AC_ARG_WITH(pgsql-include,
    [  --with-pgsql-include=DIR
                          Look for PostgreSQL include files in DIR],
    pgsql_include=$withval, pgsql_include="")

    if test "x$want_pgsql" = "xyes"
    then
        AC_MSG_CHECKING([for libpq-fe.h])
        AC_MSG_RESULT()

        pgsql_found=no

        pgsql_include="$pgsql_include /usr/include /usr/include/pgsql \
                       /usr/local/include /usr/local/include/pgsql \
                       /usr/local/pgsql/include"
    
        for dir in $pgsql_include; do
            if test "x$pgsql_found" != "xyes"
            then
                if test -f "$dir/libpq-fe.h"
                then
                    PGSQL_INCLUDE="-I$dir"
                    AC_SUBST(PGSQL_INCLUDE)
                    pgsql_found=yes 
                fi
            fi
        done

        if test "x$pgsql_found" != "xyes"
        then
            AC_MSG_ERROR([Could not find libpq-fe.h in '$pgsql_include'])
        fi
    fi
])


dnl Oracle Libraries and Includes

AC_DEFUN(AC_LIB_ORACLE, [
    ORACLE_LIB=""
    AC_SUBST(ORACLE_LIB)
])

AC_DEFUN(AC_HEADER_ORACLE, [
    ORACLE_INCLUDE=""
    AC_SUBST(ORACLE_INCLUDE)
])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])


dnl AM_PROG_LEX
dnl Look for flex, lex or missing, then run AC_PROG_LEX and AC_DECL_YYTEXT
AC_DEFUN(AM_PROG_LEX,
[missing_dir=ifelse([$1],,`cd $ac_aux_dir && pwd`,$1)
AC_CHECK_PROGS(LEX, flex lex, "$missing_dir/missing flex")
AC_PROG_LEX
AC_DECL_YYTEXT])


