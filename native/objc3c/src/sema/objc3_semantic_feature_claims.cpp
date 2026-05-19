#include "sema/objc3_semantic_feature_claims.h"

#include "diag/objc3_diag_utils.h"

unsigned OwnershipQualifierLine(
    const std::vector<Objc3SemaTokenMetadata> &tokens,
    unsigned default_line) {
  return tokens.empty() ? default_line : tokens.front().line;
}

unsigned OwnershipQualifierColumn(
    const std::vector<Objc3SemaTokenMetadata> &tokens,
    unsigned default_column) {
  return tokens.empty() ? default_column : tokens.front().column;
}

void RecordUnsupportedFeatureClaimDiagnostic(
    std::size_t &site_counter,
    unsigned line,
    unsigned column,
    const std::string &message,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats) {
  ++site_counter;
  ++stats.live_unsupported_feature_site_count;
  ++stats.live_unsupported_feature_diagnostic_count;
  diagnostics.push_back(MakeDiag(line, column, "O3S221", message));
}
