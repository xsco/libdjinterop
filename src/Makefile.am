lib_LTLIBRARIES = libengineprime.la

# TODO - add support for generation of documentation

libengineprime_la_SOURCES = \
	sqlite3.c sqlite3.h \
	sqlite_modern_cpp.h sqlite_modern_cpp/errors.h sqlite_modern_cpp/log.h \
	sqlite_modern_cpp/sqlcipher.h sqlite_modern_cpp/lists/error_codes.h \
	sqlite_modern_cpp/utility/function_traits.h \
	sqlite_modern_cpp/utility/uncaught_exceptions.h \
	sqlite_modern_cpp/utility/variant.h \
	schema.cpp schema.hpp \
	database.cpp \
	track.cpp \
	performance_data_format.cpp performance_data_format.hpp \
	performance_data.cpp
libengineprime_la_CXXFLAGS = $(ZLIB_CFLAGS)
libengineprime_la_LDFLAGS = -version-info $(LIBENGINEPRIME_INTERFACE_VERSION)
libengineprime_la_LIBADD = $(ZLIB_LIBS) $(BOOST_IOSTREAMS_LIBS)

nobase_include_HEADERS = \
	engineprime/engineprime.hpp \
	engineprime/schema_version.hpp \
	engineprime/database.hpp \
	engineprime/track.hpp
	engineprime/performance_data.hpp
	engineprime/album_art.hpp

# Unit test programs
check_PROGRAMS = database_test track_test schema_test performance_data_test
TESTS = $(check_PROGRAMS)

# Database test
database_test_SOURCES = \
	database_test.cpp \
	database.cpp
database_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
database_test_LDADD = \
	$(BOOST_UTF_LIBS) \
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
	$(BOOST_UTF_LIBS) \
	libengineprime.la \
	-lpthread -ldl

# Schema test
schema_test_SOURCES = \
	schema_test.cpp \
	schema.cpp 
schema_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
schema_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	libengineprime.la \
	-lpthread -ldl

# Performance data test
performance_data_test_SOURCES = \
	performance_data_test.cpp \
	performance_data_format.cpp performance_data_format.hpp \
	performance_data.cpp 
performance_data_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
performance_data_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	libengineprime.la \
	-lpthread -ldl

