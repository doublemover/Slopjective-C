#include "support/selectors/selector_piece_normalization.h"

#include <cctype>

namespace objc3c::support::selectors {

std::string BuildNormalizedSelectorSpelling(
    const std::vector<SelectorPieceSpelling> &pieces) {
  std::size_t size = 0;
  for (const SelectorPieceSpelling &piece : pieces) {
    size += piece.keyword.size();
    if (piece.has_parameter) {
      ++size;
    }
  }

  std::string normalized;
  normalized.reserve(size);
  for (const SelectorPieceSpelling &piece : pieces) {
    normalized.append(piece.keyword.data(), piece.keyword.size());
    if (piece.has_parameter) {
      normalized.push_back(':');
    }
  }
  return normalized;
}

std::string BuildDefaultPropertySetterSelector(std::string_view property_name) {
  if (property_name.empty()) {
    return {};
  }

  std::string selector;
  selector.reserve(property_name.size() + 4u);
  selector.append("set");
  selector.push_back(static_cast<char>(
      std::toupper(static_cast<unsigned char>(property_name.front()))));
  selector.append(property_name.substr(1).data(), property_name.size() - 1u);
  selector.push_back(':');
  return selector;
}

}  // namespace objc3c::support::selectors
