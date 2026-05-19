#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace objc3c::support::selectors {

struct SelectorPieceSpelling {
  std::string_view keyword;
  bool has_parameter = false;
};

std::string BuildNormalizedSelectorSpelling(
    const std::vector<SelectorPieceSpelling> &pieces);

std::string BuildDefaultPropertySetterSelector(std::string_view property_name);

}  // namespace objc3c::support::selectors
