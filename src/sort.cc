#include "common.h"
#include <cstdio>
#include <list>
#include <numeric>
#include <unordered_set>

void print_ans(const FAS &fas) {
  for (const auto &[from, to] : fas) {
    std::printf("<%d, %d>\n", from, to);
  }
}

namespace sfas {};

FAS sort_fas(const SparseMatrix &mat) {
  std::list<int> order(mat.size());
  std::iota(order.begin(), order.end(), 0);
  auto curr = order.begin();
  auto next = std::next(curr);
  for (int i = 0; i < mat.size(); ++i) {
    int v = *curr;
    int val = 0;
    int min = 0;
    auto loc = curr;
    auto it = curr;
    for (int j = i - 1; j > 0; --j) {
      it = std::prev(it);
      int w = *it;
      if (const auto &edges = mat[v]; edges.find(w) != edges.end()) {
        val--;
      } else if (const auto &redges = mat[w]; redges.find(v) != redges.end()) {
        val++;
      }
      if (val <= min) {
        min = val;
        loc = it;
      }
    }
    // Insert at loc
    order.insert(loc, *curr);
    order.erase(curr);
    // Go next
    curr = next;
    next = std::next(curr);
  }

#if !defined(NDEBUG)
  for (const int i : order) {
    std::printf("%d ", i);
  }
  std::puts("");
#endif

  FAS ret;
  std::unordered_set<int> set;
  for (const int node : order) {
    for (const auto &[neighbor, _] : mat[node]) {
      if (set.find(neighbor) != set.end()) {
        ret.emplace_back(node, neighbor);
      }
    }
    set.insert(node);
  }
  return ret;
}