# - Try to find the a valid boost+python combination
# Once done this will define
#
#  BOOST_PYTHON_FOUND - system has a valid boost+python combination
#  BOOST_PYTHON_INCLUDES - the include directory for boost+python
#  BOOST_PYTHON_LIBS - the needed libs for boost+python

# Copyright (c) 2006, Pino Toscano, <toscano.pino@tiscali.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(BOOST_PYTHON_INCLUDES AND BOOST_PYTHON_LIBS)
    # Already in cache, be silent
	set(BOOST_PYTHON_FIND_QUIETLY TRUE)
endif(BOOST_PYTHON_INCLUDES AND BOOST_PYTHON_LIBS)

include(CheckIncludeFileCXX)

find_package(PkgConfig)

# reset vars
set(BOOST_PYTHON_INCLUDES)
set(BOOST_PYTHON_LIBS)

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
        find_package(Boost 1.33 COMPONENTS python)
        if (Boost_PYTHON_FOUND)
          set(_found TRUE)
          set(BOOST_PYTHON_INCLUDES "${_python_INCLUDE_DIRS};${Boost_INCLUDE_DIRS}")
          set(BOOST_PYTHON_LIBS "${_python_LDFLAGS} ${Boost_PYTHON_LIBRARY}")
        endif(Boost_PYTHON_FOUND)
      endif(_python_FOUND)
    endif(NOT _found)
  endforeach(_pyver ${PYTHON_VERSIONS})

endif(HAVE_BOOST_SHARED_PTR_HPP)

if(BOOST_PYTHON_INCLUDES AND BOOST_PYTHON_LIBS)
  set(BOOST_PYTHON_FOUND TRUE)
endif(BOOST_PYTHON_INCLUDES AND BOOST_PYTHON_LIBS)

if(BOOST_PYTHON_FOUND)
  if(NOT BoostPython_FIND_QUIETLY)
    message(STATUS "Found Boost+Python: libs ${BOOST_PYTHON_LIBS}, headers ${BOOST_PYTHON_INCLUDES}")
  endif(NOT BoostPython_FIND_QUIETLY)
  set(KIG_ENABLE_PYTHON_SCRIPTING 1)
else (BOOST_PYTHON_FOUND)
  if (BoostPython_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find Boost+Python")
  endif(BoostPython_FIND_REQUIRED)
  set(KIG_ENABLE_PYTHON_SCRIPTING 0)
endif(BOOST_PYTHON_FOUND)

mark_as_advanced(BOOST_PYTHON_INCLUDES BOOST_PYTHON_LIBS)
