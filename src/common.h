#include <cstdint>
#include <map>
#include <unordered_map>
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
    -> std::vector<std::pair<int, SparseMatrix>>;