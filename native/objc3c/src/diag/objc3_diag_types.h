#pragma once

#include <limits>
#include <string>

struct DiagSortKey {
  unsigned severity_rank = std::numeric_limits<unsigned>::max();
  std::string severity = "unknown";
  unsigned line = std::numeric_limits<unsigned>::max();
  unsigned column = std::numeric_limits<unsigned>::max();
  std::string code;
  std::string message;
  std::string raw;
};
