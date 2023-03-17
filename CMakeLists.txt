cmake_minimum_required(VERSION 3.18)
project(pagerank-fas VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile -Wno-address-of-packed-member")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ExternalProject)
include(ProcessorCount)
ProcessorCount(N)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ClangFormat)
include(GitVersion)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(ROCKSDB_USE_CLANG USE_CLANG=1 CC=clang CXX=clang++ "EXTRA_CXXFLAGS=-stdlib=libc++ -Wno-unused-but-set-variable")
endif ()

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

CHECK_CXX_COMPILER_FLAG("-flto" COMPILER_SUPPORTS_FLTO)
if(COMPILER_SUPPORTS_FLTO)
  if (CMAKE_BUILD_TYPE STREQUAL Release)    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wunused-but-set-variable")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -stdlib=libc++ -Wunused-but-set-variable")
endif ()

set(PRFAS_HEADERS 
  src/common.h
)

set(PRFAS_SOURCES
  src/page_rank.cc
  ${PRFAS_HEADERS}
)

add_executable(page_rank src/page_rank.cc ${BACKWARD_ENABLE})

clangformat_setup(
  ${PRFAS_SOURCES}
  ${PRFAS_HEADERS}
)