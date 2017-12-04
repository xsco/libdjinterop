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
AC_CHECK_HEADERS([boost/uuid/uuid.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/uuid/uuid.hpp])])
AC_CHECK_HEADERS([boost/uuid/uuid_generators.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/uuid/uuid_generators.hpp])])
AC_CHECK_HEADERS([boost/uuid/uuid_io.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/uuid/uuid_io.hpp])])
AC_CHECK_HEADERS([boost/test/unit_test.hpp], [],
    [AC_MSG_ERROR([couldn't find boost/test/unit_test.hpp])])

# Library names.
AC_SUBST([BOOST_SYSTEM_LDADD], [-lboost_system])
AC_SUBST([BOOST_UTF_LDADD], [-lboost_unit_test_framework])

# Makefiles
AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT

