#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "sema/objc3_sema_contract_bootstrap_compatibility.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_contract_feature_surfaces.h"
#include "sema/objc3_sema_contract_type_handoff_container_records.h"
#include "sema/objc3_sema_contract_type_handoff_metadata.h"

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  // diagnostic precision anchor: these maps model class containers
  // only. Category containers are validated separately so valid
  // class-plus-category programs do not collapse into duplicate class-owner
  // diagnostics before runtime metadata conflict analysis runs.
  // binary-boundary anchor: the executable metadata binary envelope
  // must consume the canonical sema-owned class/category split without
  // rebuilding container ownership from manifest-only heuristics.
  std::unordered_map<std::string, Objc3InterfaceInfo> interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo> implementations;
  std::unordered_map<std::string, Objc3InterfaceInfo> category_interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo>
      category_implementations;
  std::unordered_map<std::string, Objc3CategoryMergeInfo>
      category_merge_surfaces;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3BootstrapLegalityFailureContractSummary
      bootstrap_legality_failure_contract_summary;
  Objc3BootstrapLegalitySemanticsSummary
      bootstrap_legality_semantics_summary;
  Objc3BootstrapFailureRestartSemanticsSummary
      bootstrap_failure_restart_semantics_summary;
  Objc3CompatibilityStrictnessClaimSemanticsSummary
      compatibility_strictness_claim_semantics_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3AwaitLoweringSuspensionStateSummary await_lowering_suspension_state_lowering_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  bool built = false;
};
