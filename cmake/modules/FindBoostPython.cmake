# - Try to find the a valid boost+python combination
# Once done this will define
#
#  BoostPython_FOUND - system has a valid boost+python combination
#  BoostPython_INCLUDE_DIRS - the include directory for boost+python
#  BoostPython_LIBRARIES - the needed libs for boost+python

# SPDX-FileCopyrightText: 2006 Pino Toscano <toscano.pino@tiscali.it>
# SPDX-License-Identifier: BSD-3-Clause

if(BoostPython_INCLUDE_DIRS AND BoostPython_LIBRARIES)
    # Already in cache, be silent
	set(BoostPython_FIND_QUIETLY TRUE)
endif(BoostPython_INCLUDE_DIRS AND BoostPython_LIBRARIES)

include(CheckIncludeFileCXX)
include(CMakePushCheckState)
include(FindPackageHandleStandardArgs)

# Set varname to true if we found good combination of Boost and Python.
# NOTE: varname should be different each time, or test won't run.
# Also, varname is printed as test name, so it should not be too cryptic.
macro(BoostPython_TRY_COMPILE varname)
  check_cxx_source_compiles("
#include <boost/python.hpp>
const char* greet() { return \"Hello world!\"; }
BOOST_PYTHON_MODULE(hello) { boost::python::def(\"greet\", greet); }
int main() { return 0; }
" ${varname} )
endmacro(BoostPython_TRY_COMPILE)

find_package(Boost QUIET COMPONENTS python3)
if(NOT Boost_PYTHON3_FOUND)
  find_package(Boost QUIET COMPONENTS python)
  if(Boost_PYTHON_FOUND)
    set(Boost_PYTHON3_LIBRARY ${Boost_PYTHON_LIBRARY})
    set(Boost_PYTHON3_FOUND TRUE)
  endif(Boost_PYTHON_FOUND)
endif(NOT Boost_PYTHON3_FOUND)

cmake_push_check_state()
set(CMAKE_REQUIRED_INCLUDES  ${CMAKE_REQUIRED_INCLUDES}  ${Boost_INCLUDE_DIRS})
set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${Boost_PYTHON3_LIBRARY})

check_include_file_cxx(boost/shared_ptr.hpp HAVE_BOOST_SHARED_PTR_HPP)

# This variable is not checked/used when user provide both
# BoostPython_INCLUDE_DIRS and BoostPython_LIBRARIES, by design.
# If user wants to speed up configure stage, he's allowed to do this.
# If user wants to shoot himself in the foot, he is allowed to do this, too.
set(BoostPython_COMPILES)

# If shared_ptr.hpp or Python library is not available, then there is
# no point to do anything.
if(HAVE_BOOST_SHARED_PTR_HPP AND Boost_PYTHON3_FOUND)
  if(NOT BoostPython_INCLUDE_DIRS OR NOT BoostPython_LIBRARIES)
    # First try: check if CMake Python is suitable.
    set(Python_ADDITIONAL_VERSIONS "3.9;3.8;3.7;3.6;3.5")
    find_package(PythonLibs 3 QUIET)
    if(PYTHONLIBS_FOUND)
      cmake_push_check_state()
      set(CMAKE_REQUIRED_INCLUDES  ${CMAKE_REQUIRED_INCLUDES}  ${PYTHON_INCLUDE_DIRS})
      set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${PYTHON_LIBRARIES})
      BoostPython_TRY_COMPILE(BoostPython_FromCMake)
      cmake_pop_check_state()

      if(BoostPython_FromCMake)
        set(BoostPython_COMPILES Yes)
        set(BoostPython_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
            CACHE INTERNAL "Includes search path for Boost+Python")
        set(BoostPython_LIBRARIES    ${PYTHON_LIBRARIES}    ${Boost_PYTHON3_LIBRARY}
            CACHE INTERNAL "Linker flags for Boost+Python")
      endif(BoostPython_FromCMake)
    endif(PYTHONLIBS_FOUND)
  endif(NOT BoostPython_INCLUDE_DIRS OR NOT BoostPython_LIBRARIES)

  if(NOT BoostPython_INCLUDE_DIRS OR NOT BoostPython_LIBRARIES)
    # Second try: try pkg-config way
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
      # Prefer the -embed version from Python 3.8, as it is the way to
      # actually link against the Python library.
      set(PYTHON_VERSIONS "python3-embed;python-3.9-embed;python-3.8-embed;python3;python-3.7;python-3.6;python-3.5")
      foreach(_pyver ${PYTHON_VERSIONS})
        if(NOT BoostPython_INCLUDES OR NOT BoostPython_LIBS)
          pkg_check_modules(${_pyver} QUIET ${_pyver})
          if(${_pyver}_FOUND)
            cmake_push_check_state()
            set(CMAKE_REQUIRED_INCLUDES  ${CMAKE_REQUIRED_INCLUDES}  ${${_pyver}_INCLUDE_DIRS})
            set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${${_pyver}_LDFLAGS})
            BoostPython_TRY_COMPILE(BoostPython_${_pyver})
            cmake_pop_check_state()

            if(BoostPython_${_pyver})
              set(BoostPython_COMPILES Yes)
              set(BoostPython_INCLUDE_DIRS ${${_pyver}_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                  CACHE INTERNAL "Includes search path for Boost+Python")
              set(BoostPython_LIBRARIES    ${${_pyver}_LDFLAGS}      ${Boost_PYTHON3_LIBRARY}
                  CACHE INTERNAL "Linker flags for Boost+Python")
            endif(BoostPython_${_pyver})

          endif(${_pyver}_FOUND)
        endif(NOT BoostPython_INCLUDES OR NOT BoostPython_LIBS)
      endforeach(_pyver ${PYTHON_VERSIONS})
    endif(PKG_CONFIG_FOUND)
  endif(NOT BoostPython_INCLUDE_DIRS OR NOT BoostPython_LIBRARIES )
endif(HAVE_BOOST_SHARED_PTR_HPP AND Boost_PYTHON3_FOUND)

cmake_pop_check_state()

find_package_handle_standard_args(BoostPython
                                  FOUND_VAR BoostPython_FOUND
                                  REQUIRED_VARS BoostPython_LIBRARIES BoostPython_INCLUDE_DIRS
                                 )

mark_as_advanced(BoostPython_INCLUDE_DIRS BoostPython_LIBRARIES)
