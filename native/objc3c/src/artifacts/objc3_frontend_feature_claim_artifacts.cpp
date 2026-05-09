#include "artifacts/objc3_frontend_feature_claim_artifacts.h"

#include "token/objc3_token_contract.h"

namespace objc3::artifacts {

std::vector<std::string> BuildRunnableFeatureClaimIds() {
  return {
      kObjc3RunnableFeatureClaimModule,
      kObjc3RunnableFeatureClaimGlobalLet,
      kObjc3RunnableFeatureClaimFunctionBodies,
      kObjc3RunnableFeatureClaimExternPrototypes,
      kObjc3RunnableFeatureClaimScalarCore,
      kObjc3RunnableFeatureClaimControlFlow,
      kObjc3RunnableFeatureClaimMessageSend,
  };
}

std::vector<std::string> BuildSourceOnlyFeatureClaimIds() {
  return {
      kObjc3SourceOnlyFeatureClaimProtocols,
      kObjc3SourceOnlyFeatureClaimInterfaces,
      kObjc3SourceOnlyFeatureClaimImplementations,
      kObjc3SourceOnlyFeatureClaimCategories,
      kObjc3SourceOnlyFeatureClaimProperties,
      kObjc3SourceOnlyFeatureClaimObjectPointerSurface,
  };
}

std::vector<std::string> BuildUnsupportedFeatureClaimIds() {
  return {
      kObjc3UnsupportedFeatureClaimStrictness,
      kObjc3UnsupportedFeatureClaimStrictConcurrency,
      kObjc3UnsupportedFeatureClaimThrows,
      kObjc3UnsupportedFeatureClaimAsyncAwait,
      kObjc3UnsupportedFeatureClaimActors,
      kObjc3UnsupportedFeatureClaimBlocks,
      kObjc3UnsupportedFeatureClaimArc,
  };
}

std::vector<std::string> BuildSupportedSelectionSurfaceIds() {
  return {
      kObjc3SupportedSelectionSurfaceLanguageVersion,
      kObjc3SupportedSelectionSurfaceLanguageProfile,
  };
}

std::vector<std::string> BuildUnsupportedSelectionSurfaceIds() {
  return {
      kObjc3UnsupportedSelectionSurfaceStrictness,
      kObjc3UnsupportedSelectionSurfaceStrictConcurrency,
      kObjc3RejectedSelectionSurfaceCanonicalRejectionDiagnostics,
  };
}

std::vector<std::string> BuildSuppressedMacroClaimIds() {
  return {
      kObjc3SuppressedMacroClaimStrictnessLevel,
      kObjc3SuppressedMacroClaimConcurrencyMode,
      kObjc3SuppressedMacroClaimConcurrencyStrict,
  };
}

}  // namespace objc3::artifacts
