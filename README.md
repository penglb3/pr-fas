# PR-FAS
PageRank FAS

## Requirements
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
We tested and ran the programs under these environments:

**#1**
- OS: Ubuntu 22.04
- Compiler: GCC 11.3.0
- CMake: 3.22.1
