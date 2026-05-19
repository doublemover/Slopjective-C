#include "sema/objc3_semantic_collection_helpers.h"

#include <algorithm>

bool IsSortedUniqueStrings(const std::vector<std::string> &values) {
  if (!std::is_sorted(values.begin(), values.end())) {
    return false;
  }
  return std::adjacent_find(values.begin(), values.end()) == values.end();
}

std::vector<std::string> BuildSortedUniqueStringsLocal(
    std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}
