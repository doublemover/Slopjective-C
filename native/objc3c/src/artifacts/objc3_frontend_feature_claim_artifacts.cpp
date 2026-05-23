#include "artifacts/objc3_frontend_feature_claim_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"
#include "io/json/json_writer.h"
#include "pipeline/objc3_frontend_types.h"
#include "token/objc3_token_contract.h"

namespace objc3::artifacts {
namespace {

using objc3::io::EscapeJsonString;

const char *LanguageProfileName(Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

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
      kObjc3UnsupportedFeatureClaimTypedThrows,
      kObjc3UnsupportedFeatureClaimValueOptionals,
      kObjc3UnsupportedFeatureClaimMatchExpressions,
      kObjc3UnsupportedFeatureClaimGuardedPatterns,
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

std::string BuildRunnableFeatureClaimInventoryReplayKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  std::ostringstream out;
  out << kObjc3RunnableFeatureClaimInventoryContractId
      << ";language_mode=" << kObjc3RunnableFeatureClaimModeName
      << ";language_version=" << static_cast<unsigned>(options.language_version)
      << ";language_profile=" << LanguageProfileName(options.language_profile)
      << ";truth_model=" << kObjc3RunnableFeatureClaimTruthModel
      << ";declared_globals=" << pipeline_result.program.ast.globals.size()
      << ";declared_functions=" << pipeline_result.program.ast.functions.size()
      << ";declared_protocols=" << pipeline_result.program.ast.protocols.size()
      << ";declared_interfaces=" << pipeline_result.program.ast.interfaces.size()
      << ";declared_implementations="
      << pipeline_result.program.ast.implementations.size()
      << ";protocol_properties="
      << pipeline_result.parser_contract_snapshot.protocol_property_decl_count
      << ";interface_properties="
      << pipeline_result.parser_contract_snapshot.interface_property_decl_count
      << ";implementation_properties="
      << pipeline_result.parser_contract_snapshot
             .implementation_property_decl_count
      << ";long_tail_constructs="
      << pipeline_result.parser_contract_snapshot
             .long_tail_grammar_construct_count;
  return out.str();
}

std::string BuildRunnableFeatureClaimInventoryJson(
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  const std::vector<std::string> runnable_claim_ids =
      BuildRunnableFeatureClaimIds();
  const std::vector<std::string> source_only_claim_ids =
      BuildSourceOnlyFeatureClaimIds();
  const std::vector<std::string> unsupported_claim_ids =
      BuildUnsupportedFeatureClaimIds();
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << kObjc3RunnableFeatureClaimInventoryContractId
      << "\",\"effective_language_mode\":\"" << kObjc3RunnableFeatureClaimModeName
      << "\",\"effective_language_version\":"
      << static_cast<unsigned>(options.language_version)
      << ",\"effective_language_profile\":\""
      << LanguageProfileName(options.language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << "true"
      << ",\"strictness_selection_supported\":false"
      << ",\"strict_concurrency_mode_supported\":false"
      << ",\"mode_truth_fail_closed\":true"
      << ",\"truth_model\":\"" << kObjc3RunnableFeatureClaimTruthModel << "\""
      << ",\"runnable_feature_claim_count\":" << runnable_claim_ids.size()
      << ",\"source_only_feature_claim_count\":" << source_only_claim_ids.size()
      << ",\"unsupported_feature_claim_count\":" << unsupported_claim_ids.size()
      << ",\"declared_protocol_count\":"
      << pipeline_result.program.ast.protocols.size()
      << ",\"declared_interface_count\":"
      << pipeline_result.program.ast.interfaces.size()
      << ",\"declared_implementation_count\":"
      << pipeline_result.program.ast.implementations.size()
      << ",\"declared_function_count\":"
      << pipeline_result.program.ast.functions.size()
      << ",\"declared_global_count\":"
      << pipeline_result.program.ast.globals.size()
      << ",\"long_tail_construct_count\":"
      << pipeline_result.parser_contract_snapshot.long_tail_grammar_construct_count
      << ",\"runnable_feature_claim_ids\":"
      << BuildStringArrayJson(runnable_claim_ids)
      << ",\"source_only_feature_claim_ids\":"
      << BuildStringArrayJson(source_only_claim_ids)
      << ",\"unsupported_feature_claim_ids\":"
      << BuildStringArrayJson(unsupported_claim_ids)
      << ",\"replay_key\":\""
      << EscapeJsonString(
             BuildRunnableFeatureClaimInventoryReplayKey(options,
                                                         pipeline_result))
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts
