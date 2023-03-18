#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>

using SparseVec = std::unordered_map<int, char>;
using SparseMatrix = std::vector<SparseVec>;
using Edge = std::pair<int, int>;

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

// Feedback arcs only exist in a strongly connected directed graph.
// So extracting strongly connected components not only narrows searching range
// but also detects cycle! Another problem solved! ... I guess so?
// TODO(implement).
auto strongly_connected_components(const SparseMatrix &mat)
    -> std::pair<std::vector<SparseMatrix>, std::vector<int>>;

inline uint64_t encode_edge(int from, int to) {
  return (static_cast<uint64_t>(from) << 32) + to;
}

inline uint64_t encode_edge(Edge e) { return encode_edge(e.first, e.second); }

inline Edge decode_edge(uint64_t edge_code) {
  return {edge_code >> 32, edge_code & UINT32_MAX};
}

using FAS = std::vector<Edge>;
using fas_solver = std::function<FAS(const SparseMatrix &)>;
FAS sort_fas(const SparseMatrix &mat);
FAS greedy_fas(const SparseMatrix &mat);
FAS page_rank_fas(const SparseMatrix &mat);