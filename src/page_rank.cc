#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include "common.h"
using RankVec = std::vector<float>;
using std::pair;
using std::unordered_map;
using std::vector;

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

RankVec page_rank(const SparseMatrix &mat, const double beta = 0.85,
                  const int max_iter = 30, const double stop_error = 1e-6) {
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
// @param G : the graph to compute line graph on, need to be strongly connected
// @return : The result line graph and the edge index to recover edge info
pair<SparseMatrix, unordered_map<int, Edge>> line_graph(const SparseMatrix &G) {
  int n_edges = 0;
  for (const auto &row : G) {
    n_edges += row.size();
  }
  SparseMatrix res(n_edges);
  std::unordered_set<int> visited;
  unordered_map<uint64_t, int> edge_index;
  unordered_map<int, Edge> reverse_index;
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
        reverse_index.emplace(edge_index.size(), Edge(curr, next));
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
  return {res, reverse_index};
}

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

  auto rank = page_rank(mat);
  for (float i : rank) {
    printf("%f ", i);
  }
  puts("");

  // Test line graph
  auto p = line_graph(mat);
  puts("");
  auto e_index = p.second;
  auto e_graph = p.first;
  for (int i = 0; i < e_graph.size(); i++) {
    for (auto kv : e_graph[i]) {
      Edge src = e_index[i];
      Edge to = e_index[kv.first];
      printf("(%d,%d)->(%d,%d)\n", src.first, src.second, to.first, to.second);
    }
  }
  return 0;
}
