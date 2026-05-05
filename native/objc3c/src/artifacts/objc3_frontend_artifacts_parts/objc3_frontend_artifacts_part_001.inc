#include "artifacts/objc3_frontend_artifacts.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "contracts/objc3_frontend_diagnostics_bus_contract.h"
#include "diag/objc3_diag_utils.h"
#include "ir/objc3_ir_emitter.h"
#include "io/objc3_json.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "sema/objc3_semantic_passes.h"
#include "support/objc3_identifier_safe_suffix.h"
#include "support/objc3_runtime_metadata_record_set.h"
#include "support/objc3_value_type_names.h"

namespace {

using objc3::io::EscapeJsonString;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

std::string BuildObjc3TranslationUnitIdentityKey(
    const std::string &constructor_root_symbol,
    std::uint64_t bootstrap_registration_order_ordinal);
std::string BuildExecutableMetadataSourceGraphJson(
    const Objc3ExecutableMetadataSourceGraph &graph);
std::string BuildExecutableMetadataSemanticConsistencyBoundaryJson(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary);

std::size_t CountSubstringOccurrences(const std::string &text,
                                      const std::string &needle) {
  if (needle.empty()) {
    return 0u;
  }
  std::size_t count = 0u;
  std::size_t offset = 0u;
  while ((offset = text.find(needle, offset)) != std::string::npos) {
    ++count;
    offset += needle.size();
  }
  return count;
}

bool HasRuntimeBearingExecutableSurface(
    const Objc3Program &program,
    const Objc3MessageSendSelectorLoweringContract &message_send_contract) {
  if (message_send_contract.message_send_sites > 0u) {
    return true;
  }
  for (const auto &implementation : program.implementations) {
    if (!implementation.properties.empty()) {
      return true;
    }
    for (const auto &method : implementation.methods) {
      if (!method.body.empty()) {
        return true;
      }
    }
  }
  return false;
}

bool IsSuspiciousNativeIRTruthGap(
    const std::string &ir_text,
    const Objc3Program &program,
    const Objc3MessageSendSelectorLoweringContract &message_send_contract) {
  if (!HasRuntimeBearingExecutableSurface(program, message_send_contract)) {
    return false;
  }

  const bool has_dispatch_surface =
      ir_text.find("@objc3_runtime_dispatch_i32") != std::string::npos;
  const bool has_property_runtime_surface =
      ir_text.find("@objc3_runtime_read_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_write_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_exchange_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_load_weak_current_property_i32") !=
          std::string::npos ||
      ir_text.find("@objc3_runtime_store_weak_current_property_i32") !=
          std::string::npos;
  const bool has_runtime_metadata_surface =
      ir_text.find("__objc3_class_descriptor") != std::string::npos ||
      ir_text.find("__objc3_method_entry") != std::string::npos ||
      ir_text.find("__objc3_property_descriptor") != std::string::npos;

  const std::size_t define_count =
      CountSubstringOccurrences(ir_text, "\ndefine ") +
      (ir_text.rfind("define ", 0) == 0 ? 1u : 0u);
  const bool has_stub_main =
      ir_text.find("define i32 @main()") != std::string::npos &&
      ir_text.find("ret i32 0") != std::string::npos && define_count <= 1u;

  return has_stub_main ||
         (!has_dispatch_surface && !has_property_runtime_surface &&
          !has_runtime_metadata_surface);
}
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_lowering_contract";
inline constexpr const char
    *kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_control_flow_control_flow_safety_lowering_contract";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringContractId =
        "objc3c.concurrency.continuation.abi.async.lowering.contract.v1";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_continuation_abi_and_async_lowering_contract";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringContinuationModel =
        "async-entry-points-carry-deterministic-continuation-lowering-replay-keys-and-counts-into-emitted-ir";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringAwaitModel =
        "await-suspension-state-lowering-replay-keys-and-counts-are-published-alongside-continuation-lowering";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringDeferredModel =
        "runnable-async-frame-layout-resume-cleanup-and-executor-runtime-execution-remain-later-runtime-work";
// implementation anchor: publish a second Part 7 packet that states
// the current runnable lowering truthfully. The live native slice is
// direct-call based and non-suspending; it does not claim a full continuation
// runtime yet.
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringContractId =
        "objc3c.concurrency.async.direct.call.lowering.v1";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_async_function_await_and_continuation_lowering";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringImplementationModel =
        "supported-async-functions-and-await-lower-through-direct-calls-in-native-ir-and-object-emission-for-the-current-non-suspending-slice";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringAwaitModel =
        "await-marked-expressions-currently-reuse-the-operand-direct-call-lowering-path-without-materializing-a-suspension-state-machine";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringDeferredModel =
        "continuation-allocation-resume-suspend-state-machine-cleanup-and-executor-runtime-scheduling-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationContractId =
        "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1";
// integration anchor: this packet ties the current direct-call async
// slice to the already-live autoreleasepool and defer-cleanup lowering surfaces
// instead of claiming a separate continuation cleanup runtime.
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_suspension_autorelease_and_cleanup_integration";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationModel =
        "supported-non-suspending-async-lowering-reuses-existing-autoreleasepool-scope-and-defer-cleanup-lowering-through-real-ir-and-object-emission";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationOrderingModel =
        "current-proof-fixtures-show-terminal-return-paths-compose-await-direct-call-lowering-with-autoreleasepool-pop-and-defer-cleanup-without-a-separate-suspension-runtime";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationDeferredModel =
        "continuation-resume-cleanup-suspension-state-frames-and-executor-runtime-scheduling-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringContractId =
    "objc3c.concurrency.task.runtime.lowering.contract.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_task_runtime_lowering_contract";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringTaskModel =
    "task-creation-cancellation-polls-and-task-group-artifacts-now-lower-through-explicit-replay-stable-lane-contracts";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringExecutorModel =
    "executor-affinity-and-detached-hop-boundaries-now-lower-through-explicit-actor-and-task-runtime-profile-handoffs";
// implementation anchor: this lowering packet now maps to live
// helper-backed IR rewrites for the supported task-runtime symbol family.
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringConcurrencyModel =
    "scheduler-visible-task-handoff-and-cancellation-guard-proof-points-now-lower-through-deterministic-concurrency-replay-profiles";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringDeferredModel =
    "native-task-spawn-executor-hop-cancellation-runtime-entrypoints-and-task-group-abi-completion-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataContractId =
    "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_lowering_and_metadata_contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataModel =
    "actor-member-semantic-and-hazard-packets-now-lower-through-one-deterministic-actor-metadata-isolation-thunk-and-hop-artifact-contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataDeferredModel =
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorMailboxRuntimeImportContractId =
    "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1";
inline constexpr const char *kObjc3ConcurrencyActorMailboxRuntimeImportSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface";
inline constexpr const char *kObjc3ConcurrencyActorMailboxRuntimeImportSourceModel =
    "runtime-import-surface-preserves-actor-lowering-and-isolation-replay-facts-for-cross-module-runtime-link-planning";
inline constexpr const char *kObjc3ConcurrencyActorMailboxRuntimeImportFailClosedModel =
    "missing-or-drifted-actor-mailbox-runtime-import-packets-disable-cross-module-actor-runtime-preservation-claims";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId =
    "objc3c.concurrency.task.runtime.abi.completion.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_task_group_and_runtime_abi_completion";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionArtifactModel =
    "helper-backed-task-runtime-lowering-now-publishes-a-dedicated-abi-and-runtime-proof-packet";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionProofModel =
    "scheduler-visible-runtime-proof-remains-private-and-snapshotted-through-objc3_runtime_copy_task_runtime_state_for_testing";
inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel =
        "borrowed-return-contracts-and-retainable-family-call-boundaries-now-publish-a-dedicated-ownership-abi-and-replay-packet-above-the-frozen-lowering-contract";
inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel =
        "the-supported-proof-slice-remains-direct-call-abi-emission-and-replay-stability-without-claiming-lane-d-runtime-helper-integration";

std::string BuildInteropBridgeCType(ValueType type, unsigned pointer_depth,
                                   bool object_pointer_type_spelling) {
  std::string base;
  switch (type) {
    case ValueType::I32:
      base = "int32_t";
      break;
    case ValueType::Bool:
      base = "bool";
      break;
    case ValueType::Void:
      base = "void";
      break;
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
    case ValueType::Function:
    default:
      base = "void";
      pointer_depth = std::max(pointer_depth, 1u);
      break;
  }
  if (object_pointer_type_spelling) {
    base = "void";
    pointer_depth = std::max(pointer_depth, 1u);
  }
  for (unsigned i = 0; i < pointer_depth; ++i) {
    base += "*";
  }
  return base;
}

std::string BuildInteropBridgeReturnType(const FunctionDecl &function) {
  return BuildInteropBridgeCType(function.return_type,
                                function.has_return_pointer_declarator
                                    ? function.return_pointer_declarator_depth
                                    : 0u,
                                function.return_object_pointer_type_spelling);
}

std::string BuildInteropBridgeParameterType(const FuncParam &param) {
  return BuildInteropBridgeCType(param.type,
                                param.has_pointer_declarator
                                    ? param.pointer_declarator_depth
                                    : 0u,
                                param.object_pointer_type_spelling);
}

const char *LanguageProfileName(Objc3FrontendLanguageProfile mode) {
  (void)mode;
  return "canonical";
}

const char *ArcModeName(Objc3FrontendArcMode mode) {
  return mode == Objc3FrontendArcMode::kEnabled ? "enabled" : "disabled";
}

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t index = 0; index < values.size(); ++index) {
    if (index != 0) {
      out << ",";
    }
    out << "\"" << EscapeJsonString(values[index]) << "\"";
  }
  out << "]";
  return out.str();
}

