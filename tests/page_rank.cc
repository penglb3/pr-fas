#include "page_rank.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
  constexpr int size = 5;
  SparseMatrix mat(size);

  mat[0] = {{1, 1}, {2, 1}, {4, 1}};
  mat[1] = {{0, 1}, {4, 1}};
  mat[2] = {{1, 1}, {3, 1}};
  mat[3] = {{0, 1}, {1, 1}};
  mat[4] = {{3, 1}};

  const float expected[] = {0.232071, 0.237557, 0.095753, 0.237903, 0.196715};

  auto rank = prfas::page_rank(mat, 0.85, 30);
  for (int i = 0; i < rank.size(); i++) {
    assert(std::fabs(rank[i] - expected[i]) < 1e-5);
    // printf("%f ", i);
  }
  puts("PageRank test success.");

  // Test line graph
  auto p = prfas::line_graph(mat);
  auto edges = p.second;
  auto e_graph = p.first;
  for (int i = 0; i < e_graph.size(); i++) {
    for (auto kv : e_graph[i]) {
      Edge src = edges[i];
      Edge to = edges[kv.first];
      assert(mat[src.first][src.second] == 1); // src edge exists
      assert(mat[to.first][to.second] == 1);   // to edge exists
      assert(src.second == to.first);          // src --> V --> to
      // printf("(%d,%d)->(%d,%d)\n", src.first, src.second, to.first,
      // to.second);
    }
  }
  puts("Line Graph test success.");

  SparseMatrix scc_test(7);

  // Use standard example from TA's PPT.
  add_edge(scc_test, 0, 1);
  add_edge(scc_test, 1, 2);
  add_edge(scc_test, 2, 3);
  add_edge(scc_test, 3, 0);
  add_edge(scc_test, 3, 1);

  add_edge(scc_test, 4, 5);
  add_edge(scc_test, 5, 6);
  add_edge(scc_test, 6, 4);

  // Function Call to find SCC using
  // Tarjan's Algorithm
  SccSolver g1(scc_test);
  auto sccs = g1();
  for (const SCC &p : sccs) {
    auto m = p.first;
    auto v = p.second;
    for (int i = 0; i < v.size(); i++) {
      for (auto kv : m[i]) {
        int j = kv.first;
        // See if we can recover each edge correctly.
        assert(scc_test[v[i]][v[j]] == 1);
        // printf("scc[%d, %d]=>mat[%d, %d]=%d\n", i, j, v[i], v[j],
        //        scc_test[v[i]][v[j]]);
      }
    }
    // printf("\n");
  }
  puts("SCC extraction test success.");
  return 0;
}