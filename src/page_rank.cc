#include "page_rank.h"

#include "common.h"
#include <algorithm>
#include <cstdint>
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

inline int find_or_add_edge_index(unordered_map<uint64_t, int> &index,
                                  vector<Edge> &table, int from, int to) {
  uint64_t code = encode_edge(from, to);
  auto it = index.find(code);
  if (it == index.end()) {
    table[index.size()] = Edge(from, to);
    it = index.emplace(code, index.size()).first;
  }
  return it->second;
}

auto line_graph(const SparseMatrix &G) -> pair<SparseMatrix, vector<Edge>> {
  int n_edges = 0;
  for (const auto &row : G) {
    n_edges += row.size();
  }
  SparseMatrix res(n_edges);
  unordered_map<uint64_t, int> edge_index;
  vector<Edge> edge_table(n_edges);
  // DO NOT add 0 to visited!
  for (int begin = 0; begin < G.size(); begin++) {
    for (const auto &p : G[begin]) {
      int mid = p.first;
      int e_in = find_or_add_edge_index(edge_index, edge_table, begin, mid);
      for (const auto &kv : G[mid]) {
        int end = kv.first;
        int e_out = find_or_add_edge_index(edge_index, edge_table, mid, end);
        res[e_in][e_out] = 1;
        // printf("#(%d,%d)->#(%d,%d)\n", begin, mid, mid, end);
      }
    }
  }
  return {res, edge_table};
}

using std::min;
using std::stack;
using std::vector;

// Recursive function to find the SCC using DFS traversal
void SCC_Solver::scc_util(int u) {
  // Initialize discovery time and low value
  disc[u] = low[u] = ++time;
  st.push(u);
  stack_member[u] = true;

  // Go through all vertices adjacent to this
  for (auto i = mat[u].begin(); i != mat[u].end(); ++i) {
    // v is current adjacent of 'u'
    int v = i->first;

    // If v is not visited yet, then recur for it
    if (disc[v] == NIL) {
      scc_util(v);

      // Check if the subtree rooted with 'v' has connection to one of the
      // ancestors of 'u'
      low[u] = min(low[u], low[v]);
    }

    // Update low value of 'u' only of 'v' is still in stack
    else if (stack_member[v]) {
      low[u] = min(low[u], disc[v]);
    }
  }

  // head node found, pop the stack and print an SCC

  // Store stack extracted vertices
  int w = 0;

  // If low[u] and disc[u]
  if (low[u] == disc[u]) {
    vector<int> vertex_id;
    std::unordered_map<int, int> reverse_id;
    // When st top != u, the component has >1 vertices.
    while (st.top() != u) {
      w = st.top();
      reverse_id[w] = vertex_id.size();
      vertex_id.push_back(w);
      // std::cout << w << " ";
      stack_member[w] = false;
      st.pop();
    }
    w = st.top();
    if (!vertex_id.empty()) {
      reverse_id[w] = vertex_id.size();
      vertex_id.push_back(w);
      SparseMatrix scc_mat(vertex_id.size());
      for (int i = 0; i < vertex_id.size(); i++) {
        for (auto kv : mat[vertex_id[i]]) {
          if (reverse_id.find(kv.first) != reverse_id.end()) {
            scc_mat[i].emplace(reverse_id[kv.first], 1);
          }
        }
      }
      scc.emplace_back(scc_mat, vertex_id);
    }
    // std::cout << w << "\n";
    stack_member[w] = false;
    st.pop();
  }
}

// Function to find the SCC in the graph
auto SCC_Solver::operator()() -> vector<SCC> {
  int v = mat.size();
  time = 0;
  // Clear last calculation's result
  if (!scc.empty()) {
    scc.clear();
  }

  // Initialize disc and low, and stackMember arrays
  for (int i = 0; i < v; i++) {
    disc[i] = NIL;
    low[i] = NIL;
    stack_member[i] = false;
  }

  // Recursive helper function to find the SCC in DFS tree with vertex 'i'
  for (int i = 0; i < v; i++) {
    // If current node is not yet visited
    if (disc[i] == NIL) {
      scc_util(i);
    }
  }

  // Shouldn't happen, but just in case.
  while (!st.empty()) {
    st.pop();
  }

  return scc;
}

}; // namespace prfas

inline int argmax(const prfas::RankVec &rank) {
  int index = 0;
  float max_r = 0;
  for (int i = 0; i < rank.size(); i++) {
    if (rank[i] > max_r) {
      index = i;
      max_r = rank[i];
    }
  }
  return index;
};

using std::vector;
FAS page_rank_fas(const SparseMatrix &original_mat) {
  // FAS = []
  FAS result;
  // Extract SCCs from mat
  SparseMatrix mat(original_mat);
  auto solver = prfas::SCC_Solver(mat);
  auto SCCs = solver();
  // While SCCs is not empty:
  while (!SCCs.empty()) {
    //   for scc, v_index in SCCs:
    for (const prfas::SCC &scc : SCCs) {
      SparseMatrix scc_m = scc.first;
      vector<int> v_index = scc.second;
      //     e_graph, edges = line_graph(scc)
      auto lg = prfas::line_graph(scc_m);
      SparseMatrix e_graph = lg.first;
      vector<Edge> edges = lg.second;
      //     rank = page_rank(scc)
      auto rank = prfas::page_rank(e_graph);
      //     fa_index = argmax(rank)
      int fa_index = argmax(rank);
      //     fa_scc = edges[fa_index]
      Edge fa_scc = edges[fa_index];
      //     fa = {v_index[fa_scc.from], v_index[fa_scc.to]}
      Edge fa = {v_index[fa_scc.first], v_index[fa_scc.second]};
      //     FAS.append(fa)
      result.push_back(fa);
      //     mat.remove(fa)
      remove_edge(mat, fa);
    }
    //   Extract SCCs from mat
    SCCs = solver();
  }
  // return FAS;
  return result;
}