struct Objc3ErrorHandlingResultAndBridgingArtifactReplaySurfaceSummary {
  std::string contract_id =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId;
  std::string source_contract_id =
      kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId;
  std::string surface_path =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplaySurfacePath;
  std::string import_artifact_member_name =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName;
  std::string source_model =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel;
  std::string replay_model =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel;
  std::string fail_closed_model =
      kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel;
  std::string replay_key;
  std::string error_handling_replay_key;
  std::string throws_replay_key;
  std::string result_like_replay_key;
  std::string ns_error_replay_key;
  std::string unwind_replay_key;
  std::vector<std::string> imported_module_names_lexicographic;
  std::vector<std::string> imported_error_handling_replay_keys_lexicographic;
  std::vector<std::string> imported_result_like_replay_keys_lexicographic;
  std::vector<std::string> imported_ns_error_replay_keys_lexicographic;
  bool binary_artifact_replay_ready = false;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_replay_ready = false;
  bool deterministic = false;
};

Objc3ErrorHandlingResultAndBridgingArtifactReplaySurfaceSummary
BuildErrorHandlingResultAndBridgingArtifactReplaySummary(
    const std::string &error_handling_replay_key,
    const std::string &throws_replay_key,
    const std::string &result_like_replay_key,
    const std::string &ns_error_replay_key,
    const std::string &unwind_replay_key,
    bool deterministic_error_handling_handoff,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3ErrorHandlingResultAndBridgingArtifactReplaySurfaceSummary summary;
  summary.error_handling_replay_key = error_handling_replay_key;
  summary.throws_replay_key = throws_replay_key;
  summary.result_like_replay_key = result_like_replay_key;
  summary.ns_error_replay_key = ns_error_replay_key;
  summary.unwind_replay_key = unwind_replay_key;
  summary.binary_artifact_replay_ready = !error_handling_replay_key.empty() &&
                                         !throws_replay_key.empty() &&
                                         !result_like_replay_key.empty() &&
                                         !ns_error_replay_key.empty() &&
                                         !unwind_replay_key.empty();
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready && summary.binary_artifact_replay_ready;
  summary.deterministic = deterministic_error_handling_handoff;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.error_handling_result_and_bridging_artifact_replay_present) {
      continue;
    }
    summary.imported_module_names_lexicographic.push_back(
        surface.frontend_closure_summary.module_name);
    summary.imported_error_handling_replay_keys_lexicographic.push_back(
        surface.error_handling_error_handling_replay_key);
    summary.imported_result_like_replay_keys_lexicographic.push_back(
        surface.error_handling_result_like_replay_key);
    summary.imported_ns_error_replay_keys_lexicographic.push_back(
        surface.error_handling_ns_error_replay_key);
    summary.deterministic = summary.deterministic && surface.error_handling_deterministic;
  }
  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  std::sort(summary.imported_error_handling_replay_keys_lexicographic.begin(),
            summary.imported_error_handling_replay_keys_lexicographic.end());
  std::sort(summary.imported_result_like_replay_keys_lexicographic.begin(),
            summary.imported_result_like_replay_keys_lexicographic.end());
  std::sort(summary.imported_ns_error_replay_keys_lexicographic.begin(),
            summary.imported_ns_error_replay_keys_lexicographic.end());
  const std::size_t imported_error_handling_module_count =
      std::count_if(imported_runtime_module_surfaces.begin(),
                    imported_runtime_module_surfaces.end(),
                    [](const Objc3ImportedRuntimeModuleSurface &surface) {
                      return surface
                          .error_handling_result_and_bridging_artifact_replay_present;
                    });
  summary.separate_compilation_replay_ready =
      summary.binary_artifact_replay_ready &&
      summary.runtime_import_artifact_ready &&
      summary.imported_module_names_lexicographic.size() ==
          imported_error_handling_module_count;
  std::ostringstream replay_key;
  replay_key << Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary()
             << ";binary_artifact_replay_ready="
             << (summary.binary_artifact_replay_ready ? "true" : "false")
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_replay_ready="
             << (summary.separate_compilation_replay_ready ? "true" : "false")
             << ";imported_module_count="
             << summary.imported_module_names_lexicographic.size()
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";error_handling_replay_key=" << error_handling_replay_key
             << ";result_like_replay_key=" << result_like_replay_key
             << ";ns_error_replay_key=" << ns_error_replay_key;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildErrorHandlingResultAndBridgingArtifactReplaySummaryJson(
    const Objc3ErrorHandlingResultAndBridgingArtifactReplaySurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"replay_model\":\"" << EscapeJsonString(summary.replay_model)
      << "\",\"error_handling_replay_key\":\""
      << EscapeJsonString(summary.error_handling_replay_key)
      << "\",\"throws_replay_key\":\""
      << EscapeJsonString(summary.throws_replay_key)
      << "\",\"result_like_replay_key\":\""
      << EscapeJsonString(summary.result_like_replay_key)
      << "\",\"ns_error_replay_key\":\""
      << EscapeJsonString(summary.ns_error_replay_key)
      << "\",\"unwind_replay_key\":\""
      << EscapeJsonString(summary.unwind_replay_key)
      << "\",\"imported_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.imported_module_names_lexicographic)
      << ",\"imported_error_handling_replay_keys_lexicographic\":"
      << BuildStringArrayJson(summary.imported_error_handling_replay_keys_lexicographic)
      << ",\"imported_result_like_replay_keys_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_result_like_replay_keys_lexicographic)
      << ",\"imported_ns_error_replay_keys_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_ns_error_replay_keys_lexicographic)
      << ",\"binary_artifact_replay_ready\":"
      << (summary.binary_artifact_replay_ready ? "true" : "false")
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_replay_ready\":"
      << (summary.separate_compilation_replay_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

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

constexpr char kObjc3RuntimeAwareImportModuleSurfaceContractId[] =
    "objc3c.runtime.aware.import.module.surface.v1";
constexpr char kObjc3RuntimeAwareImportModuleSurfacePath[] =
    "frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract";
constexpr char kObjc3RuntimeAwareImportModuleSurfaceSourceModel[] =
    "runtime-aware-import-module-surface-freezes-frontend-owned-runtime-declaration-and-metadata-reference-boundaries-before-cross-translation-unit-realization";
constexpr char kObjc3RuntimeAwareImportModuleSurfaceNonGoalModel[] =
    "no-imported-module-artifact-reader-no-imported-runtime-declaration-materialization-no-imported-runtime-metadata-reference-lowering";
constexpr char kObjc3RuntimeAwareImportModuleSurfaceFailureModel[] =
    "fail-closed-on-runtime-aware-import-module-surface-drift-or-premature-capability-claims";

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
      << ";declared_implementations=" << pipeline_result.program.ast.implementations.size()
      << ";protocol_properties=" << pipeline_result.parser_contract_snapshot.protocol_property_decl_count
      << ";interface_properties=" << pipeline_result.parser_contract_snapshot.interface_property_decl_count
      << ";implementation_properties=" << pipeline_result.parser_contract_snapshot.implementation_property_decl_count
      << ";long_tail_constructs=" << pipeline_result.parser_contract_snapshot.long_tail_grammar_construct_count;
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
             BuildRunnableFeatureClaimInventoryReplayKey(options, pipeline_result))
      << "\"}";
  return out.str();
}

