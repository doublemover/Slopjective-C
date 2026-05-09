#include "artifacts/objc3_frontend_parser_diagnostic_artifacts.h"

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

namespace objc3::artifacts::frontend {
namespace {

std::string TryExtractDiagnosticCode(const std::string &diag_text, bool &ok) {
  ok = false;
  const std::size_t end = diag_text.size();
  if (end < 3u || diag_text[end - 1] != ']') {
    return std::string{};
  }
  const std::size_t begin = diag_text.rfind('[');
  if (begin == std::string::npos || begin + 2u >= end) {
    return std::string{};
  }
  const std::string code = diag_text.substr(begin + 1u, end - begin - 2u);
  if (code.empty()) {
    return std::string{};
  }
  ok = true;
  return code;
}

std::uint64_t MixParserDiagnosticCodeFingerprint(std::uint64_t fingerprint,
                                                 const std::string &code) {
  constexpr std::uint64_t kFnvPrime = 1099511628211ull;
  fingerprint =
      (fingerprint ^ static_cast<std::uint64_t>(code.size())) * kFnvPrime;
  for (const unsigned char c : code) {
    fingerprint = (fingerprint ^ static_cast<std::uint64_t>(c)) * kFnvPrime;
  }
  return fingerprint;
}

}  // namespace

Objc3ParserDiagnosticCodeCoverage BuildObjc3ParserDiagnosticCodeCoverage(
    const std::vector<std::string> &parser_diagnostics) {
  Objc3ParserDiagnosticCodeCoverage coverage;
  std::unordered_set<std::string> unique_codes;
  unique_codes.reserve(parser_diagnostics.size());
  for (const auto &diag_text : parser_diagnostics) {
    bool code_ok = false;
    const std::string code = TryExtractDiagnosticCode(diag_text, code_ok);
    if (!code_ok) {
      coverage.deterministic_surface = false;
      continue;
    }
    unique_codes.insert(code);
  }
  std::vector<std::string> sorted_codes(unique_codes.begin(),
                                        unique_codes.end());
  std::sort(sorted_codes.begin(), sorted_codes.end());
  coverage.unique_code_count = sorted_codes.size();
  for (const auto &code : sorted_codes) {
    coverage.unique_code_fingerprint =
        MixParserDiagnosticCodeFingerprint(coverage.unique_code_fingerprint,
                                           code);
  }
  return coverage;
}

}  // namespace objc3::artifacts::frontend
