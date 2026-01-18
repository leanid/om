string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" compiler_name)
# Извлекаем мажорную версию (первое число до точки)
# CMAKE_CXX_COMPILER_VERSION содержит "13.2.0" -> станет "13"
string(REGEX REPLACE "^([0-9]+).*$" "\\1" compiler_major_version "${CMAKE_CXX_COMPILER_VERSION}")

set(compiler_full_id "${compiler_name}${compiler_major_version}")

message(STATUS "OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "compiler: ${compiler_full_id}")
message(STATUS "architecture: ${CMAKE_SYSTEM_PROCESSOR}")
set(triplet_name "${CMAKE_SYSTEM_NAME}-${compiler_full_id}-${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER "${triplet_name}" triplet_name)
message(STATUS "triplet OS-compiler-architecture: ${triplet_name}")

set(OM_TRIPLET_NAME "${triplet_name}" CACHE STRING "triplet name with precompiled cmake packages")

if(NOT OM_TRIPLET_NAME)
    message(FATAL_ERROR "can't find triplet name")
endif()