std::string BuildTypeSystemTypeSourceClosureSummaryJson(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"unsupported_claim_ids\":"
      << BuildStringArrayJson(summary.unsupported_claim_ids)
      << ",\"protocol_required_method_count\":"
      << summary.protocol_required_method_count
      << ",\"protocol_optional_method_count\":"
      << summary.protocol_optional_method_count
      << ",\"protocol_required_property_count\":"
      << summary.protocol_required_property_count
      << ",\"protocol_optional_property_count\":"
      << summary.protocol_optional_property_count
      << ",\"object_pointer_type_spelling_sites\":"
      << summary.object_pointer_type_spelling_sites
      << ",\"pointer_declarator_entries\":"
      << summary.pointer_declarator_entries
      << ",\"nullability_suffix_entries\":"
      << summary.nullability_suffix_entries
      << ",\"generic_suffix_entries\":"
      << summary.generic_suffix_entries
      << ",\"optional_binding_sites\":"
      << summary.optional_binding_sites
      << ",\"guard_binding_sites\":"
      << summary.guard_binding_sites
      << ",\"optional_send_sites\":"
      << summary.optional_send_sites
      << ",\"optional_member_access_sites\":"
      << summary.optional_member_access_sites
      << ",\"nil_coalescing_sites\":"
      << summary.nil_coalescing_sites
      << ",\"typed_keypath_literal_sites\":"
      << summary.typed_keypath_literal_sites
      << ",\"protocol_optional_partition_source_supported\":"
      << (summary.protocol_optional_partition_source_supported ? "true" : "false")
      << ",\"object_pointer_nullability_source_supported\":"
      << (summary.object_pointer_nullability_source_supported ? "true" : "false")
      << ",\"pragmatic_generic_suffix_source_supported\":"
      << (summary.pragmatic_generic_suffix_source_supported ? "true" : "false")
      << ",\"optional_binding_source_supported\":"
      << (summary.optional_binding_source_supported ? "true" : "false")
      << ",\"optional_send_source_supported\":"
      << (summary.optional_send_source_supported ? "true" : "false")
      << ",\"nil_coalescing_source_supported\":"
      << (summary.nil_coalescing_source_supported ? "true" : "false")
      << ",\"typed_keypath_literal_source_supported\":"
      << (summary.typed_keypath_literal_source_supported ? "true" : "false")
      << ",\"optional_member_access_fail_closed\":"
      << (summary.optional_member_access_fail_closed ? "true" : "false")
      << ",\"nil_coalescing_fail_closed\":"
      << (summary.nil_coalescing_fail_closed ? "true" : "false")
      << ",\"typed_keypath_literal_fail_closed\":"
      << (summary.typed_keypath_literal_fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildControlFlowControlFlowSourceClosureSummaryJson(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"supported_construct_ids\":"
      << BuildStringArrayJson(summary.supported_construct_ids)
      << ",\"fail_closed_construct_ids\":"
      << BuildStringArrayJson(summary.fail_closed_construct_ids)
      << ",\"guard_binding_sites\":" << summary.guard_binding_sites
      << ",\"guard_binding_clause_sites\":" << summary.guard_binding_clause_sites
      << ",\"guard_boolean_condition_sites\":"
      << summary.guard_boolean_condition_sites
      << ",\"switch_case_pattern_sites\":" << summary.switch_case_pattern_sites
      << ",\"switch_default_pattern_sites\":"
      << summary.switch_default_pattern_sites
      << ",\"defer_keyword_sites\":" << summary.defer_keyword_sites
      << ",\"match_statement_sites\":" << summary.match_statement_sites
      << ",\"match_case_pattern_sites\":" << summary.match_case_pattern_sites
      << ",\"match_default_sites\":" << summary.match_default_sites
      << ",\"match_wildcard_pattern_sites\":"
      << summary.match_wildcard_pattern_sites
      << ",\"match_literal_pattern_sites\":"
      << summary.match_literal_pattern_sites
      << ",\"match_binding_pattern_sites\":"
      << summary.match_binding_pattern_sites
      << ",\"match_result_case_pattern_sites\":"
      << summary.match_result_case_pattern_sites
      << ",\"guard_binding_source_supported\":"
      << (summary.guard_binding_source_supported ? "true" : "false")
      << ",\"guard_condition_list_source_supported\":"
      << (summary.guard_condition_list_source_supported ? "true" : "false")
      << ",\"switch_case_pattern_source_supported\":"
      << (summary.switch_case_pattern_source_supported ? "true" : "false")
      << ",\"defer_statement_source_supported\":"
      << (summary.defer_statement_source_supported ? "true" : "false")
      << ",\"match_statement_source_supported\":"
      << (summary.match_statement_source_supported ? "true" : "false")
      << ",\"match_wildcard_pattern_source_supported\":"
      << (summary.match_wildcard_pattern_source_supported ? "true" : "false")
      << ",\"match_literal_pattern_source_supported\":"
      << (summary.match_literal_pattern_source_supported ? "true" : "false")
      << ",\"match_binding_pattern_source_supported\":"
      << (summary.match_binding_pattern_source_supported ? "true" : "false")
      << ",\"match_result_case_pattern_source_supported\":"
      << (summary.match_result_case_pattern_source_supported ? "true" : "false")
      << ",\"defer_keyword_reserved\":"
      << (summary.defer_keyword_reserved ? "true" : "false")
      << ",\"defer_fail_closed\":"
      << (summary.defer_fail_closed ? "true" : "false")
      << ",\"match_expression_fail_closed\":"
      << (summary.match_expression_fail_closed ? "true" : "false")
      << ",\"guarded_pattern_fail_closed\":"
      << (summary.guarded_pattern_fail_closed ? "true" : "false")
      << ",\"type_test_pattern_fail_closed\":"
      << (summary.type_test_pattern_fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildErrorHandlingErrorSourceClosureSummaryJson(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"fail_closed_construct_ids\":"
      << BuildStringArrayJson(summary.fail_closed_construct_ids)
      << ",\"function_throws_declaration_sites\":"
      << summary.function_throws_declaration_sites
      << ",\"method_throws_declaration_sites\":"
      << summary.method_throws_declaration_sites
      << ",\"result_like_sites\":" << summary.result_like_sites
      << ",\"result_success_sites\":" << summary.result_success_sites
      << ",\"result_failure_sites\":" << summary.result_failure_sites
      << ",\"result_branch_sites\":" << summary.result_branch_sites
      << ",\"result_payload_sites\":" << summary.result_payload_sites
      << ",\"ns_error_bridging_sites\":" << summary.ns_error_bridging_sites
      << ",\"ns_error_out_parameter_sites\":"
      << summary.ns_error_out_parameter_sites
      << ",\"ns_error_bridge_path_sites\":"
      << summary.ns_error_bridge_path_sites
      << ",\"objc_nserror_attribute_sites\":"
      << summary.objc_nserror_attribute_sites
      << ",\"objc_status_code_attribute_sites\":"
      << summary.objc_status_code_attribute_sites
      << ",\"status_code_success_clause_sites\":"
      << summary.status_code_success_clause_sites
      << ",\"status_code_error_type_clause_sites\":"
      << summary.status_code_error_type_clause_sites
      << ",\"status_code_mapping_clause_sites\":"
      << summary.status_code_mapping_clause_sites
      << ",\"try_keyword_sites\":" << summary.try_keyword_sites
      << ",\"throw_keyword_sites\":" << summary.throw_keyword_sites
      << ",\"catch_keyword_sites\":" << summary.catch_keyword_sites
      << ",\"throws_declaration_source_supported\":"
      << (summary.throws_declaration_source_supported ? "true" : "false")
      << ",\"result_carrier_source_supported\":"
      << (summary.result_carrier_source_supported ? "true" : "false")
      << ",\"ns_error_bridging_source_supported\":"
      << (summary.ns_error_bridging_source_supported ? "true" : "false")
      << ",\"error_bridge_marker_source_supported\":"
      << (summary.error_bridge_marker_source_supported ? "true" : "false")
      << ",\"try_keyword_reserved\":"
      << (summary.try_keyword_reserved ? "true" : "false")
      << ",\"throw_keyword_reserved\":"
      << (summary.throw_keyword_reserved ? "true" : "false")
      << ",\"catch_keyword_reserved\":"
      << (summary.catch_keyword_reserved ? "true" : "false")
      << ",\"try_fail_closed\":"
      << (summary.try_fail_closed ? "true" : "false")
      << ",\"throw_fail_closed\":"
      << (summary.throw_fail_closed ? "true" : "false")
      << ",\"do_catch_fail_closed\":"
      << (summary.do_catch_fail_closed ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyAsyncSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"async_keyword_sites\":" << summary.async_keyword_sites
      << ",\"async_function_sites\":" << summary.async_function_sites
      << ",\"async_method_sites\":" << summary.async_method_sites
      << ",\"await_keyword_sites\":" << summary.await_keyword_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"executor_attribute_sites\":" << summary.executor_attribute_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"async_function_source_supported\":"
      << (summary.async_function_source_supported ? "true" : "false")
      << ",\"async_method_source_supported\":"
      << (summary.async_method_source_supported ? "true" : "false")
      << ",\"await_expression_source_supported\":"
      << (summary.await_expression_source_supported ? "true" : "false")
      << ",\"executor_attribute_source_supported\":"
      << (summary.executor_attribute_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipSystemExtensionSourceClosureSummaryJson(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_close_clause_sites\":" << summary.resource_close_clause_sites
      << ",\"resource_invalid_clause_sites\":" << summary.resource_invalid_clause_sites
      << ",\"borrowed_pointer_sites\":" << summary.borrowed_pointer_sites
      << ",\"returns_borrowed_attribute_sites\":" << summary.returns_borrowed_attribute_sites
      << ",\"explicit_capture_list_sites\":" << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":" << summary.explicit_capture_item_sites
      << ",\"explicit_capture_weak_sites\":" << summary.explicit_capture_weak_sites
      << ",\"explicit_capture_unowned_sites\":" << summary.explicit_capture_unowned_sites
      << ",\"explicit_capture_move_sites\":" << summary.explicit_capture_move_sites
      << ",\"explicit_capture_plain_sites\":" << summary.explicit_capture_plain_sites
      << ",\"resource_attribute_source_supported\":"
      << (summary.resource_attribute_source_supported ? "true" : "false")
      << ",\"borrowed_pointer_source_supported\":"
      << (summary.borrowed_pointer_source_supported ? "true" : "false")
      << ",\"returns_borrowed_source_supported\":"
      << (summary.returns_borrowed_source_supported ? "true" : "false")
      << ",\"explicit_capture_list_source_supported\":"
      << (summary.explicit_capture_list_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"cleanup_attribute_sites\":" << summary.cleanup_attribute_sites
      << ",\"cleanup_sugar_sites\":" << summary.cleanup_sugar_sites
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_sugar_sites\":" << summary.resource_sugar_sites
      << ",\"resource_close_clause_sites\":"
      << summary.resource_close_clause_sites
      << ",\"resource_invalid_clause_sites\":"
      << summary.resource_invalid_clause_sites
      << ",\"explicit_capture_list_sites\":" << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":" << summary.explicit_capture_item_sites
      << ",\"explicit_capture_weak_sites\":" << summary.explicit_capture_weak_sites
      << ",\"explicit_capture_unowned_sites\":"
      << summary.explicit_capture_unowned_sites
      << ",\"explicit_capture_move_sites\":" << summary.explicit_capture_move_sites
      << ",\"explicit_capture_plain_sites\":" << summary.explicit_capture_plain_sites
      << ",\"cleanup_attribute_source_supported\":"
      << (summary.cleanup_attribute_source_supported ? "true" : "false")
      << ",\"resource_sugar_source_supported\":"
      << (summary.resource_sugar_source_supported ? "true" : "false")
      << ",\"explicit_capture_list_source_supported\":"
      << (summary.explicit_capture_list_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"family_retain_sites\":" << summary.family_retain_sites
      << ",\"family_release_sites\":" << summary.family_release_sites
      << ",\"family_autorelease_sites\":" << summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << summary.compatibility_consumed_sites
      << ",\"callable_annotation_source_supported\":"
      << (summary.callable_annotation_source_supported ? "true" : "false")
      << ",\"compatibility_alias_source_supported\":"
      << (summary.compatibility_alias_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceClosureSummaryJson(
    const Objc3FrontendDispatchDispatchIntentSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"direct_callable_sites\":" << summary.direct_callable_sites
      << ",\"final_callable_sites\":" << summary.final_callable_sites
      << ",\"dynamic_callable_sites\":" << summary.dynamic_callable_sites
      << ",\"direct_members_container_sites\":"
      << summary.direct_members_container_sites
      << ",\"final_container_sites\":" << summary.final_container_sites
      << ",\"sealed_container_sites\":" << summary.sealed_container_sites
      << ",\"actor_container_sites\":" << summary.actor_container_sites
      << ",\"callable_annotation_source_supported\":"
      << (summary.callable_annotation_source_supported ? "true" : "false")
      << ",\"container_annotation_source_supported\":"
      << (summary.container_annotation_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildDispatchDispatchIntentSourceCompletionSummaryJson(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"prefixed_container_attribute_sites\":"
      << summary.prefixed_container_attribute_sites
      << ",\"direct_members_container_sites\":"
      << summary.direct_members_container_sites
      << ",\"final_container_sites\":" << summary.final_container_sites
      << ",\"sealed_container_sites\":" << summary.sealed_container_sites
      << ",\"effective_direct_member_sites\":"
      << summary.effective_direct_member_sites
      << ",\"direct_members_defaulted_method_sites\":"
      << summary.direct_members_defaulted_method_sites
      << ",\"direct_members_dynamic_opt_out_sites\":"
      << summary.direct_members_dynamic_opt_out_sites
      << ",\"prefixed_attribute_source_supported\":"
      << (summary.prefixed_attribute_source_supported ? "true" : "false")
      << ",\"defaulting_source_supported\":"
      << (summary.defaulting_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"derive_marker_sites\":" << summary.derive_marker_sites
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"derive_marker_source_supported\":"
      << (summary.derive_marker_source_supported ? "true" : "false")
      << ",\"macro_marker_source_supported\":"
      << (summary.macro_marker_source_supported ? "true" : "false")
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"macro_marker_sites\":" << summary.macro_marker_sites
      << ",\"macro_package_sites\":" << summary.macro_package_sites
      << ",\"macro_provenance_sites\":" << summary.macro_provenance_sites
      << ",\"macro_cache_key_sites\":" << summary.macro_cache_key_sites
      << ",\"macro_sandbox_policy_sites\":"
      << summary.macro_sandbox_policy_sites
      << ",\"expansion_visible_macro_sites\":"
      << summary.expansion_visible_macro_sites
      << ",\"macro_package_source_supported\":"
      << (summary.macro_package_source_supported ? "true" : "false")
      << ",\"macro_provenance_source_supported\":"
      << (summary.macro_provenance_source_supported ? "true" : "false")
      << ",\"macro_cache_key_source_supported\":"
      << (summary.macro_cache_key_source_supported ? "true" : "false")
      << ",\"macro_sandbox_policy_source_supported\":"
      << (summary.macro_sandbox_policy_source_supported ? "true" : "false")
      << ",\"expansion_visible_source_supported\":"
      << (summary.expansion_visible_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"property_behavior_sites\":" << summary.property_behavior_sites
      << ",\"interface_property_behavior_sites\":"
      << summary.interface_property_behavior_sites
      << ",\"implementation_property_behavior_sites\":"
      << summary.implementation_property_behavior_sites
      << ",\"protocol_property_behavior_sites\":"
      << summary.protocol_property_behavior_sites
      << ",\"synthesized_binding_visible_sites\":"
      << summary.synthesized_binding_visible_sites
      << ",\"synthesized_getter_visible_sites\":"
      << summary.synthesized_getter_visible_sites
      << ",\"synthesized_setter_visible_sites\":"
      << summary.synthesized_setter_visible_sites
      << ",\"property_behavior_source_supported\":"
      << (summary.property_behavior_source_supported ? "true" : "false")
      << ",\"synthesized_declaration_visibility_supported\":"
      << (summary.synthesized_declaration_visibility_supported ? "true"
                                                               : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropForeignImportSourceClosureSummaryJson(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"extern_foreign_callable_sites\":"
      << summary.extern_foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"imported_module_name_sites\":"
      << summary.imported_module_name_sites
      << ",\"export_header_annotation_sites\":"
      << summary.export_header_annotation_sites
      << ",\"export_header_name_sites\":"
      << summary.export_header_name_sites
      << ",\"mixed_image_annotation_sites\":"
      << summary.mixed_image_annotation_sites
      << ",\"mixed_image_name_sites\":"
      << summary.mixed_image_name_sites
      << ",\"package_entry_annotation_sites\":"
      << summary.package_entry_annotation_sites
      << ",\"package_entry_name_sites\":"
      << summary.package_entry_name_sites
      << ",\"interop_annotation_sites\":"
      << summary.interop_annotation_sites
      << ",\"foreign_declaration_source_supported\":"
      << (summary.foreign_declaration_source_supported ? "true" : "false")
      << ",\"imported_surface_source_supported\":"
      << (summary.imported_surface_source_supported ? "true" : "false")
      << ",\"interop_annotation_source_supported\":"
      << (summary.interop_annotation_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"swift_name_annotation_sites\":"
      << summary.swift_name_annotation_sites
      << ",\"swift_private_annotation_sites\":"
      << summary.swift_private_annotation_sites
      << ",\"cpp_name_annotation_sites\":"
      << summary.cpp_name_annotation_sites
      << ",\"header_name_annotation_sites\":"
      << summary.header_name_annotation_sites
      << ",\"abi_alignment_annotation_sites\":"
      << summary.abi_alignment_annotation_sites
      << ",\"foreign_type_annotation_sites\":"
      << summary.foreign_type_annotation_sites
      << ",\"interop_metadata_annotation_sites\":"
      << summary.interop_metadata_annotation_sites
      << ",\"named_annotation_payload_sites\":"
      << summary.named_annotation_payload_sites
      << ",\"swift_annotation_source_supported\":"
      << (summary.swift_annotation_source_supported ? "true" : "false")
      << ",\"cpp_annotation_source_supported\":"
      << (summary.cpp_annotation_source_supported ? "true" : "false")
      << ",\"interop_metadata_source_supported\":"
      << (summary.interop_metadata_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\"}";
  return out.str();
}

std::string BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
    const Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"failure_model\":\"" << EscapeJsonString(summary.failure_model)
      << "\",\"dependency_contract_ids\":"
      << BuildStringArrayJson(summary.dependency_contract_ids)
      << ",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"advanced_feature_family_count\":"
      << summary.advanced_feature_family_count
      << ",\"dependency_surface_count\":"
      << summary.dependency_surface_count
      << ",\"aggregated_source_only_claim_count\":"
      << summary.aggregated_source_only_claim_count
      << ",\"fail_closed_construct_count\":"
      << summary.fail_closed_construct_count
      << ",\"diagnostic_surface_sites\":"
      << summary.diagnostic_surface_sites
      << ",\"fixit_surface_sites\":" << summary.fixit_surface_sites
      << ",\"migrator_surface_sites\":" << summary.migrator_surface_sites
      << ",\"canonicalization_hint_sites\":"
      << summary.canonicalization_hint_sites
      << ",\"error_surface_sites\":" << summary.error_surface_sites
      << ",\"concurrency_surface_sites\":"
      << summary.concurrency_surface_sites
      << ",\"system_surface_sites\":" << summary.system_surface_sites
      << ",\"dispatch_surface_sites\":" << summary.dispatch_surface_sites
      << ",\"metaprogramming_surface_sites\":"
      << summary.metaprogramming_surface_sites
      << ",\"interop_surface_sites\":" << summary.interop_surface_sites
      << ",\"diagnostics_inventory_source_supported\":"
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ",\"fixit_inventory_source_supported\":"
      << (summary.fixit_inventory_source_supported ? "true" : "false")
      << ",\"migrator_inventory_source_supported\":"
      << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
    const Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"completion_model\":\""
      << EscapeJsonString(summary.completion_model)
      << "\",\"failure_model\":\""
      << EscapeJsonString(summary.failure_model)
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"language_profile\":\""
      << EscapeJsonString(summary.language_profile)
      << "\",\"canonical_literal_rejection_diagnostics_enabled\":"
      << (summary.canonical_literal_rejection_diagnostics_enabled ? "true" : "false")
      << ",\"canonical_literal_rejection_diagnostics_required\":"
      << (summary.canonical_literal_rejection_diagnostics_required ? "true" : "false")
      << ",\"legacy_yes_sites\":" << summary.legacy_yes_sites
      << ",\"legacy_no_sites\":" << summary.legacy_no_sites
      << ",\"legacy_null_sites\":" << summary.legacy_null_sites
      << ",\"legacy_total_sites\":" << summary.legacy_total_sites
      << ",\"canonical_true_rewrite_sites\":"
      << summary.canonical_true_rewrite_sites
      << ",\"canonical_false_rewrite_sites\":"
      << summary.canonical_false_rewrite_sites
      << ",\"canonical_nil_rewrite_sites\":"
      << summary.canonical_nil_rewrite_sites
      << ",\"canonicalization_candidate_sites\":"
      << summary.canonicalization_candidate_sites
      << ",\"fixit_candidate_sites\":" << summary.fixit_candidate_sites
      << ",\"migrator_candidate_sites\":"
      << summary.migrator_candidate_sites
      << ",\"dependency_inventory_ready\":"
      << (summary.dependency_inventory_ready ? "true" : "false")
      << ",\"canonicalization_surface_supported\":"
      << (summary.canonicalization_surface_supported ? "true" : "false")
      << ",\"fixit_migration_surface_supported\":"
      << (summary.fixit_migration_surface_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
    const Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"portability_model\":\""
      << EscapeJsonString(summary.portability_model)
      << "\",\"portability_dependency_surface_ids\":"
      << BuildStringArrayJson(summary.portability_dependency_surface_ids)
      << ",\"diagnostic_namespace\":\""
      << EscapeJsonString(summary.diagnostic_namespace)
      << "\",\"portability_dependency_count\":"
      << summary.portability_dependency_count
      << ",\"diagnostics_total\":" << summary.diagnostics_total
      << ",\"diagnostics_after_pass_final\":"
      << summary.diagnostics_after_pass_final
      << ",\"diagnostics_emitted_total\":"
      << summary.diagnostics_emitted_total
      << ",\"ownership_arc_diagnostic_candidate_sites\":"
      << summary.ownership_arc_diagnostic_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << summary.ownership_arc_fixit_available_sites
      << ",\"ownership_arc_profiled_sites\":"
      << summary.ownership_arc_profiled_sites
      << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ",\"ownership_arc_contract_violation_sites\":"
      << summary.ownership_arc_contract_violation_sites
      << ",\"migration_canonicalization_candidate_sites\":"
      << summary.migration_canonicalization_candidate_sites
      << ",\"migration_surface_ready\":"
      << (summary.migration_surface_ready ? "true" : "false")
      << ",\"portability_dependencies_frozen\":"
      << (summary.portability_dependencies_frozen ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"recovery_replay_key\":\""
      << EscapeJsonString(summary.recovery_replay_key)
      << "\",\"arc_diagnostics_fixit_replay_key\":\""
      << EscapeJsonString(summary.arc_diagnostics_fixit_replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
    const Objc3ToolingFeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(summary.frontend_surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"fixit_family_ids\":"
      << BuildStringArrayJson(summary.fixit_family_ids)
      << ",\"fixit_family_count\":" << summary.fixit_family_count
      << ",\"migration_fixit_candidate_sites\":"
      << summary.migration_fixit_candidate_sites
      << ",\"migrator_candidate_sites\":" << summary.migrator_candidate_sites
      << ",\"ownership_arc_fixit_available_sites\":"
      << summary.ownership_arc_fixit_available_sites
      << ",\"ownership_arc_empty_fixit_hint_sites\":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ",\"diagnostic_taxonomy_ready\":"
