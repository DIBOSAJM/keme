set(A_DIR_MODULE_ROOT ${CMAKE_CURRENT_LIST_DIR})

include_directories(SYSTEM ${A_DIR_MODULE_ROOT})

set(A_SOURCES_HEADERS
	${A_SOURCES_HEADERS}
    ${A_DIR_MODULE_ROOT}/CommonClasses.h
)

set(A_SOURCES_CPP
	${A_SOURCES_CPP}
    ${A_DIR_MODULE_ROOT}/CommonClasses.cpp
)