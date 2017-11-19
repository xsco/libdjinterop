lib_LTLIBRARIES = libengineprime.la

libengineprime_la_SOURCES = \
	sqlite3.c sqlite3.h sqlite3_db_raii.hpp \
	database.cpp \
	track.cpp
libengineprime_la_LDFLAGS = -version-info 1:0:0

nobase_include_HEADERS = \
	engineprime/engineprime.hpp \
	engineprime/database.hpp \
	engineprime/track.hpp

# Unit test programs
check_PROGRAMS = database_test track_test
TESTS = $(check_PROGRAMS)

# Database test
database_test_SOURCES = \
	database_test.cpp \
	database.cpp
database_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
database_test_LDADD = \
	$(BOOST_UTF_LDADD) \
	libengineprime.la \
	-lpthread -ldl

# Track test
track_test_SOURCES = \
	track_test.cpp \
	track.cpp \
	database.cpp
track_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
track_test_LDADD = \
	$(BOOST_UTF_LDADD) \
	libengineprime.la \
	-lpthread -ldl

