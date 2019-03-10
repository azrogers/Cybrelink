## This file is part of the GLTT library
##    Copyright (C) 1998-1999 Stephane Rehel

## Portions grabbed from the KDE project:
##    Copyright (C) 1997 Janos Farkas (chexum@shadow.banki.hu)
##              (C) 1997 Stephan Kulow (coolo@kde.org)

##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation; either version 2 of the License, or
##   (at your option) any later version.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public License
##   along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##


# CHECK FOR COMPILERS

AC_DEFUN(AC_CHECK_COMPILERS,
[
  AC_ARG_ENABLE(debug,[  --enable-debug          creates debugging code [default=no]],
  [
   if test $enableval = "no"; dnl
     then ac_use_debug_code="no"
     else ac_use_debug_code="yes"
   fi
  ], [ac_use_debug_code="no"])

dnl this was AC_PROG_CC. I had to include it manualy, since I had to patch it
  AC_MSG_CHECKING(for a C-Compiler)
  dnl if there is one, print out. if not, don't matter
  AC_MSG_RESULT($CC)

  if test -z "$CC"; then AC_CHECK_PROG(CC, gcc, gcc) fi
  if test -z "$CC"; then AC_CHECK_PROG(CC, cc, cc, , , /usr/ucb/cc) fi
  if test -z "$CC"; then AC_CHECK_PROG(CC, xlc, xlc) fi
  test -z "$CC" && AC_MSG_ERROR([no acceptable cc found in \$PATH])
  AC_PROVIDE([AC_PROG_CC])

  AC_PROG_CC_WORKS
  AC_PROG_CC_GNU

  if test $ac_cv_prog_gcc = yes; then
    have_GCC=yes
  else
    have_GCC=
  fi

  if test -z "$CFLAGS"; then
    if test "$ac_use_debug_code" = "yes"; then
      AC_PROG_CC_G
      if test $ac_cv_prog_cc_g = yes; then
        CFLAGS="-g"
      fi
    else
      if test "$have_GCC" = "yes"; then
        CFLAGS="-O2 -g"
      else
        CFLAGS=""
      fi
    fi

    if test "$have_GCC" = "yes"; then
     CFLAGS="$CFLAGS -Wall"
    fi

  fi

  if test -z "$LDFLAGS" && test "$ac_use_debug_code" = "no" && test "$have_GCC" = "yes"; then
     LDFLAGS=""
  fi


dnl this is AC_PROG_CPP. I had to include it here, since autoconf checks
dnl dependecies between AC_PROG_CPP and AC_PROG_CC (or is it automake?)

  AC_MSG_CHECKING(how to run the C preprocessor)
  # On Suns, sometimes $CPP names a directory.
  if test -n "$CPP" && test -d "$CPP"; then
    CPP=
  fi
  if test -z "$CPP"; then
  AC_CACHE_VAL(ac_cv_prog_CPP,
  [  # This must be in double quotes, not single quotes, because CPP may get
    # substituted into the Makefile and "${CC-cc}" will confuse make.
    CPP="${CC-cc} -E"
    # On the NeXT, cc -E runs the code through the compiler's parser,
    # not just through cpp.
    dnl Use a header file that comes with gcc, so configuring glibc
    dnl with a fresh cross-compiler works.
    AC_TRY_CPP([#include <assert.h>
    Syntax Error], ,
    CPP="${CC-cc} -E -traditional-cpp"
    AC_TRY_CPP([#include <assert.h>
    Syntax Error], , CPP=/lib/cpp))
    ac_cv_prog_CPP="$CPP"])dnl
    CPP="$ac_cv_prog_CPP"
  else
    ac_cv_prog_CPP="$CPP"
  fi
  AC_MSG_RESULT($CPP)
  AC_PROVIDE([AC_PROG_CPP])
  AC_SUBST(CPP)dnl


  AC_MSG_CHECKING(for a C++-Compiler)
  dnl if there is one, print out. if not, don't matter
  AC_MSG_RESULT($CXX)

  if test -z "$CXX"; then AC_CHECK_PROG(CXX, g++, g++) fi
  if test -z "$CXX"; then AC_CHECK_PROG(CXX, CC, CC) fi
  if test -z "$CXX"; then AC_CHECK_PROG(CXX, xlC, xlC) fi
  if test -z "$CXX"; then AC_CHECK_PROG(CXX, DCC, DCC) fi
  test -z "$CXX" && AC_MSG_ERROR([no acceptable C++-compiler found in \$PATH])

  AC_PROG_CXX_WORKS
  AC_PROG_CXX_GNU

  if test $ac_cv_prog_gxx = yes; then
    have_GXX=yes
  else
    AC_MSG_CHECKING(whether we are using SPARC CC)
    have_GXX=
    cat > conftest.C << EOF
#ifdef __SUNPRO_CC
   yes;
#endif
EOF

    ac_try="$CXX -E conftest.C"
    if { (eval echo configure:__online__: \"$ac_try\") 1>&5; (eval $ac_try) 2>&5; } | egrep yes >/dev/null 2>&1; then
      ok_ac_cv_prog_CC=yes
    else
      ok_ac_cv_prog_CC=no
    fi
    AC_MSG_RESULT($ok_ac_cv_prog_CC)
  fi

  if test -z "$CXXFLAGS"; then
    if test "$ac_use_debug_code" = "yes"; then
      AC_PROG_CXX_G
      if test $ac_cv_prog_cxx_g = yes; then
        CXXFLAGS="-g"
      fi
      if test "$ok_ac_cv_prog_CC" = "yes"; then
        CXXFLAGS="$CXXFLAGS -pto"
      fi
    else
      if test "$have_GXX" = "yes"; then
         CXXFLAGS="-O2 -g"
      else
         if test "$ok_ac_cv_prog_CC" = "yes"; then
            CXXFLAGS="-pto -O2"
         else
            CXXFLAGS=""
         fi
      fi
    fi

    if test "$have_GXX" = "yes"; then
       CXXFLAGS="$CXXFLAGS -Wall"
    fi
  fi

])


## ------------------------------------------------------------------------
## Find a file (or one of more files in a list of dirs)
## ------------------------------------------------------------------------
##
AC_DEFUN(AC_FIND_FILE,
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])

## ------------------------------------------------------------------------
## Find the header files and libraries for X-Windows. Extended the
## macro AC_PATH_X
## ------------------------------------------------------------------------
##
AC_DEFUN(GLTT_PATH_X,
[
AC_MSG_CHECKING(for X)
AC_CACHE_VAL(ac_cv_have_x,
[# One or both of the vars are not set, and there is no cached value.
ac_x_includes=NO ac_x_libraries=NO
AC_PATH_X_DIRECT
AC_PATH_X_XMKMF
if test "$ac_x_includes" = NO || test "$ac_x_libraries" = NO; then
  AC_MSG_ERROR([Can't find X includes. Please check your installation and add the correct paths!])
else
  # Record where we found X for the cache.
  ac_cv_have_x="have_x=yes \
                ac_x_includes=$ac_x_includes ac_x_libraries=$ac_x_libraries"
fi])dnl
eval "$ac_cv_have_x"

if test "$have_x" != yes; then
  AC_MSG_RESULT($have_x)
  no_x=yes
else
  # If each of the values was on the command line, it overrides each guess.
  test "x$x_includes" = xNONE && x_includes=$ac_x_includes
  test "x$x_libraries" = xNONE && x_libraries=$ac_x_libraries

  # don't set -I/usr/X11R6/include please. It's boring. /S.Rehel
  # IRIX test doesn't support -ef ! Aaah GNU ... Aah linux!
  #
  if test ! -z "$x_includes" ; then
    xdir1=`cd $x_includes/X11 && /bin/pwd`
    xdir2=`cd /usr/include/X11 && /bin/pwd`
    if test "$xdir1" = "$xdir2" ; then
      x_includes=/usr/include
    fi
  fi

  # Update the cache value to reflect the command line values.
  ac_cv_have_x="have_x=yes \
                ac_x_includes=$x_includes ac_x_libraries=$x_libraries"
  AC_MSG_RESULT([libraries $x_libraries, headers $x_includes])

#  CPPFLAGS="$CPPFLAGS -L$x_libraries"
fi

if test -z "$x_includes" || \
   test "$x_includes" = "/usr/include" || \
   test "x$x_includes" = xNONE; then
  X_INCLUDES=""
  x_includes="."; dnl better than nothing :-
 else
  X_INCLUDES="-I$x_includes"
fi

if test -z "$x_libraries" || test "$x_libraries" = xNONE; then
  X_LDFLAGS=""
# It does bad things under IRIX 6.4 /SR Mar.12.1999
#  x_libraries="/usr/lib"; dnl better than nothing :-
  all_libraries=""
 else
  X_LDFLAGS="-L$x_libraries"
  all_libraries=$X_LDFLAGS
fi

AC_SUBST(X_INCLUDES)
AC_SUBST(X_LDFLAGS)
all_includes=$X_INCLUDES
])


## ------------------------------------------------------------------------
## Try to find the TTF headers and libraries.
## $(TTF_LDFLAGS) will be -Lttfliblocation (if needed)
## and $(TTF_INCLUDES) will be -Ittfhdrlocation (if needed)
## ------------------------------------------------------------------------
##
AC_DEFUN(AC_PATH_TTF,
[
AC_MSG_CHECKING([for FreeType])
ac_ttf_includes=NO ac_ttf_libraries=NO
ttf_libraries=""
ttf_includes=""
AC_ARG_WITH(ttf-dir,
    [  --with-ttf-dir          where the root of freetype is installed ],
    [  ac_ttf_includes="$withval"/include
       ac_ttf_libraries="$withval"/lib
    ])

AC_ARG_WITH(ttf-includes,
    [  --with-ttf-includes     where the freetype includes are. ],
    [
       ac_ttf_includes="$withval"
    ])

AC_ARG_WITH(ttf-libraries,
    [  --with-ttf-libraries    where the freetype library is installed.],
    [  ac_ttf_libraries="$withval"
    ])

if test "$ac_ttf_includes" = NO || test "$ac_ttf_libraries" = NO; then

AC_CACHE_VAL(ac_cv_have_ttf,
[#try to guess ttf locations

ttf_incdirs="$ac_ttf_includes /usr/lib/freetype/include /usr/local/freetype/include /usr/include/freetype /usr/include /usr/local/include /usr/local/ttf $TTF_INC"
test -n "$TTFDIR" && ttf_incdirs="$TTFDIR/include $TTFDIR $ttf_incdirs"
AC_FIND_FILE(freetype.h, $ttf_incdirs, ttf_incdir)
ac_ttf_includes=$ttf_incdir

ttf_libdirs="$ac_ttf_libraries /usr/lib/freetype/lib /usr/local/freetype/lib /usr/lib/freetype /usr/lib /usr/local/lib /usr/local/ttf/lib $TTFLIB"
test -n "$TTFDIR" && ttf_libdirs="$TTFDIR/lib $TTFDIR $ttf_libdirs"
AC_FIND_FILE(libttf.so libttf.so.1.0.0 libttt.so.1 libttf.a libttf.sl, $ttf_libdirs, ttf_libdir)
ac_ttf_libraries=$ttf_libdir

ac_cxxflags_safe=$CXXFLAGS
ac_ldflags_safe=$LDFLAGS
ac_libs_safe=$LIBS

CXXFLAGS="$CXXFLAGS -I$ttf_incdir"
LDFLAGS="-L$ttf_libdir"
LIBS="$LIBS -lttf"

AC_LANG_CPLUSPLUS
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "freetype.h"
int main() {
  TT_Engine engine;
  TT_Init_FreeType(&engine);
  return 0;
}
EOF

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
  ac_ttf_libraries="NO"
fi
rm -f conftest*
CXXFLAGS=$ac_cxxflags_safe
LDFLAGS=$ac_ldflags_safe
LIBS=$ac_libs_safe

if test "$ac_ttf_includes" = NO || test "$ac_ttf_libraries" = NO; then
  ac_cv_have_ttf="have_ttf=no"
  ac_ttf_notfound=""
  if test "$ac_ttf_includes" = NO; then
    if test "$ac_ttf_libraries" = NO; then
      ac_ttf_notfound="(headers and libraries)";
    else
      ac_ttf_notfound="(headers)";
    fi
  else
    ac_ttf_notfound="(libraries)";
  fi

  AC_MSG_ERROR([FreeType $ac_ttf_notfound not found. Please check your installation! ]);
else
  have_ttf="yes"
fi
])
else
  have_ttf="yes"
fi

eval "$ac_cv_have_ttf"

if test "$have_ttf" != yes; then
  AC_MSG_RESULT([$have_ttf]);
else
  ac_cv_have_ttf="have_ttf=yes \
    ac_ttf_includes=$ac_ttf_includes ac_ttf_libraries=$ac_ttf_libraries"
  AC_MSG_RESULT([libraries $ac_ttf_libraries, headers $ac_ttf_includes])

  ttf_libraries=$ac_ttf_libraries
  ttf_includes=$ac_ttf_includes
fi

AC_SUBST(ttf_libraries)
AC_SUBST(ttf_includes)

if test "$ttf_includes" = "$x_includes" || \
   test "$ttf_includes" = "/usr/include" || \
   test -z "$ttf_includes"; then
 TTF_INCLUDES="";
else
 TTF_INCLUDES="-I$ttf_includes"
 all_includes="$TTF_INCLUDES $all_includes"
fi

if test "$ttf_libraries" = "$x_libraries" || \
   test "$ttf_libraries" = "/usr/lib" || \
   test "$ttf_libraries" = "/usr/local/lib" || \
   test -z "$ttf_libraries"; then
 TTF_LDFLAGS=""
else
 TTF_LDFLAGS="-L$ttf_libraries"
 all_libraries="$TTF_LDFLAGS $all_libraries"
fi

AC_SUBST(TTF_INCLUDES)
AC_SUBST(TTF_LDFLAGS)
])

## ------------------------------------------------------------------------
## Try to find the GLUT headers and libraries.
## $(GLUT_LDFLAGS) will be -Lglutliblocation (if needed)
## and $(GLUT_INCLUDES) will be -Igluthdrlocation (if needed)
## ------------------------------------------------------------------------
##
AC_DEFUN(AC_PATH_GLUT,
[
AC_MSG_CHECKING([for glut])
ac_glut_includes=NO ac_glut_libraries=NO
glut_libraries=""
glut_includes=""
AC_ARG_WITH(glut-dir,
    [  --with-glut-dir         where the root of glut is installed ],
    [  ac_glut_includes="$withval"/include
       ac_glut_libraries="$withval"/lib/glut
    ])

AC_ARG_WITH(glut-includes,
    [  --with-glut-includes    where the glut includes are. ],
    [
       ac_glut_includes="$withval"
    ])

AC_ARG_WITH(glut-libraries,
    [  --with-glut-libraries   where the glut library is installed.],
    [
       ac_glut_libraries="$withval"
    ])

if test "$ac_glut_includes" = NO || test "$ac_glut_libraries" = NO; then

AC_CACHE_VAL(ac_cv_have_glut,
[#try to guess glut locations

GLTT_MAKE_TARGET=lib_and_demos

glut_incdirs="$ac_glut_includes /usr/include /usr/local/include /usr/glut/include /usr/local/glut/include $HOME/glut/include $HOME/Mesa/include $GLUT_INC $gl_includes"
test -n "$GLUTDIR" && glut_incdirs="$GLUTDIR/include $GLUTDIR $glut_incdirs"
AC_FIND_FILE(GL/glut.h, $glut_incdirs, glut_incdir)
ac_glut_includes=$glut_incdir

glut_libdirs="$ac_glut_libraries /usr/lib /usr/local/lib /usr/glut/lib/glut /usr/local/glut/lib/glut $HOME/glut/lib/glut $HOME/Mesa/lib $GLUTLIB $gl_libraries"
test -n "$GLUTDIR" && glut_libdirs="$GLUTDIR/lib/glut $GLUTDIR $glut_libdirs"
AC_FIND_FILE(libglut.so libglut.so.3.6 libglut.so.3 libglut.a libglut.sl, $glut_libdirs, glut_libdir)
ac_glut_libraries=$glut_libdir

ac_cxxflags_safe=$CXXFLAGS
ac_ldflags_safe=$LDFLAGS
ac_libs_safe=$LIBS

CXXFLAGS="$CXXFLAGS -I$glut_incdir -I$gl_includes"
LDFLAGS="-L$glut_libdir"
LIBS="-lglut -L$gl_libraries -l${GL_LIBNAME}GL -l${GL_LIBNAME}GLU $LIBSOCKET $LIBS"

AC_LANG_CPLUSPLUS
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "GL/glut.h"
int main( int argc, char** argv ) {
  glutInit(&argc, argv);
  return 0;
}
EOF

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
  ac_glut_libraries="NO"
fi
rm -f conftest*
CXXFLAGS=$ac_cxxflags_safe
LDFLAGS=$ac_ldflags_safe
LIBS=$ac_libs_safe

if test "$ac_glut_includes" = NO || test "$ac_glut_libraries" = NO; then
  ac_cv_have_glut="have_glut=no"
  ac_glut_notfound=""
  if test "$ac_glut_includes" = NO; then
    if test "$ac_glut_libraries" = NO; then
      ac_glut_notfound="(headers and libraries)";
    else
      ac_glut_notfound="(headers)";
    fi
  else
    ac_glut_notfound="(libraries)";
  fi

  AC_MSG_WARN([glut $ac_glut_notfound not found. Demos won't be built! ]);
  GLTT_MAKE_TARGET=lib_only
else
  have_glut="yes"
fi
ac_make_target=$GLTT_MAKE_TARGET
])
else
  have_glut="yes"
fi

eval "$ac_cv_have_glut"

if test "$have_glut" != yes; then
  AC_MSG_RESULT([$have_glut]);
else
  ac_cv_have_glut="have_glut=yes \
    ac_glut_includes=$ac_glut_includes ac_glut_libraries=$ac_glut_libraries ac_make_target=$GLTT_MAKE_TARGET"
  AC_MSG_RESULT([libraries $ac_glut_libraries, headers $ac_glut_includes])

  glut_libraries=$ac_glut_libraries
  glut_includes=$ac_glut_includes
  GLTT_MAKE_TARGET=$ac_make_target
fi

AC_SUBST(glut_libraries)
AC_SUBST(glut_includes)

if test "$glut_includes" = "$x_includes" || \
   test "$glut_includes" = "/usr/include" || \
   test -z "$glut_includes"; then
 GLUT_INCLUDES="";
else
 GLUT_INCLUDES="-I$glut_includes"
 all_includes="$GLUT_INCLUDES $all_includes"
fi

if test "$glut_libraries" = "$x_libraries" || \
   test "$glut_libraries" = "/usr/lib" || \
   test "$glut_libraries" = "/usr/local/lib" || \
   test -z "$glut_libraries"; then
 GLUT_LDFLAGS=""
else
 GLUT_LDFLAGS="-L$glut_libraries"
 all_libraries="$GLUT_LDFLAGS $all_libraries"
fi

AC_SUBST(GLUT_INCLUDES)
AC_SUBST(GLUT_LDFLAGS)
AC_SUBST(GLTT_MAKE_TARGET)
])

## ------------------------------------------------------------------------
## Try to find the GL/MesaGL headers and libraries.
## Try to find the GLU/MesaGLU headers and libraries.
## ------------------------------------------------------------------------

AC_DEFUN(AC_HAVE_GL,
[
AC_MSG_CHECKING([for GL])
ac_gl_includes=NO
ac_gl_libraries=NO
gl_libraries=""
gl_includes=""
AC_ARG_WITH(gl-dir,
    [  --with-gl-dir           where the root of OpenGL is installed ],
    [  ac_gl_includes="$withval"/include
       ac_gl_libraries="$withval"/lib
    ])

AC_ARG_WITH(gl-includes,
    [  --with-gl-includes      where the OpenGL includes are. ],
    [
       ac_gl_includes="$withval"
    ])

AC_ARG_WITH(gl-libraries,
    [  --with-gl-libraries     where the OpenGL library is installed.],
    [  ac_gl_libraries="$withval"
    ])

if test "$ac_gl_includes" = NO || test "$ac_gl_libraries" = NO; then

AC_CACHE_VAL(ac_cv_have_gl,
[#try to guess gl locations

gl_incdirs="$ac_gl_includes \
            /usr/include \
            /usr/X11R6/include \
            /usr/local/include \
            /usr/local/Mesa/include \
            $HOME/Mesa/include \
            $GL_INC"
test -n "$GLDIR" && gl_incdirs="$GLDIR/include $GLDIR $gl_incdirs"
AC_FIND_FILE(GL/gl.h, $gl_incdirs, gl_incdir)
ac_gl_includes=$gl_incdir

gl_libdirs="$ac_gl_libraries \
            /usr/lib \
            /usr/X11R6/lib \
            /usr/local/lib \
            /usr/local/Mesa/lib \
            $HOME/Mesa/lib \
            $GLLIB"
test -n "$GLDIR" && gl_libdirs="$GLDIR/lib $GLDIR $gl_libdirs"

GL_LIBNAME=""
AC_FIND_FILE(libGL.so libGL.a, $gl_libdirs, gl_libdir)
if test "$gl_libdir" = NO ; then
  AC_FIND_FILE(libMesaGL.so libMesaGL.a, $gl_libdirs, gl_libdir)
  ac_gl_libraries=$gl_libdir
  GL_LIBNAME="Mesa"
else
  ac_gl_libraries=$gl_libdir
  GL_LIBNAME=""
fi

AC_FIND_FILE(libGL.so libMesaGL.so libGL.a libMesaGL.a, $gl_libdirs, gl_libdir)
ac_gl_libraries=$gl_libdir

ac_cxxflags_safe=$CXXFLAGS
ac_ldflags_safe=$LDFLAGS
ac_libs_safe=$LIBS

CXXFLAGS="$CXXFLAGS -I$gl_incdir"
LDFLAGS="-L$gl_libdir"
LIBS="-lGL $X_LDFLAGS $LIBS"

AC_LANG_CPLUSPLUS
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "GL/gl.h"
int main() {
  glEnd();
  return 0;
}
EOF

if AC_TRY_EVAL(ac_link) && test -s conftest; then
  rm -f conftest*
else
  rm -f conftest*
  LIBS="$ac_libs_safe -lMesaGL $X_LDFLAGS -lX11 -lXext -lm"

cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "GL/gl.h"
int main() {
  glEnd();
  return 0;
}
EOF

  if AC_TRY_EVAL(ac_link) && test -s conftest; then
    rm -f conftest*
   else
    echo "configure: failed program was:" >&AC_FD_CC
    cat conftest.$ac_ext >&AC_FD_CC
    ac_gl_libraries="NO"
   fi
fi

rm -f conftest*
CXXFLAGS=$ac_cxxflags_safe
LDFLAGS=$ac_ldflags_safe
LIBS=$ac_libs_safe

if test "$ac_gl_includes" = NO || test "$ac_gl_libraries" = NO; then
  ac_cv_have_gl="have_gl=no"
  ac_gl_notfound=""
  if test "$ac_gl_includes" = NO; then
    if test "$ac_gl_libraries" = NO; then
      ac_gl_notfound="(headers and libraries)";
    else
      ac_gl_notfound="(headers)";
    fi
  else
    ac_gl_notfound="(libraries)";
  fi

  AC_MSG_ERROR([OpenGL $ac_gl_notfound not found. Please check your installation! ]);
else
  have_gl="yes"
fi
])
else
  have_gl="yes"
fi

eval "$ac_cv_have_gl"

if test "$have_gl" != yes; then
  AC_MSG_RESULT([$have_gl]);
else
  ac_cv_have_gl="have_gl=yes \
    ac_gl_includes=$ac_gl_includes ac_gl_libraries=$ac_gl_libraries"
  AC_MSG_RESULT([libraries $ac_gl_libraries/lib${GL_LIBNAME}GL*, headers $ac_gl_includes/GL])

  gl_libraries=$ac_gl_libraries
  gl_includes=$ac_gl_includes
fi

AC_SUBST(gl_libraries)
AC_SUBST(gl_includes)

if test "$gl_includes" = "$x_includes" || \
   test "$gl_includes" = "/usr/include" || \
   test -z "$gl_includes"; then
 GL_INCLUDES="";
else
 GL_INCLUDES="-I$gl_includes"
 all_includes="$GL_INCLUDES $all_includes"
fi

if test "$gl_libraries" = "$x_libraries" || \
   test "$gl_libraries" = "/usr/lib" || \
   test "$gl_libraries" = "/usr/local/lib" || \
   test -z "$gl_libraries"; then
 GL_LDFLAGS=""
else
 GL_LDFLAGS="-L$gl_libraries"
 all_libraries="$GL_LDFLAGS $all_libraries"
fi

AC_SUBST(GL_INCLUDES)
AC_SUBST(GL_LDFLAGS)
AC_SUBST(GL_LIBNAME)
])

## ------------------------------------------------------------------------
## Check for libtool
## ------------------------------------------------------------------------
##
dnl just a wrapper to clean up configure.in
AC_DEFUN(GLTT_PROG_LIBTOOL,
[
dnl libtool is only for C, so I must force him
dnl to find the correct flags for C++

ac_save_cc=$CC
ac_save_cflags=$CFLAGS
CC=$CXX
CFLAGS=$CXXFLAGS
AM_PROG_LIBTOOL dnl for libraries
CC=$ac_save_cc
CFLAGS=$ac_save_cflags
])
