#pragma once

#include <string>
#include <vector>

std::vector<std::string> BuildScopePathLexicographic(std::string owner_symbol,
                                                     std::string entry_symbol);
std::string BuildObjcContainerScopeOwner(const std::string &container_kind,
                                         const std::string &name,
                                         bool has_category,
                                         const std::string &category_name);
