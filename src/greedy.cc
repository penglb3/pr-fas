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
  for (auto &[_, neighbors] : g) {
    for (const auto &[target, _] : g[point]) {
      neighbors.erase(target);
    }
  }
  g.erase(point);
}
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
    s1.push_back(target);
    gfas::remove_node(graph, target);
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
  }
  return ret;
}