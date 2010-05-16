#  - MACRO_ADD_PACKAGE(nome_do_pacote versao versao_pacote mantenedor sumario decricao dependencia scripts_control secao )
#
# Copyright (c) 2009, Trópico Telecomunicações Avançadas, <via@tropiconet.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

macro (MACRO_ADD_PACKAGE _PKG_NAME _VERSION _PKG_VERSION _MAINTAINER _SUMMARY _DESCRIPTION _DEPENDS _CONTROL _SECTION)

# marca pacote com Debug para diferenciação
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CPACK_PACKAGE_NAME "${_PKG_NAME}-via-dbg")
else(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CPACK_PACKAGE_NAME "${_PKG_NAME}-via")
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

set(CPACK_PACKAGE_VERSION "${_VERSION}") 
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${_MAINTAINER}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${_SUMMARY}")
set(CPACK_PACKAGE_DESCRIPTION "${_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "${_DEPENDS}")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${_CONTROL}")
set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${_PKG_VERSION}")
set(CPACK_DEBIAN_PACKAGE_SECTION "${_SECTION}")

endmacro (MACRO_ADD_PACKAGE _PKG_NAME _VERSION _PKG_VERSION _MAINTAINER _SUMMARY _DESCRIPTION _DEPENDS _CONTROL _SECTION)



