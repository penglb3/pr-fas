cmake_minimum_required(VERSION 3.18)
project(pagerank-fas VERSION 0.1)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC") # MSVC doesnt recognize these flags.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile -Wno-address-of-packed-member")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ExternalProject)
include(ProcessorCount)
ProcessorCount(N)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ClangFormat)
include(GitVersion)
include(CTest)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  set(ROCKSDB_USE_CLANG USE_CLANG=1 CC=clang CXX=clang++ "EXTRA_CXXFLAGS=-stdlib=libc++ -Wno-unused-but-set-variable")
endif ()

include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
# valgrind has problem understanding certain native instructions, so don't use it in Debug.
set(USE_NATIVE_INST "Yes")
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  set(USE_NATIVE_INST "No")
endif()
if(COMPILER_SUPPORTS_MARCH_NATIVE AND USE_NATIVE_INST MATCHES "Yes")
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
  src/page_rank.h
)

set(PRFAS_SOURCES
  src/page_rank.cc
  src/sort.cc
  src/greedy.cc
)

include_directories(src)
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_library(fas ${PRFAS_SOURCES})
else()
  add_library(fas ${PRFAS_SOURCES})
endif()
link_libraries(fas)

add_executable(test_bench src/test_bench.cc)

add_executable(page_rank.test tests/page_rank.cc)
add_executable(sort.test tests/sort.cc)
add_executable(greedy.test tests/greedy.cc)

add_test(NAME TestBench COMMAND test_bench)
add_test(NAME PageRankTest COMMAND page_rank.test)
add_test(NAME GreedyTest COMMAND greedy.test)
add_test(NAME SortTest COMMAND sort.test)

# Clang format is not necessary, so don't let it cause fatal error.
find_program(clang_format_executable clang-format)
if (CLANGFORMAT_EXECUTABLE OR clang_format_executable)
  message("Clang format found")
  clangformat_setup(
    ${PRFAS_SOURCES}
    ${PRFAS_HEADERS}
  )
endif()