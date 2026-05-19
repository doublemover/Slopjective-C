#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "parse/objc3_parser_contract_types.h"
#include "token/objc3_sema_token_metadata.h"

struct Objc3UnsupportedFeatureClaimEnforcementStats {
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
};

struct Objc3UnsupportedFeatureClaimContext {
  std::size_t owned_runtime_backed_object_property_sites = 0;
  bool has_owned_runtime_backed_object_storage = false;
  bool allow_source_only_block_literals = false;
  bool allow_source_only_defer_statements = false;
  bool allow_source_only_error_runtime_surface = false;
  bool arc_mode_enabled = false;
};

Objc3UnsupportedFeatureClaimContext BuildUnsupportedFeatureClaimContext(
    const Objc3Program &ast,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled);

bool ShouldRejectThrowsFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context);
bool ShouldRejectDeferFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context);
bool ShouldRejectBlockLiteralFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context,
    bool block_literal_uses_runnable_subset);
bool ShouldRejectOwnershipQualifierFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context,
    bool source_only_block_literal_scope,
    bool runnable_block_literal_scope);

unsigned OwnershipQualifierLine(
    const std::vector<Objc3SemaTokenMetadata> &tokens,
    unsigned default_line);
unsigned OwnershipQualifierColumn(
    const std::vector<Objc3SemaTokenMetadata> &tokens,
    unsigned default_column);
void RecordUnsupportedFeatureClaimDiagnostic(
    std::size_t &site_counter,
    unsigned line,
    unsigned column,
    const std::string &message,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats);
