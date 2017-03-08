INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ISM ism)

FIND_PATH(
    ISm_INCLUDE_DIRS
    NAMES ism/api.h
    HINTS $ENV{ISM_DIR}/include
    ${PC_ISM_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREEFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ISM_LIBRARIES
    NAMES gnuradio-ism
    HINTS $ENV{ISM_DIR}/lib
    ${PC_ISM_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ISM DEFAULT_MSG ISM_LIBRARIES ISM_INCLUDE_DIRS)
MARK_AS_ADVANCED(ISM_LIBRARIES ISM_INCLUDE_DIRS)

