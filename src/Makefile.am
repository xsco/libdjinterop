lib_LTLIBRARIES = libdjinterop.la

libdjinterop_la_SOURCES = \
	sqlite3.c sqlite3.h \
	sqlite_modern_cpp.h sqlite_modern_cpp/errors.h sqlite_modern_cpp/log.h \
	sqlite_modern_cpp/sqlcipher.h sqlite_modern_cpp/lists/error_codes.h \
	sqlite_modern_cpp/utility/function_traits.h \
	sqlite_modern_cpp/utility/uncaught_exceptions.h \
	sqlite_modern_cpp/utility/variant.h \
	enginelibrary/schema.cpp enginelibrary/schema.hpp \
	enginelibrary/schema_validation_utils.hpp \
	enginelibrary/schema_1_6_0.cpp enginelibrary/schema_1_6_0.hpp \
	enginelibrary/schema_1_7_1.cpp enginelibrary/schema_1_7_1.hpp \
	enginelibrary/database.cpp \
	enginelibrary/track.cpp \
	enginelibrary/crate.cpp \
	enginelibrary/encode_decode_utils.cpp enginelibrary/encode_decode_utils.hpp \
	enginelibrary/performance_data_format.cpp enginelibrary/performance_data_format.hpp \
	enginelibrary/performance_data.cpp
libdjinterop_la_CXXFLAGS = $(ZLIB_CFLAGS)
libdjinterop_la_LDFLAGS = -version-info $(LIBRARY_INTERFACE_VERSION)
libdjinterop_la_LIBADD = $(ZLIB_LIBS)

nobase_include_HEADERS = \
	djinterop/enginelibrary.hpp \
	djinterop/enginelibrary/album_art.hpp \
	djinterop/enginelibrary/crate.hpp \
	djinterop/enginelibrary/database.hpp \
	djinterop/enginelibrary/musical_key.hpp \
	djinterop/enginelibrary/pad_colour.hpp \
	djinterop/enginelibrary/performance_data.hpp \
	djinterop/enginelibrary/schema_version.hpp \
	djinterop/enginelibrary/track.hpp

# Unit test programs
check_PROGRAMS = \
	el_database_test \
	el_track_test \
	el_crate_test \
	el_schema_test \
	el_performance_data_test
TESTS = $(check_PROGRAMS)

# Database test
el_database_test_SOURCES = \
	enginelibrary/database_test.cpp \
	enginelibrary/database.cpp \
	enginelibrary/schema.cpp
el_database_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
el_database_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	$(ZLIB_LIBS) \
	libdjinterop.la \
	-lpthread -ldl

# Track test
el_track_test_SOURCES = \
	enginelibrary/track_test.cpp \
	enginelibrary/track.cpp \
	enginelibrary/database.cpp
el_track_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
el_track_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	$(ZLIB_LIBS) \
	libdjinterop.la \
	-lpthread -ldl

# Crate test
el_crate_test_SOURCES = \
	enginelibrary/crate_test.cpp \
	enginelibrary/crate.cpp \
	enginelibrary/database.cpp
el_crate_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
el_crate_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	$(ZLIB_LIBS) \
	libdjinterop.la \
	-lpthread -ldl

# Schema test
el_schema_test_SOURCES = \
	enginelibrary/schema_test.cpp \
	enginelibrary/schema.cpp 
el_schema_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
el_schema_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	$(ZLIB_LIBS) \
	libdjinterop.la \
	-lpthread -ldl

# Performance data test
el_performance_data_test_SOURCES = \
	enginelibrary/performance_data_test.cpp \
	enginelibrary/performance_data_format.cpp performance_data_format.hpp \
	enginelibrary/performance_data.cpp 
el_performance_data_test_CXXFLAGS = \
	-DTESTDATA_DIR="../testdata"
el_performance_data_test_LDADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_UTF_LIBS) \
	$(ZLIB_LIBS) \
	libdjinterop.la \
	-lpthread -ldl

