cmake_minimum_required(VERSION 3.18)
project(pagerank-fas VERSION 0.1)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile -Wno-address-of-packed-member")
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
  src/page_rank.h
)

set(PRFAS_SOURCES
  src/page_rank.cc
  src/sort.cc
  src/greedy.cc
)

# add_executable(page_rank src/page_rank.cc ${BACKWARD_ENABLE})
add_library(fas SHARED ${PRFAS_SOURCES})
add_executable(test_bench src/test_bench.cc)
target_link_libraries(test_bench PRIVATE fas)

include_directories(src)
add_executable(page_rank.test tests/page_rank.cc src/page_rank.cc)
add_executable(sort.test tests/sort.cc src/sort.cc)
add_executable(greedy.test tests/greedy.cc src/greedy.cc)

add_test(NAME TestBench COMMAND test_bench)
add_test(NAME PageRankTest COMMAND page_rank.test)
add_test(NAME GreedyTest COMMAND greedy.test)
add_test(NAME SortTest COMMAND sort.test)

clangformat_setup(
  ${PRFAS_SOURCES}
  ${PRFAS_HEADERS}
)