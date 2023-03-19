# PR-FAS
PageRank FAS

## Requirements
- Compiler supporting `c++17` (trials on godbolt.org suggests GCC >= 7, clang >= 5, msvc >= 19.15)

## Build
`cmake -S . -B build` and then `cmake --build build`

(If you have `ninja-build` installed, you can do `cmake -S . -B build -G Ninja` for the first step to speed things up.)

## Test Environments
We tested and ran the programs under these environments:

**#1**
- OS: Ubuntu 22.04
- Compiler: GCC 11.3.0
- CMake: 3.22.1

## Run
`./build/test_bench -s [greedy|sort|page_rank]`