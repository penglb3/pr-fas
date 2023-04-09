# PR-FAS
Implements PageRank FAS ([Computing a Feedback Arc Set Using PageRank](https://doi.org/10.1007/978-3-031-22203-0_1))

## Requirements
- Linux System (Code compiles on Windows, but we can't guarantee its behavior)
- Compiler supporting `c++17` (trials on godbolt.org suggests GCC >= 7, clang >= 5, msvc >= 19.15)
- CMake >= 3.18

## Build
`cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo` and then `cmake --build build`

Note: `CMAKE_BUILD_TYPE` greatly affects the performance of program! So make sure it is set to release mode instead of pure debug or none.

(If you have `ninja-build` installed, you can add `-G Ninja` to the first step to speed things up.)

## Run
WARNING: We have **MODIFIED DATA FILE** from TA (added num of vertices at the beginning), so **PLEASE USE DATA IN `./data`** instead of your own!

Usage:

`./build/test_bench [-s <solver_name>] [-i <input_file_path>] [-p]`

Parameters:
- `-s`: Specify solver. Optional. Default value = `page_rank`. Available options are: `greedy`, `sort`, `page_rank`, `greedy_opt`(optimized greedy), `page_rank_lb`(PageRank using loop based line graph generation). See Results below for how much time each solver would take.
- `-i`: Specify input dataset file path. Optional. Default = use standard small graph from TA's slides.
- `-p`: Print out result FAS when specified.

## Results
These results are obtained in test env #1, one solver per thread, multi-threaded. Therefore you may achieve much better time when you run a solver alone. **Peak memory usage** on WA-2011 and enron are **~100MB** and **~980MB** respectively, both achieved by PageRank.

|     **Dataset**    	|    **WA-2011**  	|                	|     **enron**     |                	|
|:----------------------:	|:-------------:	|:--------------:	|:-------------:	|:--------------:	|
|  **FAS solver**  	| **Time** 	| **FAS\%** 	| **Time** 	| **FAS\%** 	|
|  **PageRank(LB)** 	|     21:07     	|      14.68     	|    12:07:09   	|      11.02     	|
| **PageRank(DFS)** 	|     22:18     	|      14.68     	|    12:32:02   	|      11.02     	|
|  **Greedy(Opt)**  	|     00:06     	|      18.89     	|    00:04:01   	|      12.58     	|
|  **Greedy(Naive)**    |     45:33     	|      18.23     	|    101:04:18     	|      11.91       	|
|      **Sort**     	|     00:02     	|      20.17     	|    00:00:57   	|      14.16     	|

(By the time I wrote this down, Greedy Naive on enron has taken 37hrs and not yet finished. I will update the form once it's done.)

## Test Environments
We are able to compile and run the programs under these environments:

**#1**
- CPU: Intel Xeon Gold 6230N (20c40t @2.3~3.5GHz)
- DRAM: 192GB DDR4 2400MHz
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