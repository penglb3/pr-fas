#include "common.h"
#include <climits>
#include <cstdio>
#include <list>
#include <unordered_set>

namespace gfas {
using GreedyGraph = std::unordered_map<int, SparseVec>;

uint32_t get_out_degree(const GreedyGraph &g, int point) {
  int d = -1;
  if (auto it = g.find(point); it != g.end()) {
    d = it->second.size();
  }
  return d;
}

uint32_t get_in_degree(const GreedyGraph &g, int point) {
  uint32_t count = 0;
  for (const auto &[_, neighbors] : g) {
    if (neighbors.find(point) != neighbors.end()) {
      count++;
    }
  }
  return count;
}

using Predicate = std::function<uint32_t(const GreedyGraph &, int)>;
int get_target_node(const GreedyGraph &g, const Predicate &pred) {
  int target = -1;
  for (const auto &[point, neighbors] : g) {
    if (pred(g, point) == 0) {
      target = point;
      break;
    }
  }
  return target;
}

void remove_node(GreedyGraph &g, int point) {
  if (g.find(point) == g.end()) { // No such point in graph
    return;
  }
  // O(n^2)
  for (auto &[from, neighbors] : g) {
    if (from == point) {
      continue;
    }
    neighbors.erase(point);
  }
  g.erase(point);
}

namespace optimized {
struct NodeRefs {
  using node_list_t = std::list<int>;
  explicit NodeRefs(const SparseMatrix &mat)
      : n_(mat.size()), node_nexts_(n_), node_classes_(2 * n_ - 3, {-1}) {
    for (int i = 0; i < n_; ++i) {
      uint32_t d_in = ::get_in_degree(mat, i);
      uint32_t d_out = ::get_out_degree(mat, i);
      int ref_idx = get_ref_idx(n_, d_out, d_in);
      node_nexts_[i] = node_classes_[ref_idx].begin();
      node_classes_[ref_idx].push_front(i);
    }
  }

  static int get_ref_idx(int n, uint32_t d_out, uint32_t d_in) {
    if (d_out == 0) {
      return 0;
    }
    if (d_in == 0 && d_out > 0) {
      // There are (2n - 3) refs in total
      return 2 * n - 4;
    }
    int delta = d_out - d_in;
    return delta + n - 2;
  }

  int get_class_size(int class_idx) {
    return node_classes_[class_idx].size() - 1;
  }

  node_list_t::iterator get_next_of_node(int n) { return node_nexts_[n]; }

  int n_;
  std::vector<node_list_t::iterator> node_nexts_;
  std::vector<node_list_t> node_classes_;
};

}; // namespace optimized
}; // namespace gfas

FAS greedy_fas(const SparseMatrix &mat) {
  // Build graph from mat
  gfas::GreedyGraph graph;
  for (int i = 0; i < mat.size(); ++i) {
    graph[i] = mat[i];
  }
  std::vector<int> s1;
  std::list<int> s2;
  while (!graph.empty()) {
    int target;
    // Find sources
    while ((target = gfas::get_target_node(graph, gfas::get_in_degree)) != -1) {
      s1.push_back(target);
      gfas::remove_node(graph, target);
    }
    while ((target = gfas::get_target_node(graph, gfas::get_out_degree)) !=
           -1) {
      s2.push_front(target);
      gfas::remove_node(graph, target);
    }
    int d_max = INT_MIN;
    for (const auto &[point, _] : graph) {
      auto d_in = gfas::get_in_degree(graph, point);
      auto d_out = gfas::get_out_degree(graph, point);
      if (uint32_t tmp = d_out - d_in; static_cast<int>(tmp) > d_max) {
        target = point;
        d_max = tmp;
      }
    }
    if (!graph.empty()) {
      s1.push_back(target);
      gfas::remove_node(graph, target);
    }
  }
  // Loop s1:s2 to generate answer
  FAS ret;
  // Record visited nodes
  std::unordered_set<int> set;
  for (const int point : s1) {
    for (const auto &[neighbor, _] : mat[point]) {
      if (set.find(neighbor) != set.end()) {
        ret.emplace_back(point, neighbor);
      }
    }
    set.insert(point);
  }
  for (const int point : s2) {
    for (const auto &[neighbor, _] : mat[point]) {
      if (set.find(neighbor) != set.end()) {
        ret.emplace_back(point, neighbor);
      }
    }
    set.insert(point);
  }
  return ret;
}

FAS greedy_fas_optimized(const SparseMatrix &mat) {
  gfas::optimized::NodeRefs greedy(mat);
  // Check preprocess
  for (int i = 0; i < mat.size(); ++i) {
    std::printf("prev %d is: %d\n", i, *std::prev(greedy.get_next_of_node(i)));
  }
  for (int i = 0; i < 2 * mat.size() - 3; ++i) {
    std::printf("class %d has %d nodes\n", i, greedy.get_class_size(i));
    // Last element is -1
    for (const int n : greedy.node_classes_[i]) {
      std::printf("%d ", n);
    }
    std::puts("");
  }
  return {};
}