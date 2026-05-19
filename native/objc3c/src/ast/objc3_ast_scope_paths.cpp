#include "ast/objc3_ast_scope_paths.h"

#include <utility>

#include "ast/objc3_ast_ordering.h"

std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                     std::string entry_symbol) {
  std::vector<std::string> path;
  if (!owner_symbol.empty()) {
    path.push_back(std::move(owner_symbol));
  }
  if (!entry_symbol.empty()) {
    path.push_back(std::move(entry_symbol));
  }
  return Objc3AstSortedUniqueStrings(std::move(path));
}

std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                         const std::string &name,
                                         bool has_category,
                                         const std::string &category_name) {
  std::string owner = container_kind + ":" + name;
  if (has_category) {
    owner += "(" + category_name + ")";
  }
  return owner;
}
