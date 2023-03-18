#include "common.h"
#include <cstdio>

void print_ans(const FAS &fas) {
  for (const auto &[from, to] : fas) {
    std::printf("<%d, %d>\n", from, to);
  }
}

int main() {
  SparseMatrix mat0(4);
  add_edge(mat0, 0, 1);
  add_edge(mat0, 2, 2);
  add_edge(mat0, 2, 3);
  add_edge(mat0, 3, 0);
  add_edge(mat0, 3, 1);
  FAS fas = greedy_fas(mat0);
  print_ans(fas);

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
  print_ans(fas_std);
  return 0;
}