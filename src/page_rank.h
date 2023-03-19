#pragma once
#include "common.h"

namespace prfas {
using RankVec = std::vector<float>;
using std::pair;
using std::unordered_map;
using std::vector;

// Page Rank computation function
// @param mat : The graph matrix consisting of only 0 and 1
// @param beta : Damping factor
// @param max_iter : Maximum iteration numbers
// @param stop_error : Error threshold, not yet implemented.
// @return : the result rank vector.
RankVec page_rank(const SparseMatrix &mat, double beta = 0.85,
                  int max_iter = 30, double stop_error = 1e-6);

// Calculates the line graph in 1 pass via BFS.
// @param G : the graph to compute line graph on, need to be strongly connected
// @return : The result line graph and the edge index to recover edge info
auto line_graph(const SparseMatrix &G) -> pair<SparseMatrix, vector<Edge>>;

} // namespace prfas