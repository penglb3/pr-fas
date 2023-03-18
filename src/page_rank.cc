#include "page_rank.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>

namespace prfas {

// PageRank iteration step: vec = vec * mat
inline RankVec operator*(const RankVec &vec, const SparseMatrix &mat) {
  RankVec res(vec.size(), 0);
  for (int i = 0; i < mat.size(); i++) {
    for (auto kv : mat[i]) {
      res[kv.first] += vec[i] * kv.second / get_out_degree(mat, i);
    }
  }
  return res;
}

template <class T>
inline RankVec operator+(const RankVec &vec, const T val) {
  static_assert(std::is_arithmetic<T>::value, "Class T has to be numeric");
  RankVec res(vec);
  for (float &elem : res) {
    elem += val;
  }
  return res;
}

template <class T>
inline RankVec operator+(const T val, const RankVec &vec) {
  return vec + val;
}

template <class T>
inline RankVec operator*(const T val, const RankVec &vec) {
  static_assert(std::is_arithmetic<T>::value, "Class T has to be numeric");
  RankVec res(vec);
  for (float &elem : res) {
    elem *= val;
  }
  return res;
}

template <class T>
inline RankVec operator*(const RankVec &vec, const T val) {
  return val * vec;
}

RankVec page_rank(const SparseMatrix &mat, const double beta,
                  const int max_iter, const double stop_error) {
  const auto size = mat.size();
  RankVec rank(size, static_cast<float>(1) / size);
  double error = std::numeric_limits<double>::infinity();
  for (int i = 0; i < max_iter || error < stop_error; i++) {
    rank = (beta * rank * mat) + (1 - beta) / size;
    // TODO(maybe add error computation)
  }
  return rank;
}

// Calculates the line graph in 1 pass via BFS.
// @param G : the graph to compute line graph on, need to be strongly
// connected
// @return : The result line graph and the edge index to recover edge info
auto line_graph(const SparseMatrix &G) -> pair<SparseMatrix, vector<Edge>> {
  int n_edges = 0;
  for (const auto &row : G) {
    n_edges += row.size();
  }
  SparseMatrix res(n_edges);
  std::unordered_set<int> visited;
  unordered_map<uint64_t, int> edge_index;
  vector<Edge> edge_table(n_edges);
  std::queue<Edge> q;
  q.push({-1, 0});
  // DO NOT add 0 to visited!
  while (!q.empty()) {
    Edge p = q.front();
    q.pop();
    int prev = p.first;
    int curr = p.second;
    uint64_t from_code = encode_edge(p);
    for (auto kv : G[curr]) {
      int next = kv.first;
      uint64_t to_code = encode_edge(curr, next);
      auto it = edge_index.find(to_code);
      if (it == edge_index.end()) {
        edge_table[edge_index.size()] = Edge(curr, next);
        it = edge_index.emplace(to_code, edge_index.size()).first;
      }
      if (prev >= 0) {
        res[edge_index[from_code]][it->second] = 1;
        // printf("#(%d,%d)->#(%d,%d)\n", prev, curr, curr, next);
      }
      if (visited.find(next) == visited.end()) {
        q.emplace(curr, next);
        visited.insert(next);
      }
    }
  }
  return {res, edge_table};
}
}; // namespace prfas

FAS page_rank_fas(const SparseMatrix &mat) { return {}; }
