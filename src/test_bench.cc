#include "common.h"
#include <algorithm>
#include <string>
#include <unistd.h>

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

// test_bench.cc
int main(int argc, const char *argv[]) {
  fas_solver solver_function;
  InputParser parser(argc, argv);
  constexpr int size = 5;
  SparseMatrix mat(size);

  // Do some initialization for mat;
  mat[0] = {{1, 1}, {2, 1}, {4, 1}};
  mat[1] = {{0, 1}, {4, 1}};
  mat[2] = {{1, 1}, {3, 1}};
  mat[3] = {{0, 1}, {1, 1}};
  mat[4] = {{3, 1}};

  if (parser.option_exists("-s")) {
    string solver_name = parser.get_option("-s");
    if (solver_name == "greedy") {
      solver_function = greedy_fas;
    } else if (solver_name == "sort") {
      solver_function = sort_fas;
    } else if (solver_name == "page_rank") {
      solver_function = page_rank_fas;
    } else {
      return -1;
    }
  } else {
    std::puts("test_bench should be called via -s solver option.");
    return 0;
  }
  FAS result = solver_function(mat);
  // Do some validation and test.

  return 0;
}