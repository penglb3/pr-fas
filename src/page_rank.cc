#include "common.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
using RankVec = std::vector<float>;

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

inline SparseMatrix line_graph(const SparseMatrix &G) {
  // Assert G is strongly connected.
  int n_edges = 0;
  for (const auto &row : G) {
    n_edges += row.size();
  }
  SparseMatrix lg(n_edges);
  std::queue<int> q;
  int out;
  q.push(0);
  while (!q.empty()) {
  }
  return lg;
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
  puts("\n");
  return 0;
}
