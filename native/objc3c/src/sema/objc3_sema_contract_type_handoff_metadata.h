#pragma once

#include <cstddef>
#include <string>

struct Objc3AutoreleasePoolScopeSiteMetadata {
  std::string scope_symbol;
  unsigned scope_depth = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3AutoreleasePoolScopeSummary {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  unsigned max_scope_depth = 0;
  bool deterministic = true;
};
