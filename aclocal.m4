dnl aclocal.m4 generated automatically by aclocal 1.4a

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

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
dnl We require 2.13 because we rely on SHELL being computed by configure.
AC_PREREQ([2.13])
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

dnl Autoconf support for C++
dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software 
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl As a special exception to the GNU General Public License, if you 
dnl distribute this file as part of a program that contains a configuration 
dnl script generated by Autoconf, you may include it under the same 
dnl distribution terms that you use for the rest of that program.

# -------------------------------------------------------------------------
# Use this macro to configure your C compiler
# When called the macro does the following things:
# 1. It finds an appropriate C compiler.
#    If you passed the flag --with-cc=foo then it uses that
#    particular compiler
# 2. Check whether the compiler works.
# 3. Checks whether the compiler accepts the -g 
# -------------------------------------------------------------------------

AC_DEFUN(LF_CONFIGURE_CC,[
  dnl Sing the song
  AC_PROG_CC
  AC_PROG_CPP
  AC_AIX
  AC_ISC_POSIX
  AC_MINIX 
  AC_HEADER_STDC
])


dnl Autoconf support for C++
dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software 
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl As a special exception to the GNU General Public License, if you 
dnl distribute this file as part of a program that contains a configuration 
dnl script generated by Autoconf, you may include it under the same 
dnl distribution terms that you use for the rest of that program.

# -----------------------------------------------------------------
# This macro should be called to configure your C++ compiler.
# When called, the macro does the following things:
# 1. It finds an appropriate C++ compiler
#    If you passed the flag --with-cxx=foo, then it uses that
#    particular compiler
# 2. Checks whether the compiler accepts the -g 
# ------------------------------------------------------------------

AC_DEFUN(LF_CONFIGURE_CXX,[
 AC_PROG_CXX 
 AC_PROG_CXXCPP
 LF_CPP_PORTABILITY
])

# -----------------------------------------------------------------------
# This macro tests the C++ compiler for various portability problem.
# 1. Defines CXX_HAS_NO_BOOL if the compiler does not support the bool
#    data type
# 2. Defines CXX_HAS_BUGGY_FOR_LOOPS if the compiler has buggy
#    scoping for the for-loop
# 3. Defines USE_ASSERT if the user wants to use assertions
# Seperately we provide some config.h.bot code to be added to acconfig.h
# that implements work-arounds for these problems.
# -----------------------------------------------------------------------

dnl ACCONFIG TEMPLATE
dnl #undef CXX_HAS_BUGGY_FOR_LOOPS
dnl #undef CXX_HAS_NO_BOOL
dnl #undef NDEBUG
dnl END ACCONFIG

