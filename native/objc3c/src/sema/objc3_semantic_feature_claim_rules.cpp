#include "sema/objc3_semantic_feature_claims.h"

bool ShouldRejectThrowsFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context) {
  return !context.allow_source_only_error_runtime_surface;
}

bool ShouldRejectDeferFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context) {
  return !context.allow_source_only_defer_statements;
}

bool ShouldRejectBlockLiteralFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context,
    bool block_literal_uses_runnable_subset) {
  return !context.allow_source_only_block_literals &&
         !block_literal_uses_runnable_subset;
}

bool ShouldRejectOwnershipQualifierFeatureClaim(
    const Objc3UnsupportedFeatureClaimContext &context,
    bool source_only_block_literal_scope,
    bool runnable_block_literal_scope) {
  return !context.arc_mode_enabled && !source_only_block_literal_scope &&
         !runnable_block_literal_scope;
}
