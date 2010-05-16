#  - MACRO_CREATE_INSTALL_DIRECTORY(name component )
#
#  Create a plugin from the given source files.
#  If WITH_PREFIX is given, the resulting plugin will have the
#  prefix "lib", otherwise it won't.
#
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
# Copyright (c) 2006, Andreas Schneider, <mail@cynapses.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_CREATE_COMPONENT _comp_NAME _comp_DESC)

string(TOUPPER "${_comp_NAME}" _comp_NAME_p)

set(CPACK_COMPONENT_${_comp_NAME_p}_DISPLAY_NAME "${_comp_NAME}" CACHE INTERNAL "${_comp_NAME}")
set(CPACK_COMPONENT_${_comp_NAME_p}_DESCRIPTION  "${_comp_DESC}" CACHE INTERNAL "${_comp_NAME}")  

endmacro (MACRO_CREATE_COMPONENT _comp_NAME _comp_DESC)



