#pragma once

#include <cstddef>

struct Objc3FrontendCanonicalLiteralRejectionCounts {
  std::size_t yes_literal_sites = 0;
  std::size_t no_literal_sites = 0;
  std::size_t null_literal_sites = 0;

  std::size_t total_literal_sites() const {
    return yes_literal_sites + no_literal_sites + null_literal_sites;
  }
};

inline bool IsObjc3FrontendCanonicalLiteralRejectionCountsConsistent(
    const Objc3FrontendCanonicalLiteralRejectionCounts &counts,
    std::size_t token_count) {
  const std::size_t total = counts.total_literal_sites();
  return total == counts.yes_literal_sites + counts.no_literal_sites +
                      counts.null_literal_sites &&
         total <= token_count && total == 0;
}
