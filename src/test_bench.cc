#include "common.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <string>
using std::string;

class InputParser {
public:
  InputParser(int argc, const char *argv[]) {
    for (int i = 1; i < argc; ++i) {
      this->tokens_.emplace_back(argv[i]);
    }
  }
  /// @author iain
  const std::string &get_option(const std::string &option) const {
    std::vector<std::string>::const_iterator itr;
    itr = std::find(this->tokens_.begin(), this->tokens_.end(), option);
    if (itr != this->tokens_.end() && ++itr != this->tokens_.end()) {
      return *itr;
    }
    static const std::string empty_string;
    return empty_string;
  }
  /// @author iain
  bool option_exists(const std::string &option) const {
    return std::find(this->tokens_.begin(), this->tokens_.end(), option) !=
           this->tokens_.end();
  }

private:
  std::vector<std::string> tokens_;
};

auto read_input(const string &filename) -> std::pair<SparseMatrix, int> {
  if (filename.empty()) { // Use standard example from TA's PPT.
    SparseMatrix mat(7);
    add_edge(mat, 0, 1);
    add_edge(mat, 1, 2);
    add_edge(mat, 2, 3);
    add_edge(mat, 3, 0);
    add_edge(mat, 3, 1);
    add_edge(mat, 4, 5);
    add_edge(mat, 5, 6);
    add_edge(mat, 6, 4);
    return {mat, 8};
  }
  FILE *file = fopen(filename.c_str(), "r");
  if (file == nullptr) {
    printf("File '%s' doesn't exist.\n", filename.c_str());
    return {};
  }
  int size, from, to, n_edges = 0;
  int n_scanned = fscanf(file, "%d", &size);
  if (n_scanned != 1) {
    printf("Can't read graph size\n");
    return {};
  }
  char sep[4];
  SparseMatrix mat(size);
  while (n_scanned != EOF) {
    n_scanned = fscanf(file, "%d%3[ ,]%d", &from, sep, &to);
    switch (n_scanned) {
    case 3:
      add_edge(mat, from, to);
      n_edges++;
      break;
    case EOF:
      continue;
    default:
      puts("Input pattern mismatch");
      return {};
    }
  }
  return {mat, n_edges};
}
std::unordered_map<std::string, fas_solver> func_mapping{
    {"sort", sort_fas},
    {"greedy", greedy_fas},
    {"greedy_opt", greedy_fas_optimized},
    {"page_rank", page_rank_fas},
    {"page_rank_lb", page_rank_fas}};

// test_bench.cc
int main(int argc, const char *argv[]) {
  InputParser parser(argc, argv);

  // Default = PageRank
  fas_solver solver_function = page_rank_fas;
  string solver_name = "page_rank";
  if (parser.option_exists("-s")) {
    solver_name = parser.get_option("-s");
    if (auto it = func_mapping.find(solver_name); it != func_mapping.end()) {
      solver_function = func_mapping[solver_name];
    } else {
      return -1;
    }
  }
  printf("Solver: %s\n", solver_name.c_str());

  if (solver_name == "page_rank_lb") {
    loop_based_line_graph_gen = true;
  }

  string input_file = parser.get_option("-i");
  const auto [mat, n_edges] = read_input(input_file);
  if (mat.empty() || n_edges == 0) {
    puts("Read input failed. Abort.");
    return -1;
  }
  printf("Testing graph has %lu vertices and %d edges\n", mat.size(), n_edges);

  printf("Solving start...");
  fflush(stdout);
  auto start = std::chrono::high_resolution_clock::now();
  FAS result = solver_function(mat);
  auto end = std::chrono::high_resolution_clock::now();
  puts("Done.");

  auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  printf("Time Elapsed: %.3f(ms). \n", time.count() * 1e-6);

  float fas_percentage = result.size() * static_cast<float>(100) / n_edges;
  printf("Result FAS size = %lu (%.2f%%)\n", result.size(), fas_percentage);
  // Do some validation and test.

  if (parser.option_exists("-p")) {
    puts("\nResult FAS:");
    print_ans(result);
  }

  return 0;
}