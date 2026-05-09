#include "ast/objc3_ast_ordering.h"

#include <algorithm>

std::vector<std::string> Objc3AstSortedUniqueStrings(
    std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}
