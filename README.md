# PR-FAS
PageRank FAS

## Requirements
- Linux System (Code compiles on Windows, but we can't guarantee its behavior)
- Compiler supporting `c++17` (trials on godbolt.org suggests GCC >= 7, clang >= 5, msvc >= 19.15)
- CMake >= 3.18

## Build
`cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo` and then `cmake --build build`

Note: `CMAKE_BUILD_TYPE` greatly affects the performance of program! For best performance, use `-DCMAKE_BUILD_TYPE=Release`!

(If you have `ninja-build` installed, you can add `-G Ninja` to the first step to speed things up.)

## Run
`./build/test_bench [-s <solver_name>] [-i <input_file_path>]`

FYI, `page_rank` takes ~4s to finish on `data/v300_e2731.txt` (which has 300 vertices and 2731 edges) while others take <0.2s. So please BE PATIENT when you use it.

Parameters:
- `-s`: Specify solver. Optional. Default value = `page_rank`. Available options are: `greedy`, `sort`, `page_rank`, `greedy_opt`(optimized greedy), `page_rank_lb`(PageRank using loop based line graph generation).
- `-i`: Specify input dataset file path. Optional. Default = use standard small graph from TA's slides.
- `-p`: Print out result FAS when specified.

## Test Environments
We were able to compile and run the programs under these environments:

**#1**
- OS: Ubuntu 22.04
- Compiler: GCC 11.3.0
- CMake: 3.22.1

**#2**
- OS: Ubuntu 18.04
- Compiler: GCC 8.4.0
- CMake: 3.20.0

**#3** (KNOWN ISSUES: `ctest` won't run and time in `test_bench` can be inaccurate!)
- OS: Windows 10 21H2
- Compiler: MSVC 19.28
- CMake: 3.20.3