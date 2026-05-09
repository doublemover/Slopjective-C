#include "ast/objc3_ast_scope_semantic_links.h"

#include <utility>

#include "ast/objc3_ast_ordering.h"

std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names) {
  std::vector<std::string> targets;
  targets.reserve(protocol_names.size());
  for (const auto &name : protocol_names) {
    if (!name.empty()) {
      targets.push_back("protocol:" + name);
    }
  }
  return Objc3AstSortedUniqueStrings(std::move(targets));
}

std::string BuildObjcCategorySemanticLinkSymbol(
    const std::string &owner_name,
    const std::string &category_name) {
  return "category:" + owner_name + "(" + category_name + ")";
}
