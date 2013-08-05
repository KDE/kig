# - Try to find the a valid boost+python combination
# Once done this will define
#
#  BoostPython_FOUND - system has a valid boost+python combination
#  BoostPython_INCLUDE_DIRS - the include directory for boost+python
#  BoostPython_LIBRARIES - the needed libs for boost+python

# Copyright (c) 2006, Pino Toscano, <toscano.pino@tiscali.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(BoostPython_INCLUDE_DIRS AND BoostPython_LIBRARIES)
    # Already in cache, be silent
	set(BoostPython_FIND_QUIETLY TRUE)
endif(BoostPython_INCLUDE_DIRS AND BoostPython_LIBRARIES)

include(CheckIncludeFileCXX)

find_package(PkgConfig)

# reset vars
set(BoostPython_INCLUDE_DIRS)
set(BoostPython_LIBRARIES)

# handy arrays
set(PYTHON_VERSIONS "python;python2.7;python2.6;python2.5;python2.4;python2.3;python2.2")

# 1st: check for boost/shared_ptr.hpp
check_include_file_cxx(boost/shared_ptr.hpp HAVE_BOOST_SHARED_PTR_HPP)

if(HAVE_BOOST_SHARED_PTR_HPP)

  # try pkg-config next
  set(_found FALSE)
  foreach(_pyver ${PYTHON_VERSIONS})
    if(NOT _found)
      pkg_check_modules(_python QUIET ${_pyver})
      if (_python_FOUND)
        find_package(Boost 1.31 COMPONENTS python)
        if (Boost_PYTHON_FOUND)
          set(_found TRUE)
          set(BoostPython_INCLUDE_DIRS "${_python_INCLUDE_DIRS};${Boost_INCLUDE_DIRS}")
          set(BoostPython_LIBRARIES "${_python_LDFLAGS} ${Boost_PYTHON_LIBRARY}")
        endif(Boost_PYTHON_FOUND)
      endif(_python_FOUND)
    endif(NOT _found)
  endforeach(_pyver ${PYTHON_VERSIONS})

endif(HAVE_BOOST_SHARED_PTR_HPP)

if(BoostPython_INCLUDE_DIRS AND BoostPython_LIBRARIES)
  set(BoostPython_FOUND TRUE)
endif(BoostPython_INCLUDE_DIRS AND BoostPython_LIBRARIES)

if(BoostPython_FOUND)
  if(NOT BoostPython_FIND_QUIETLY)
    message(STATUS "Found Boost+Python: libs ${BoostPython_LIBRARIES}, headers ${BoostPython_INCLUDE_DIRS}")
  endif(NOT BoostPython_FIND_QUIETLY)
  set(KIG_ENABLE_PYTHON_SCRIPTING 1)
else (BoostPython_FOUND)
  if (BoostPython_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Boost+Python")
  endif(BoostPython_FIND_REQUIRED)
  set(KIG_ENABLE_PYTHON_SCRIPTING 0)
endif(BoostPython_FOUND)

mark_as_advanced(BoostPython_INCLUDE_DIRS BoostPython_LIBRARIES)
