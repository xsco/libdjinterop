AC_INIT([libengineprime], [1.0.0], [bugs@xsco.org], [libengineprime])
#AM_SILENT_RULES([yes])
AM_INIT_AUTOMAKE([-Wall -Werror])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX
AC_LANG([C++])

AM_PROG_CC_C_O
AM_PROG_AR

LT_INIT

# Require C++11
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])

# Check some mandatory dependencies.
PKG_CHECK_MODULES([ZLIB], [zlib])
AC_CHECK_HEADERS([boost/uuid/uuid.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/uuid/uuid.hpp])])
AC_CHECK_HEADERS([boost/uuid/uuid_generators.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/uuid/uuid_generators.hpp])])
AC_CHECK_HEADERS([boost/uuid/uuid_io.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/uuid/uuid_io.hpp])])
AC_CHECK_HEADERS([boost/iostreams/filter/zlib.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/iostreams/filter/zlib.hpp])])

# Check optional dependencies.
AC_CHECK_HEADERS([boost/test/unit_test.hpp], [],
    [AC_MSG_WARN([couldn't find boost/test/unit_test.hpp, which is needed for unit testing])])
AC_CHECK_HEADERS([boost/filesystem.hpp], [],
    [AC_MSG_WARN([couldn't find boost/filesystem.hpp, which is needed for unit testing])])

# Library names.
AC_SUBST([BOOST_SYSTEM_LIBS], [-lboost_system])
AC_SUBST([BOOST_IOSTREAMS_LIBS], [-lboost_iostreams])
AC_SUBST([BOOST_FILESYSTEM_LIBS], [-lboost_filesystem])
AC_SUBST([BOOST_UTF_LIBS], [-lboost_unit_test_framework])

# Consider generating a pkg-config file:
# https://www.gnu.org/software/autoconf-archive/ax_create_pkgconfig_info.html

# Makefiles
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT

