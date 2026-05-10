#include "pipeline/runtime_import_link_plan.h"

#include <sstream>
#include <utility>

namespace objc3c::pipeline {

std::vector<std::string> SplitRuntimeImportLinkerResponseFlags(
    const std::string &text) {
  std::vector<std::string> flags;
  std::istringstream input(text);
  for (std::string line; std::getline(input, line);) {
    if (!line.empty()) {
      flags.push_back(std::move(line));
    }
  }
  return flags;
}

}  // namespace objc3c::pipeline
