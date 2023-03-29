#pragma once
#include "common.h"

namespace prfas {
using RankVec = std::vector<float>;
using std::pair;
using std::unordered_map;
using std::vector;

inline uint64_t encode_edge(int from, int to) {
  return (static_cast<uint64_t>(from) << 32) + to;
}

inline uint64_t encode_edge(Edge e) { return encode_edge(e.first, e.second); }

inline Edge decode_edge(uint64_t edge_code) {
  return {edge_code >> 32, edge_code & UINT32_MAX};
}

// Page Rank computation function
// @param mat : The graph matrix consisting of only 0 and 1
// @param beta : Damping factor
// @param max_iter : Maximum iteration numbers
// @param stop_error : Error threshold, not yet implemented.
// @return : the result rank vector.
RankVec page_rank(const SparseMatrix &mat, float beta = 1, int max_iter = 30,
                  float stop_error = 1e-5);

// Calculates the line graph in 1 pass via DFS or for loop.
// @param G : the graph to compute line graph on, need to be strongly connected
// @return : The result line graph and the edge index to recover edge info
auto line_graph(const SparseMatrix &G) -> pair<SparseMatrix, vector<Edge>>;

// Feedback arcs only exist in a strongly connected directed graph.
// So extracting strongly connected components not only narrows searching range
// but also detects cycle! Another problem solved!W

using SCC = std::pair<SparseMatrix, std::vector<int>>;

// SCC solver: extracts all SCCs with >1 vertices using Tarjan's Algorithm
class SCC_Solver {
  static constexpr int NIL = -1;
  const SparseMatrix &mat;
  std::stack<int> st;
  // Current node's discovery time
  int time;
  // Stores the discovery times of the nodes
  std::vector<int> disc;
  // Stores the nodes with least discovery time
  std::vector<int> low;
  // Checks whether a node is in the stack or not
  std::vector<bool> stack_member;

  // A Recursive DFS based function used by SCC
  void scc_util(int u);

public:
  // The result SCC
  std::vector<SCC> result_scc;

  explicit SCC_Solver(const SparseMatrix &mat)
      : mat(mat), disc(mat.size()), low(mat.size()), stack_member(mat.size()){};
  ~SCC_Solver() = default;
  const std::vector<SCC> &operator()();
};

// Implements the DFS line graph generation in original paper.
class LineGraphGeneator {
  const SparseMatrix &mat;
  SparseMatrix line_graph;
  unordered_map<uint64_t, int> edge_index;
  vector<Edge> edge_table;
  vector<bool> visited;

public:
  explicit LineGraphGeneator(const SparseMatrix &mat, const int n_edges)
      : mat(mat), visited(mat.size(), false), line_graph(n_edges),
        edge_table(n_edges){};
  ~LineGraphGeneator() = default;
  void dfs_util(int curr, int prev);
  pair<SparseMatrix, vector<Edge>> operator()() {
    dfs_util(0, -1);
    return {line_graph, edge_table};
  }
};
} // namespace prfas