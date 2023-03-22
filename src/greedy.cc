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

FAS merge_s1s2(const SparseMatrix &mat, const std::vector<int> &s1,
               const std::list<int> &s2) {
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

namespace optimized {
struct greedy_t {
  using node_list_t = std::list<int>;
  explicit greedy_t(const SparseMatrix &mat)
      : mat_(mat), n_(mat.size()), node_refs_(n_), node_classes_(2 * n_ - 3),
        node_class_indices_(n_) {
    for (int i = 0; i < n_; ++i) {
      uint32_t d_in = ::get_in_degree(mat, i);
      uint32_t d_out = ::get_out_degree(mat, i);
      int ref_idx = get_ref_idx(n_, d_out, d_in);
      node_class_indices_[i] = ref_idx;
      node_classes_[ref_idx].push_front(i);
      node_refs_[i] = node_classes_[ref_idx].begin();
      nodes_.insert(i);
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

  int size() const { return nodes_.size(); }
  bool empty() const { return nodes_.empty(); }

  int get_node_class(int point) const {
    return nodes_.find(point) == nodes_.end() ? -1 : node_class_indices_[point];
  }

  int get_sink_node() const {
    auto sink_nodes = node_classes_[0];
    return sink_nodes.empty() ? -1 : *sink_nodes.begin();
  }

  int get_source_node() const {
    auto source_nodes = node_classes_[2 * n_ - 4];
    return source_nodes.empty() ? -1 : *source_nodes.begin();
  }

  int get_delta_node() const {
    if (nodes_.empty()) {
      return -1;
    }
    int ret;
    // Loop in the desacending order of delta = d_out - d_in
    for (int i = 2 * n_ - 5; i > 0; --i) {
      if (!node_classes_[i].empty()) {
        ret = *node_classes_[i].begin();
        break;
      }
    }
    return ret;
  }

  // O(n)
  void remove_node(int point) {
    // Delete out edges
    for (const auto &[neighbor, _] : mat_[point]) {
      // If there is an edge from pont to neighbor
      if (nodes_.find(neighbor) != nodes_.end()) {
        int delta = get_node_class(neighbor);
        node_classes_[delta].erase(node_refs_[neighbor]);
        node_classes_[delta + 1].push_front(neighbor);
        node_refs_[neighbor] = node_classes_[delta + 1].begin();
        node_class_indices_[neighbor] = delta + 1;
      }
    }
    // Delete in edges
    for (int i = 0; i < n_; ++i) {
      // Skip the point itself and nodes that have been removed
      if (i == point || (nodes_.find(i) == nodes_.end())) {
        continue;
      }
      // If there is an edge from i to point
      if (const auto &neighbors = mat_[i];
          neighbors.find(point) != neighbors.end()) {
        int delta = get_node_class(i);
        node_classes_[delta].erase(node_refs_[i]);
        node_classes_[delta - 1].push_front(i);
        node_refs_[i] = node_classes_[delta - 1].begin();
        node_class_indices_[i] = delta - 1;
      }
    }
    int d = get_node_class(point);
    node_classes_[d].erase(node_refs_[point]);
    nodes_.erase(point);
  }

  int n_;
  const SparseMatrix &mat_;
  // Store reference to each node in one of class lists
  std::vector<node_list_t::iterator> node_refs_;
  // delta = d_out - d_in for each node
  std::vector<int> node_class_indices_;
  // The node classes
  std::vector<node_list_t> node_classes_;
  std::unordered_set<int> nodes_;
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
      if (int tmp = d_out - d_in; tmp > d_max) {
        target = point;
        d_max = tmp;
      }
    }
    if (!graph.empty()) {
      s1.push_back(target);
      gfas::remove_node(graph, target);
    }
  }
  return gfas::merge_s1s2(mat, s1, s2);
}

FAS greedy_fas_optimized(const SparseMatrix &mat) {
  gfas::optimized::greedy_t greedy{mat};

  std::vector<int> s1;
  std::list<int> s2;
  // O(n^2)
  while (!greedy.empty()) {
    int target;
    while ((target = greedy.get_sink_node()) != -1) {
      s2.push_front(target);
      greedy.remove_node(target);
    }
    while ((target = greedy.get_source_node()) != -1) {
      s1.push_back(target);
      greedy.remove_node(target);
    }
    target = greedy.get_delta_node();
    if (target != -1) {
      s1.push_back(target);
      greedy.remove_node(target);
    }
  }
  return gfas::merge_s1s2(mat, s1, s2);
}