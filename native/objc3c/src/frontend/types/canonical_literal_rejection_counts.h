#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

enum class Objc3CanonicalLiteralRejectionKind : std::uint8_t {
  Yes = 0,
  No = 1,
  Null = 2,
};

struct Objc3CanonicalLiteralRejectionCounts {
  std::size_t yes_literal_sites = 0;
  std::size_t no_literal_sites = 0;
  std::size_t null_literal_sites = 0;

  std::size_t total_literal_sites() const {
    return yes_literal_sites + no_literal_sites + null_literal_sites;
  }
};

inline bool AreObjc3CanonicalLiteralRejectionCountsAdditive(
    const Objc3CanonicalLiteralRejectionCounts &counts) {
  const std::size_t yes_no_total =
      counts.yes_literal_sites + counts.no_literal_sites;
  if (yes_no_total < counts.yes_literal_sites) {
    return false;
  }
  const std::size_t total = yes_no_total + counts.null_literal_sites;
  return total >= yes_no_total && total == counts.total_literal_sites();
}

inline bool AreObjc3CanonicalLiteralRejectionsCleared(
    const Objc3CanonicalLiteralRejectionCounts &counts) {
  return AreObjc3CanonicalLiteralRejectionCountsAdditive(counts) &&
         counts.total_literal_sites() == 0u;
}

inline bool DoObjc3CanonicalLiteralRejectionsFitTokenBudget(
    const Objc3CanonicalLiteralRejectionCounts &counts,
    std::size_t token_count) {
  return AreObjc3CanonicalLiteralRejectionCountsAdditive(counts) &&
         counts.total_literal_sites() <= token_count;
}

inline std::string BuildObjc3CanonicalLiteralRejectionReplayKey(
    const Objc3CanonicalLiteralRejectionCounts &counts,
    std::size_t token_count) {
  return "canonical-literal-rejections:yes=" +
         std::to_string(counts.yes_literal_sites) +
         ";no=" + std::to_string(counts.no_literal_sites) +
         ";null=" + std::to_string(counts.null_literal_sites) +
         ";tokens=" + std::to_string(token_count);
}
