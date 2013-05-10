# cmake/modules/python.cmake
#
# Copyright (C) 2006  Alan W. Irwin
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

# Module for determining Python bindings configuration options

# Options to enable Python bindings
if(DEFAULT_NO_BINDINGS)
  option(ENABLE_python "Enable Python bindings" OFF)
else(DEFAULT_NO_BINDINGS)
  option(ENABLE_python "Enable Python bindings" ON)
endif(DEFAULT_NO_BINDINGS)

if(ENABLE_python AND NOT BUILD_SHARED_LIBS)
  message(STATUS "WARNING: "
    "Python requires shared libraries. Disabling Python bindings")
  set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif(ENABLE_python AND NOT BUILD_SHARED_LIBS)

if(ENABLE_python AND NOT SWIG_FOUND)
  message(STATUS "WARNING: "
    "swig not found. Disabling Python bindings")
  set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
endif(ENABLE_python AND NOT SWIG_FOUND)

if(ENABLE_python)
  # Check for Python interpreter (which also defines PYTHON_EXECUTABLE)
  find_package(PythonInterp)
  if(NOT PYTHONINTERP_FOUND)
    message(STATUS "WARNING: "
      "Python interpreter not found. Disabling Python bindings")
    set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
  endif(NOT PYTHONINTERP_FOUND)
endif(ENABLE_python)

if(ENABLE_python)
  # Check for Python libraries which defines
  #  PYTHON_LIBRARIES     = path to the Python library
  #  PYTHON_INCLUDE_PATH  = path to where Python.h is found
  find_package(PythonLibs)
  if(NOT PYTHON_LIBRARIES OR NOT PYTHON_INCLUDE_PATH)
    message(STATUS "WARNING: "
      "Python library and/or header not found. Disabling Python bindings")
    set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
  endif(NOT PYTHON_LIBRARIES OR NOT PYTHON_INCLUDE_PATH)
endif(ENABLE_python)

option(HAVE_NUMPY "Have the numpy package" ON)

if(ENABLE_python)
  # NUMERIC_INCLUDE_PATH = path to arrayobject.h for numpy.
  #message(STATUS "DEBUG: NUMERIC_INCLUDE_PATH = ${NUMERIC_INCLUDE_PATH}") 
  if(NOT NUMERIC_INCLUDE_PATH)
    if(HAVE_NUMPY)
      # First check for new version of numpy 
      execute_process(
	COMMAND
	${PYTHON_EXECUTABLE} -c "import numpy; print numpy.get_include()"
	OUTPUT_VARIABLE NUMPY_INCLUDE_PATH
	RESULT_VARIABLE NUMPY_ERR
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)
      if(NUMPY_ERR)
	set(HAVE_NUMPY OFF CACHE BOOL "Have the numpy package" FORCE)
      endif(NUMPY_ERR)
    endif(HAVE_NUMPY)

    if(HAVE_NUMPY)
      # We use the full path name (including numpy on the end), but
      # Double-check that all is well with that choice.
      find_path(
	NUMERIC_INCLUDE_PATH
	arrayobject.h
	${NUMPY_INCLUDE_PATH}/numpy
	)
      if(NUMERIC_INCLUDE_PATH)
	set(PYTHON_NUMERIC_NAME numpy CACHE INTERNAL "")
      endif(NUMERIC_INCLUDE_PATH)
    endif(HAVE_NUMPY)

  endif(NOT NUMERIC_INCLUDE_PATH)

  if(NOT NUMERIC_INCLUDE_PATH)
    message(STATUS "WARNING: "
	"NumPy header not found. Disabling Python bindings")
    set(ENABLE_python OFF CACHE BOOL "Enable Python bindings" FORCE)
  endif(NOT NUMERIC_INCLUDE_PATH)
endif(ENABLE_python)

if(ENABLE_python AND HAVE_NUMPY)
  # This numpy installation bug found by Geoff.
  option(EXCLUDE_PYTHON_LIBRARIES "Linux temporary workaround for numpy installation bug for non-system Python install prefix" OFF)
  if(EXCLUDE_PYTHON_LIBRARIES)
    set(PYTHON_LIBRARIES)
  endif(EXCLUDE_PYTHON_LIBRARIES)
endif(ENABLE_python AND HAVE_NUMPY)

if(ENABLE_python)
  # N.B. This is a nice way to obtain all sorts of Python information
  # using distutils.
  execute_process(
    COMMAND
    ${PYTHON_EXECUTABLE} -c "from distutils import sysconfig; print sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_EXEC_PREFIX}')"
    OUTPUT_VARIABLE PYTHON_INSTDIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  # Get the Python version.
  execute_process(
    COMMAND
    ${PYTHON_EXECUTABLE} -c "import sys; print('%s.%s.%s' % sys.version_info[0:3])"
    OUTPUT_VARIABLE PYTHON_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  message(STATUS "PYTHON_VERSION = ${PYTHON_VERSION}")

  # Enable plsmem if the Python and Swig versions support it
  transform_version(NUMERICAL_SWIG_MINIMUM_VERSION_FOR_PLSMEM "1.3.38")
  transform_version(NUMERICAL_PYTHON_MINIMUM_VERSION_FOR_PLSMEM "2.6.0")
  transform_version(NUMERICAL_SWIG_VERSION "${SWIG_VERSION}")
  transform_version(NUMERICAL_PYTHON_VERSION "${PYTHON_VERSION}")

  SET(PYTHON_HAVE_PYBUFFER OFF)
  IF(NUMERICAL_SWIG_MINIMUM_VERSION_FOR_PLSMEM LESS NUMERICAL_SWIG_VERSION)
    IF(NUMERICAL_PYTHON_MINIMUM_VERSION_FOR_PLSMEM LESS NUMERICAL_PYTHON_VERSION)
      message(STATUS "Building Python binding with plsmem() support")
      SET(PYTHON_HAVE_PYBUFFER ON)
    ENDIF(NUMERICAL_PYTHON_MINIMUM_VERSION_FOR_PLSMEM LESS NUMERICAL_PYTHON_VERSION)
  ENDIF(NUMERICAL_SWIG_MINIMUM_VERSION_FOR_PLSMEM LESS NUMERICAL_SWIG_VERSION)

endif(ENABLE_python)
