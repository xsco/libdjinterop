lib_LTLIBRARIES = libengineprime.la

libengineprime_la_SOURCES = \
	sqlite3.c sqlite3.h \
	sqlite_modern_cpp.h sqlite_modern_cpp/errors.h sqlite_modern_cpp/log.h \
	sqlite_modern_cpp/sqlcipher.h sqlite_modern_cpp/lists/error_codes.h \
	sqlite_modern_cpp/utility/function_traits.h \
	sqlite_modern_cpp/utility/uncaught_exceptions.h \
	sqlite_modern_cpp/utility/variant.h \
	schema.cpp schema.hpp \
	database.cpp \
	track.cpp
libengineprime_la_LDFLAGS = -version-info 1:0:0

nobase_include_HEADERS = \
	engineprime/engineprime.hpp \
	engineprime/schema_version.hpp \
	engineprime/database.hpp \
	engineprime/track.hpp
	engineprime/performance_data.hpp
	engineprime/album_art.hpp

# Unit test programs
check_PROGRAMS = database_test track_test schema_test
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

# Schema test
schema_test_SOURCES = \
	schema_test.cpp \
	schema.cpp 
schema_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
schema_test_LDADD = \
	$(BOOST_SYSTEM_LDADD) \
	$(BOOST_FILESYSTEM_LDADD) \
	$(BOOST_UTF_LDADD) \
	libengineprime.la \
	-lpthread -ldl

