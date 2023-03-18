#include "page_rank.h"
#include <iostream>

int main() {
  constexpr int size = 5;
  SparseMatrix mat(size);
  /*
    [0,1,1,0,1],
    [1,0,0,0,1],
    [0,1,0,1,0],
    [1,1,0,0,0],
    [0,0,0,1,0]
  */
  mat[0] = {{1, 1}, {2, 1}, {4, 1}};
  mat[1] = {{0, 1}, {4, 1}};
  mat[2] = {{1, 1}, {3, 1}};
  mat[3] = {{0, 1}, {1, 1}};
  mat[4] = {{3, 1}};

  auto rank = prfas::page_rank(mat);
  for (float i : rank) {
    printf("%f ", i);
  }
  puts("");

  // Test line graph
  auto p = prfas::line_graph(mat);
  puts("");
  auto edges = p.second;
  auto e_graph = p.first;
  for (int i = 0; i < e_graph.size(); i++) {
    for (auto kv : e_graph[i]) {
      Edge src = edges[i];
      Edge to = edges[kv.first];
      printf("(%d,%d)->(%d,%d)\n", src.first, src.second, to.first, to.second);
    }
  }
  return 0;
}