AC_DEFUN(LF_CPP_PORTABILITY,[

  dnl
  dnl Check for common C++ portability problems
  dnl

  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  dnl Check whether we have bool
  AC_MSG_CHECKING(whether C++ has bool)
  AC_TRY_RUN([main() { bool b1=true; bool b2=false; }],
             [ AC_MSG_RESULT(yes) ],
             [ AC_MSG_RESULT(no)
               AC_DEFINE(CXX_HAS_NO_BOOL) ],
             [ AC_MSG_WARN(Don't cross-compile)]
            )

  dnl Test whether C++ has buggy for-loops
  AC_MSG_CHECKING(whether C++ has buggy scoping in for-loops)
  AC_TRY_COMPILE([#include <iostream.h>], [
   for (int i=0;i<10;i++) { }
   for (int i=0;i<10;i++) { }
], [ AC_MSG_RESULT(no) ],
   [ AC_MSG_RESULT(yes)
     AC_DEFINE(CXX_HAS_BUGGY_FOR_LOOPS) ])

  dnl Test whether the user wants to enable assertions
  AC_MSG_CHECKING(whether user wants assertions)
  AC_ARG_ENABLE(assert,
                [  --disable-assert        don't use cpp.h assert],
                [ AC_DEFINE(NDEBUG)
                  AC_MSG_RESULT(no)  ],
                [ AC_MSG_RESULT(yes) ],
               )

  dnl Done with the portability checks
  AC_LANG_RESTORE
])

dnl ACCONFIG BOTTOM
dnl 
dnl // This file defines portability work-arounds for various proprietory
dnl // C++ compilers
dnl 
dnl // Workaround for compilers with buggy for-loop scoping
dnl // That's quite a few compilers actually including recent versions of
dnl // Dec Alpha cxx, HP-UX CC and SGI CC.
dnl // The trivial "if" statement provides the correct scoping to the 
dnl // for loop
dnl 
dnl #ifdef CXX_HAS_BUGGY_FOR_LOOPS
dnl #undef for
dnl #define for if(1) for
dnl #endif
dnl 
dnl //
dnl // Fortran-like integer looping macros
dnl // these critters depend on the scoping work-around above
dnl //
dnl 
dnl #define loop(COUNTER,BEGIN,END)  \
dnl for (int COUNTER = BEGIN ; COUNTER <= END ; COUNTER ## ++)
dnl 
dnl #define inverse_loop(COUNTER,END,BEGIN) \
dnl for (int COUNTER = END; COUNTER >= BEGIN; COUNTER ## --)
dnl 
dnl #define integer_loop(COUNTER,BEGIN,END,STEP) \
dnl for (int COUNTER = BEGIN; COUNTER <= END; COUNTER += STEP)
dnl 
dnl //
dnl // Class protection levels
dnl // addictive syntactic sugar to make coding nicer
dnl //
dnl 
dnl #define pub public:
dnl #define pro protected:
dnl #define pri private:
dnl 
dnl //
dnl // Every mathematician would like to know pi
dnl // so this is as good a place as any to throw it in.
dnl //
dnl 
dnl #define pi 3.14159265358979324
dnl 
dnl //
dnl // If the C++ compiler we use doesn't have bool, then
dnl // the following is a near-perfect work-around. 
dnl // You must make sure your code does not depend on "int" and "bool"
dnl // being two different types, in overloading for instance.
dnl //
dnl 
dnl #ifdef CXX_HAS_NO_BOOL
dnl #define bool int
dnl #define true 1
dnl #define false 0
dnl #endif
dnl    
dnl #include <assert.h>
dnl 
dnl END ACCONFIG


dnl Autoconf support for C++
dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software 
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl As a special exception to the GNU General Public License, if you 
dnl distribute this file as part of a program that contains a configuration 
dnl script generated by Autoconf, you may include it under the same 
dnl distribution terms that you use for the rest of that program.

# -----------------------------------------------------------------------
# This macro determines hardware-vendor-os information and 
# sets the variable ``canonical_host_type'' to that information
# ------------------------------------------------------------------------

dnl ACCONFIG TEMPLATE
dnl #undef YOUR_OS
dnl END ACCONFIG

AC_DEFUN(LF_HOST_TYPE, [
  AC_CANONICAL_HOST
  if test -z "$host"
  then
    host=unknown
  fi
  canonical_host_type=$host
  if test "$host" = unknown
  then
    AC_MSG_WARN(configuring for unknown system type)
  fi
  AC_SUBST(canonical_host_type)
  AC_DEFINE_UNQUOTED(YOUR_OS,"$canonical_host_type")
])

dnl Copyright (C) 1988 Eleftherios Gkioulekas <lf@amath.washington.edu>
dnl  
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl 
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software 
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl As a special exception to the GNU General Public License, if you 
dnl distribute this file as part of a program that contains a configuration 
dnl script generated by Autoconf, you may include it under the same 
dnl distribution terms that you use for the rest of that program.

# --------------------------------------------------------------------------
# Check whether the C++ compiler accepts a certain flag
# If it does it adds the flag to CXXFLAGS
# If it does not then it returns an error to lf_ok
# Usage:
#   LF_CHECK_CXX_FLAG(-flag1 -flag2 -flag3 ...)
# -------------------------------------------------------------------------

AC_DEFUN(LF_CHECK_CXX_FLAG,[
  echo 'void f(){}' > conftest.cc
  for i in $1
  do
    AC_MSG_CHECKING([whether $CXX accepts $i])
    if test -z "`${CXX} $i -c conftest.cc 2>&1`"
    then
      CXXFLAGS="${CXXFLAGS} $i"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.cc conftest.o
])

# --------------------------------------------------------------------------
# Check whether the C compiler accepts a certain flag
# If it does it adds the flag to CFLAGS
# If it does not then it returns an error to lf_ok
# Usage:
#  LF_CHECK_CC_FLAG(-flag1 -flag2 -flag3 ...)
# -------------------------------------------------------------------------

AC_DEFUN(LF_CHECK_CC_FLAG,[
  echo 'void f(){}' > conftest.c
  for i in $1
  do
    AC_MSG_CHECKING([whether $CC accepts $i])
    if test -z "`${CC} $i -c conftest.c 2>&1`"
    then
      CFLAGS="${CFLAGS} $i"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.c conftest.o
])

# --------------------------------------------------------------------------
# Check whether the Fortran compiler accepts a certain flag
# If it does it adds the flag to FFLAGS
# If it does not then it returns an error to lf_ok
# Usage:
#  LF_CHECK_F77_FLAG(-flag1 -flag2 -flag3 ...)
# -------------------------------------------------------------------------

AC_DEFUN(LF_CHECK_F77_FLAG,[
  cat << EOF > conftest.f
c....:++++++++++++++++++++++++
      PROGRAM MAIN
      PRINT*,'Hello World!'
      END
EOF
  for i in $1
  do
    AC_MSG_CHECKING([whether $F77 accepts $i])
    if test -z "`${F77} $i -c conftest.f 2>&1`"
    then
      FFLAGS="${FFLAGS} $i"
      AC_MSG_RESULT(yes)  
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.f conftest.o
])

# ----------------------------------------------------------------------
# Provide the configure script with an --with-warnings option that
# turns on warnings. Call this command AFTER you have configured ALL your
# compilers. 
# ----------------------------------------------------------------------

AC_DEFUN(LF_SET_WARNINGS,[
  dnl Check for --with-warnings
  AC_MSG_CHECKING([whether user wants warnings])
  AC_ARG_WITH(warnings,
              [  --with-warnings         Turn on warnings],
              [ lf_warnings=yes ], [ lf_warnings=no ])
  AC_MSG_RESULT($lf_warnings)
  
  dnl Warnings for the two main compilers
  cc_warning_flags="-Wall"
  cxx_warning_flags="-Wall -Woverloaded-virtual -Wtemplate-debugging"
  if test $lf_warnings = yes
  then
    if test -n "${CC}"
    then
      LF_CHECK_CC_FLAG($cc_warning_flags)
    fi
    if test -n "${CXX}" 
    then
      LF_CHECK_CXX_FLAG($cxx_warning_flags)
    fi
  fi
])

