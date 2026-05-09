#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace objc3::artifacts::frontend {

struct Objc3ParserDiagnosticCodeCoverage {
  std::size_t unique_code_count = 0;
  std::uint64_t unique_code_fingerprint = 1469598103934665603ull;
  bool deterministic_surface = true;
};

[[nodiscard]] Objc3ParserDiagnosticCodeCoverage BuildObjc3ParserDiagnosticCodeCoverage(
    const std::vector<std::string> &parser_diagnostics);

}  // namespace objc3::artifacts::frontend
