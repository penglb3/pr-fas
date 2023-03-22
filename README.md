# PR-FAS
PageRank FAS

## Requirements
- Linux System (Code compiles on Windows, but we can't guarantee its behavior)
- Compiler supporting `c++17` (trials on godbolt.org suggests GCC >= 7, clang >= 5, msvc >= 19.15)

## Build
`cmake -S . -B build` and then `cmake --build build`

(If you have `ninja-build` installed, you can do `cmake -S . -B build -G Ninja` for the first step to speed things up.)

## Run
`./build/test_bench [-s greedy|greedy_opt|sort|page_rank] [-i <input_file_path>]`

Parameters:
- `-s`: Specify solver. Optional. Default value = `page_rank`
- `-i`: Specify input dataset file path. Optional. Default = use standard small graph from TA's slides.

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