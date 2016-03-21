kde_enable_exceptions()
include(CheckIncludeFiles)
include(CheckFunctionExists)

check_include_files(ieeefp.h HAVE_IEEEFP_H)

set(CMAKE_REQUIRED_INCLUDES "math.h")
set(CMAKE_REQUIRED_LIBRARIES m)
check_function_exists(trunc      HAVE_TRUNC)
set(CMAKE_REQUIRED_INCLUDES)
set(CMAKE_REQUIRED_LIBRARIES)

# at the end, output the configuration
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/config-kig.h.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/config-kig.h
)


