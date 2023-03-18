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
  FasSolver *solver_ptr;
  InputParser parser(argc, argv);
  SparseMatrix mat;

  // Do some initialization for mat;

  if (parser.option_exists("-s")) {
    string solver_name = parser.get_option("-s");
    if (solver_name == "greedy") {
      solver_ptr = new GreedyFas(mat);
    } else if (solver_name == "sort") {
      solver_ptr = new SortFas(mat);
    } else if (solver_name == "page_rank") {
      solver_ptr = new PageRankFas(mat);
    }
  }
  solver_ptr->solve();

  // Do some validation and test.

  delete solver_ptr;
  return 0;
}