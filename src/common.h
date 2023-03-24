#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

using SparseVec = std::unordered_map<int, char>;
using SparseMatrix = std::vector<SparseVec>;
using Edge = std::pair<int, int>;

inline bool add_edge(SparseMatrix &mat, int from, int to) {
  return mat[from].emplace(to, 1).second;
}

inline bool remove_edge(SparseMatrix &mat, const Edge &e) {
  return mat[e.first].erase(e.second) == 1;
}

// O(1)
inline uint32_t get_out_degree(const SparseMatrix &mat, int point) {
  return mat[point].size();
}

// O(n)
inline uint32_t get_in_degree(const SparseMatrix &mat, int point) {
  uint32_t count = 0;
  for (const auto &row : mat) {
    // For an unordered_map, find() is O(1) on avg.
    if (row.find(point) != row.end()) {
      count++;
    }
  }
  return count;
}

extern bool loop_based_line_graph_gen;

using FAS = std::vector<Edge>;
using fas_solver = std::function<FAS(const SparseMatrix &)>;
FAS sort_fas(const SparseMatrix &mat);
FAS greedy_fas(const SparseMatrix &mat);
FAS greedy_fas_optimized(const SparseMatrix &mat);
FAS page_rank_fas(const SparseMatrix &mat);

void print_ans(const FAS &fas);
