#pragma once

#include <string>
#include <vector>

std::vector<std::string> BuildProtocolSemanticLinkTargetsLexicographic(
    const std::vector<std::string> &protocol_names);
std::string BuildObjcCategorySemanticLinkSymbol(
    const std::string &owner_name,
    const std::string &category_name);
