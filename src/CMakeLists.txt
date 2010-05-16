set(SMARTALL_SHARED_LIBRARY
  smartall_c_shared
  CACHE INTERNAL ""
)

set(smartall_SRCS
  smartall.c
  alloc.c
)

add_library(${SMARTALL_SHARED_LIBRARY} SHARED ${smartall_SRCS})

set_target_properties(
  ${SMARTALL_SHARED_LIBRARY}
    PROPERTIES
      VERSION
        ${LIBRARY_VERSION}
      SOVERSION
        ${LIBRARY_SOVERSION}
      OUTPUT_NAME
        smartall
)

set_target_properties(
    ${SMARTALL_SHARED_LIBRARY}
      PROPERTIES CLEAN_DIRECT_OUTPUT 1
)