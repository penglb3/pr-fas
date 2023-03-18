# PR-FAS
PageRank FAS

## Build
`cmake -S . -B build` and then `cmake --build build`

(If you have `ninja-build` installed, you can do `cmake -S . -B build -G Ninja` for the first step to speed things up.)

## Run
`./build/test_bench -s [greedy|sort|page_rank]`