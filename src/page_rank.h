#pragma once
#include "common.h"
using RankVec = std::vector<float>;

namespace prfas {
using std::pair;
using std::unordered_map;
using std::vector;

RankVec page_rank(const SparseMatrix &mat, double beta = 0.85,
                  int max_iter = 30, double stop_error = 1e-6);

auto line_graph(const SparseMatrix &G) -> pair<SparseMatrix, vector<Edge>>;

} // namespace prfas