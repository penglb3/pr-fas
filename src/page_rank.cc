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

using std::min;
using std::pair;
using std::stack;
using std::vector;

// Recursive function to find the SCC using DFS traversal
void SccSolver::scc_util(int u) {
  static int time = 0;

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
auto SccSolver::operator()() -> vector<SCC> {
  int v = mat.size();

  disc = new int[v];
  low = new int[v];
  stack_member = new bool[v];

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

  delete stack_member;
  delete low;
  delete disc;

  return scc;
}

FAS page_rank_fas(const SparseMatrix &mat) {
  // FAS = []
  // Extract SCCs from mat
  // While SCCs is not empty:
  //   for scc, v_index in SCCs:
  //     e_graph, edges = line_graph(scc)
  //     rank = page_rank(scc)
  //     fa_index = argmax(rank)
  //     fa_scc = edges[fa_index]
  //     fa = {v_index[fa_scc.from], v_index[fa_scc.to]}
  //     FAS.append(fa)
  //     mat.remove(fa)
  //   Extract SCCs from mat
  // return FAS;
  return {};
}
