# define system dependent compiler flags

include(CheckCCompilerFlag)

# configura as opções de compilação de cada build
set(CMAKE_C_FLAGS_PROFILING " -g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wwrite-strings -fprofile-arcs -ftest-coverage" CACHE STRING "Profiling Compiler Flags")
set(CMAKE_SHARED_LINKER_FLAGS_PROFILING " -fprofile-arcs -ftest-coverage" CACHE STRING "Profiling Linker Flags")
set(CMAKE_MODULE_LINKER_FLAGS_PROFILING " -fprofile-arcs -ftest-coverage" CACHE STRING "Profiling Linker Flags")
set(CMAKE_EXEC_LINKER_FLAGS_PROFILING " -fprofile-arcs -ftest-coverage" CACHE STRING "Profiling Linker Flags")

set(CMAKE_C_FLAGS_DEBUG " -g" CACHE STRING "Debug Compiler Flags")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "" CACHE STRING "Debug Linker Flags")
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "" CACHE STRING "Debug Linker Flags")
set(CMAKE_EXEC_LINKER_FLAGS_DEBUG "" CACHE STRING "Debug Linker Flags")

# quando solicitado RELEASE não sera compilado com nenhuma otimização
set(CMAKE_C_FLAGS_RELEASE "-O0" CACHE STRING "Release Compiler Flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "" CACHE STRING "Release Linker Flags" FORCE)
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "" CACHE STRING "Release Linker Flags" FORCE)
set(CMAKE_EXEC_LINKER_FLAGS_RELEASE "" CACHE STRING "Release Linker Flags" FORCE)

set(CMAKE_C_FLAGS_MINSIZEREL " -Os -DNDEBUG" CACHE STRING "Min Size Release Compiler Flags")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "" CACHE STRING "Min Size Release Compiler Flags")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "" CACHE STRING "Min Size Release Compiler Flags")
set(CMAKE_EXEC_LINKER_FLAGS_MINSIZEREL "" CACHE STRING "Min Size Release Compiler Flags")

set(CMAKE_C_FLAGS_RELWITHDEBINFO " -O2 -g" CACHE STRING "Release With Debug Info Compiler Flags")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "Release With Debug Info Compiler Flags")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "Release With Debug Info Compiler Flags")
set(CMAKE_EXEC_LINKER_FLAGS_RELWITHDEBINFO "" CACHE STRING "Release With Debug Info Compiler Flags")

if (UNIX AND NOT WIN32)
  # with -fPIC
  check_c_compiler_flag("-fPIC" WITH_FPIC)
  if (WITH_FPIC)
    add_definitions(-fPIC)
  endif (WITH_FPIC)

  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
    # with large file support
    execute_process(
      COMMAND
        getconf LFS64_CFLAGS
      OUTPUT_VARIABLE
        _lfs_CFLAGS
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  else (CMAKE_SIZEOF_VOID_P MATCHES "8")
    # with large file support
    execute_process(
      COMMAND
        getconf LFS_CFLAGS
      OUTPUT_VARIABLE
        _lfs_CFLAGS
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")

#  string(REGEX REPLACE "[\r\n]" " " ${_lfs_CFLAGS} "${${_lfs_CFLAGS}}")
#  add_definitions(${_lfs_CFLAGS})

#  add_definitions(-Wall -Wextra -Wmissing-prototypes -Wdeclaration-after-statement -Wunused)
  add_definitions(-Wall -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers)
  
  if(WITH_LOG_TOOLS)
    add_definitions(-DENABLE_LOG_TOOLS)
  endif(WITH_LOG_TOOLS)

  # para ARM
  if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
    add_definitions(-D__ARM_PROCESSOR__)
  endif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
  
  # TODO: reentrance?
  add_definitions(-D_REENTRANT)

  check_c_compiler_flag("-fstack-protector" WITH_STACK_PROTECTOR)
  if (WITH_STACK_PROTECTOR)
    add_definitions(-fstack-protector)
  endif (WITH_STACK_PROTECTOR)

  check_c_compiler_flag("-D_FORTIFY_SOURCE=2" WITH_FORTIFY_SOURCE)
  if (WITH_FORTIFY_SOURCE)
    add_definitions(-D_FORTIFY_SOURCE=2)
  endif (WITH_FORTIFY_SOURCE)

endif (UNIX AND NOT WIN32)
