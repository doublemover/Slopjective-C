#include "sema/objc3_semantic_passes_feature_claim_unsupported_diagnostic_shaping.inc"

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context);

#include "sema/objc3_semantic_passes_feature_claim_unsupported_block_profiles.inc"
#include "sema/objc3_semantic_passes_feature_claim_unsupported_statement_walk.inc"
#include "sema/objc3_semantic_passes_feature_claim_unsupported_expression_walk.inc"
