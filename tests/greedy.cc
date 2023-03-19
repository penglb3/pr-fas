#include "common.h"
#include <cstdio>

int main() {
  SparseMatrix mat0(4);
  add_edge(mat0, 0, 1);
  add_edge(mat0, 1, 2);
  add_edge(mat0, 2, 3);
  add_edge(mat0, 3, 0);
  add_edge(mat0, 3, 1);
  FAS fas = greedy_fas(mat0);
  // print_ans(fas);
  fas = greedy_fas_optimized(mat0);
  std::puts("");

  SparseMatrix mat1(4);
  add_edge(mat1, 0, 1);
  add_edge(mat1, 1, 0);
  add_edge(mat1, 1, 2);
  add_edge(mat1, 2, 3);
  add_edge(mat1, 3, 0);
  add_edge(mat1, 3, 1);
  fas = greedy_fas(mat1);
  // print_ans(fas);
  fas = greedy_fas_optimized(mat1);
  std::puts("");

  SparseMatrix mat_std(7);
  // Use standard example from TA's PPT.
  add_edge(mat_std, 0, 1);
  add_edge(mat_std, 1, 2);
  add_edge(mat_std, 2, 3);
  add_edge(mat_std, 3, 0);
  add_edge(mat_std, 3, 1);

  add_edge(mat_std, 4, 5);
  add_edge(mat_std, 5, 6);
  add_edge(mat_std, 6, 4);

  FAS fas_std = greedy_fas(mat_std);
  // print_ans(fas_std);
  fas = greedy_fas_optimized(mat_std);
  return 0;
}