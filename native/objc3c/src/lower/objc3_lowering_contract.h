#pragma once

#include "lower/core/lowering_primitive_ops.h"
#include "lower/contracts/arc_boundary_lowering_contracts.h"
#include "lower/contracts/block_arc_lowering_plan.h"
#include "lower/contracts/block_runtime_lowering_contracts.h"
#include "lower/contracts/cross_module_lowering_contracts.h"
#include "lower/contracts/diagnostic_recovery_lowering_contracts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "lower/contracts/error_handling_lowering_contracts.h"
#include "lower/contracts/executable_layout_lowering_contracts.h"
#include "lower/contracts/function_method_lowering_state.h"
#include "lower/contracts/interop_lowering_contracts.h"
#include "lower/contracts/lowering_concurrency_contracts.h"
#include "lower/contracts/lowering_diagnostics.h"
#include "lower/contracts/lowering_phase_io.h"
#include "lower/contracts/metaprogramming_lowering_contracts.h"
#include "lower/contracts/optional_control_flow_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_lowering_contracts.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "lower/contracts/runtime_dispatch_lowering_contracts.h"
#include "lower/contracts/runtime_bootstrap_lowering_contracts.h"
#include "lower/contracts/runtime_metadata_emission_contracts.h"
#include "lower/contracts/runtime_metadata_handoff.h"
#include "lower/contracts/runtime_object_support_contracts.h"
#include "lower/contracts/type_system_generic_lowering_contracts.h"
#include "lower/contracts/unsafe_intrinsic_governance_contracts.h"

#include <cstddef>
#include <string>

// binary inspection harness expansion anchor: lane-C now freezes one
// emitted-metadata inspection corpus over llvm-readobj/llvm-objdump so every
// currently emitted metadata section family can be asserted structurally from
// produced objects, including fail-closed negative gating when metadata
// compilation stops before object emission.
inline constexpr const char *kObjc3RuntimeBinaryInspectionHarnessContractId =
    "objc3c.runtime.binary.inspection.harness.v1";
inline constexpr const char *kObjc3RuntimeBinaryInspectionPositiveCorpusModel =
    "positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks";
inline constexpr const char *kObjc3RuntimeBinaryInspectionNegativeCorpusModel =
    "negative-compile-failure-gating-with-no-object-inspection";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSectionCommand =
    "llvm-readobj --sections module.obj";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSymbolCommand =
    "llvm-objdump --syms module.obj";
// object-packaging/retention freeze anchor: lane-D now freezes the
// current produced-object boundary around module.obj plus retained aggregate
// metadata symbols. Later archive/link/startup-registration work must preserve
// these anchors instead of redefining the object boundary ad hoc.
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionContractId =
    "objc3c.runtime.object.packaging.retention.boundary.v1";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionBoundaryModel =
    "current-object-file-boundary-with-retained-metadata-section-aggregates";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionAnchorModel =
    "llvm.used-plus-aggregate-section-symbols";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionArtifact =
    "module.obj";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionSymbolPrefix =
    "__objc3_sec_";
// linker-retention/dead-strip resistance anchor: lane-D now adds one
// public linker anchor plus one public discovery root over the retained
// metadata aggregates. The compiler publishes a driver-friendly linker-response
// file and a discovery JSON sidecar so single-library packaging can survive
// dead stripping without claiming the later multi-archive/TU edge cases.
inline constexpr const char *kObjc3RuntimeLinkerRetentionContractId =
    "objc3c.runtime.linker.retention.and.dead.strip.resistance.v1";
inline constexpr const char *kObjc3RuntimeLinkerRetentionAnchorModel =
    "public-linker-anchor-rooted-in-discovery-table";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryModel =
    "public-discovery-root-over-retained-metadata-aggregates";
inline constexpr const char *kObjc3RuntimeLinkerResponseArtifactSuffix =
    ".runtime-metadata-linker-options.rsp";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryArtifactSuffix =
    ".runtime-metadata-discovery.json";
inline constexpr const char *kObjc3RuntimeLinkerAnchorLogicalSection =
    "objc3.runtime.linker_anchor";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryRootLogicalSection =
    "objc3.runtime.discovery_root";
inline constexpr const char *kObjc3RuntimeLinkerRetentionCoffFlagModel =
    "-Wl,/include:<symbol>";
inline constexpr const char *kObjc3RuntimeLinkerRetentionElfFlagModel =
    "-Wl,--undefined=<symbol>";
inline constexpr const char *kObjc3RuntimeLinkerRetentionMachOFlagModel =
    "-Wl,-u,_<symbol>";
// versioned conformance-report lowering freeze anchor: lane-C
// lowers the truthful runnable/source-only/unsupported claim packets into one
// emitted machine-readable sidecar artifact. Later runtime capability and
// driver publication issues must preserve this versioned lowering boundary
// rather than reconstructing claim truth ad hoc from docs or release evidence.
inline constexpr const char *kObjc3VersionedConformanceReportLoweringContractId =
    "objc3c.versioned.conformance.report.lowering.v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringSemanticContractId =
        "objc3c.compatibility.strictness.claim.semantics.v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringArtifactSuffix =
        ".objc3-conformance-report.json";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringArtifactSchemaId =
        "objc3c-versioned-conformance-report-v1";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringSurfacePath =
        "frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringPayloadModel =
        "frontend-truth-packets-lower-into-one-versioned-machine-readable-conformance-sidecar";
inline constexpr const char
    *kObjc3VersionedConformanceReportLoweringAuthorityModel =
        "runnable-feature-inventory-plus-truth-surface-plus-fail-closed-semantics";
inline constexpr const char
    *kObjc3VersionedConformanceReportKnownUnsupportedModel =
        "unsupported-claims-remain-published-as-known-unsupported-without-runnable-overclaim";
inline constexpr const char
    *kObjc3VersionedConformanceReportSelectionModel =
        "canonical-and-legacy-compatibility-selection-only-strictness-and-concurrency-claims-remain-fail-closed";
inline constexpr const char
    *kObjc3VersionedConformanceReportCanonicalInterfaceMode =
        "no-standalone-interface-payload-yet";
inline constexpr const char
    *kObjc3VersionedConformanceReportPublicationModel =
        "written-next-to-manifest-when-out-dir-is-present";
// runtime capability reporting anchor: lane-C turns the versioned
// conformance sidecar into a truthful machine-readable capability and public
// conformance-report payload that later driver/CLI publication lanes can emit
// directly without inventing a second truth surface.
inline constexpr const char *kObjc3RuntimeCapabilityReportingContractId =
    "objc3c.runtime.capability.reporting.v1";
inline constexpr const char *kObjc3RuntimeCapabilityReportingSchemaId =
    "objc3c-runtime-capability-report-v1";
inline constexpr const char *kObjc3RuntimeCapabilityReportingSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_capability_report";
inline constexpr const char *kObjc3RuntimeCapabilityReportingProfileModel =
    "core-and-strict-profiles-claimed-while-optional-feature-gaps-remain-not-claimed-until-runtime-backed";
inline constexpr const char *kObjc3RuntimeCapabilityReportingOptionalFeatureModel =
    "unsupported-runtime-feature-ids-lower-into-not-claimed-public-optional-features";
inline constexpr const char *kObjc3RuntimeCapabilityReportingVersionModel =
    "deterministic-dev-version-surface-for-frontend-runtime-stdlib-and-module-format";
inline constexpr const char *kObjc3RuntimeCapabilityPublicSchemaId =
    "objc3-conformance-report/v1";
inline constexpr const char *kObjc3RuntimeCapabilityGeneratedAtReplayValue =
    "1970-01-01T00:00:00Z";
inline constexpr const char *kObjc3RuntimeCapabilityToolchainName = "objc3c";
inline constexpr const char *kObjc3RuntimeCapabilityToolchainVendor =
    "doublemover";
inline constexpr const char *kObjc3RuntimeCapabilityToolchainVersion =
    "0.0.0-dev";
inline constexpr const char *kObjc3RuntimeCapabilityTargetTriple =
    "x86_64-pc-windows-msvc";
inline constexpr const char *kObjc3RuntimeCapabilityLanguageFamily =
    "objective-c";
inline constexpr const char *kObjc3RuntimeCapabilityLanguageVersion = "3.0";
inline constexpr const char *kObjc3RuntimeCapabilitySpecRevision = "v1";
inline constexpr const char *kObjc3RuntimeCapabilityStrictnessMode =
    "permissive";
inline constexpr const char *kObjc3RuntimeCapabilityConcurrencyMode = "off";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportContractId =
        "objc3c.tooling.machine.readable.conformance.report.contract.v1";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportDependencyContractId =
        "objc3c.tooling.legacy.canonical.migration.semantics.v1";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_machine_readable_conformance_report_contract";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportPayloadModel =
        "tooling-advanced-feature-truth-reuses-the-existing-versioned-conformance-sidecar-and-runtime-capability-publication-path";
inline constexpr const char
    *kObjc3ToolingMachineReadableConformanceReportAuthorityModel =
        "tooling-machine-readable-reporting-remains-bounded-to-the-lowered-versioned-conformance-sidecar-and-live-migration-semantics";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionDependencyContractId =
        "objc3c.tooling.machine.readable.conformance.report.contract.v1";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_feature_aware_conformance_report_emission";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-feature-aware-migration-and-fixit-state-without-introducing-a-second-report-sidecar";
inline constexpr const char
    *kObjc3ToolingFeatureAwareConformanceReportEmissionAuthorityModel =
        "tooling-feature-aware-reporting-remains-bounded-to-live-fixit-migration-and-machine-readable-report-contract-surfaces";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingContractId =
        "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId =
        "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_tooling_corpus_sharding_release_evidence_packaging";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel =
        "versioned-conformance-report-now-embeds-tooling-corpus-shard-and-release-evidence-packaging-without-introducing-a-parallel-report-format";
inline constexpr const char
    *kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel =
        "tooling-release-evidence-packaging-remains-bounded-to-emitted-report-payloads-checklist-refs-and-stable-conformance-bucket-manifests";
inline constexpr const char *kObjc3RuntimeCapabilityModuleFormatVersion =
    "objc3c-runtime-metadata-v1";
// manifest/object/IR truth gate anchor: this binds the compiler sidecar
// manifest, emitted LLVM IR, native object, runtime registration artifacts,
// and release-claim sidecars into one deterministic evidence boundary.
inline constexpr const char *kObjc3ManifestObjectIrTruthGateContractId =
    "objc3c.manifest.object.ir.truth.gate.v1";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateEvidenceModel =
    "manifest-ir-object-registration-and-conformance-sidecars-form-one-regenerated-truth-set";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateManifestModel =
    "module.manifest.json-publishes-the-semantic-lowering-runtime-metadata-and-replay-key-source-of-truth";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateIrModel =
    "module.ll-republishes-the-same-contract-boundaries-and-runtime-registration-roots-as-reviewable-ir-evidence";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateObjectModel =
    "module.obj-materializes-the-same-objc3-runtime-sections-symbols-and-registration-roots-observed-in-ir";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateClaimModel =
    "versioned-conformance-and-runtime-capability-sidecars-are-bound-to-the-same-replay-key-and-remain-narrower-than-evidence";
inline constexpr const char *kObjc3ManifestObjectIrTruthGateFailureModel =
    "missing-artifact-hash-drift-object-section-drift-or-unsupported-negative-emission-fails-closed";
inline constexpr const char *kObjc3MethodLookupOverrideConflictLaneContract =
    "objc3c.method.lookup.override.conflict.v1";
inline constexpr const char *kObjc3PropertySynthesisIvarBindingLaneContract =
    "objc3c.property.synthesis.ivar.binding.v1";
inline constexpr const char *kObjc3IdClassSelObjectPointerTypecheckLaneContract =
    "objc3c.id.class.sel.object.pointer.typecheck.v1";
inline constexpr const char *kObjc3MessageSendSelectorLoweringLaneContract =
    "objc3c.message.send.selector.lowering.v1";
inline constexpr const char *kObjc3DispatchAbiMarshallingLaneContract =
    "objc3c.dispatch.abi.marshalling.v1";
inline constexpr const char *kObjc3NilReceiverSemanticsFoldabilityLaneContract =
    "objc3c.nil.receiver.semantics.foldability.v1";
inline constexpr const char *kObjc3SuperDispatchMethodFamilyLaneContract =
    "objc3c.super.dispatch.method.family.v1";
inline constexpr const char *kObjc3RuntimeLinkHostLinkLaneContract =
    "objc3c.runtime.dispatch.host.link.v1";
inline constexpr const char *kObjc3OwnershipQualifierLoweringLaneContract =
    "objc3c.ownership.qualifier.lowering.v1";
inline constexpr const char *kObjc3RetainReleaseOperationLoweringLaneContract =
    "objc3c.retain.release.operation.lowering.v1";
inline constexpr const char *kObjc3AutoreleasePoolScopeLoweringLaneContract =
    "objc3c.autoreleasepool.scope.lowering.v1";
inline constexpr const char *kObjc3WeakUnownedSemanticsLoweringLaneContract =
    "objc3c.weak.unowned.semantics.lowering.v1";
inline constexpr const char *kObjc3ArcDiagnosticsFixitLoweringLaneContract =
    "objc3c.arc.diagnostics.fixit.lowering.v1";
struct Objc3LoweringContract {
  std::size_t max_message_send_args = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
};

struct Objc3LoweringIRBoundary {
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_global_ordering = kObjc3SelectorGlobalOrdering;
};

struct Objc3MethodLookupOverrideConflictContract {
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;
};

struct Objc3PropertySynthesisIvarBindingContract {
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypecheckContract {
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t total_typecheck_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchSurfaceClassificationContract {
  std::size_t instance_dispatch_sites = 0;
  std::size_t class_dispatch_sites = 0;
  std::size_t super_dispatch_sites = 0;
  std::size_t direct_dispatch_sites = 0;
  std::size_t dynamic_dispatch_sites = 0;
  std::string instance_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string class_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string super_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string direct_entrypoint_family = kObjc3DispatchSurfaceDirectDispatchBinding;
  std::string dynamic_entrypoint_family = kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringContract {
  std::size_t message_send_sites = 0;
  std::size_t unary_selector_sites = 0;
  std::size_t keyword_selector_sites = 0;
  std::size_t selector_piece_sites = 0;
  std::size_t argument_expression_sites = 0;
  std::size_t receiver_expression_sites = 0;
  std::size_t selector_literal_entries = 0;
  std::size_t selector_literal_characters = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots_marshaled = 0;
  std::size_t selector_slots_marshaled = 0;
  std::size_t argument_value_slots_marshaled = 0;
  std::size_t argument_padding_slots_marshaled = 0;
  std::size_t argument_total_slots_marshaled = 0;
  std::size_t total_marshaled_slots = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilityContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilyContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkContract {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RuntimeDispatchLoweringAbiContract {
  std::size_t message_send_sites = 0;
  std::size_t fixed_argument_slot_count = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_parameter_count = 0;
  std::string lowering_boundary_model =
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel;
  std::string canonical_runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;
  std::string default_lowering_target_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol;
  std::string selector_handle_type =
      kObjc3RuntimeDispatchLoweringSelectorHandleType;
  std::string receiver_abi_type = kObjc3RuntimeDispatchLoweringReceiverAbiType;
  std::string selector_abi_type = kObjc3RuntimeDispatchLoweringSelectorAbiType;
  std::string argument_abi_type = kObjc3RuntimeDispatchLoweringArgumentAbiType;
  std::string result_abi_type = kObjc3RuntimeDispatchLoweringResultAbiType;
  std::string selector_operand_model =
      kObjc3RuntimeDispatchLoweringSelectorOperandModel;
  std::string selector_handle_model =
      kObjc3RuntimeDispatchLoweringSelectorHandleModel;
  std::string argument_padding_model =
      kObjc3RuntimeDispatchLoweringArgumentPaddingModel;
  std::string default_lowering_target_model =
      kObjc3RuntimeDispatchLoweringDefaultTargetModel;
  std::string strict_dispatch_error_model =
      kObjc3RuntimeDispatchLoweringStrictDispatchErrorModel;
  std::string deferred_cases_model =
      kObjc3RuntimeDispatchLoweringDeferredCasesModel;
  bool fail_closed = true;
  bool deterministic = true;
};

struct Objc3OwnershipQualifierLoweringContract {
  std::size_t ownership_qualifier_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_annotation_sites = 0;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationLoweringContract {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeLoweringContract {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  unsigned max_scope_depth = 0;
  std::size_t scope_entry_transition_sites = 0;
  std::size_t scope_exit_transition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsLoweringContract {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitLoweringContract {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidRuntimeDispatchSymbol(const std::string &symbol);
bool TryNormalizeObjc3LoweringContract(const Objc3LoweringContract &input,
                                       Objc3LoweringContract &normalized,
                                       std::string &error);
bool TryBuildObjc3LoweringIRBoundary(const Objc3LoweringContract &input,
                                     Objc3LoweringIRBoundary &boundary,
                                     std::string &error);
std::string Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary);
bool UsesCanonicalObjc3RuntimeDispatchEntrypoint(
    const std::string &dispatch_surface_family);
bool RequiresFailClosedObjc3RuntimeDispatchError(
    const std::string &dispatch_surface_family);
const char *Objc3DispatchSurfaceRuntimeEntrypointSymbol(
    const std::string &dispatch_surface_family);
std::string Objc3RuntimeDispatchDeclarationReplayKey(const Objc3LoweringIRBoundary &boundary);
std::string Objc3ManifestObjectIrTruthGateSummary();
std::string Objc3ToolingMachineReadableConformanceReportContractLoweringSummary();
std::string Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary();
std::string Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary();
std::string Objc3VersionedConformanceReportLoweringContractSummary();
std::string Objc3RuntimeCapabilityReportingContractSummary();
bool IsValidObjc3MethodLookupOverrideConflictContract(const Objc3MethodLookupOverrideConflictContract &contract);
std::string Objc3MethodLookupOverrideConflictReplayKey(const Objc3MethodLookupOverrideConflictContract &contract);
Objc3PropertySynthesisIvarBindingContract Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic = true);
bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract);
std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract);
bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
bool IsValidObjc3DispatchSurfaceClassificationContract(
    const Objc3DispatchSurfaceClassificationContract &contract);
std::string Objc3DispatchSurfaceClassificationReplayKey(
    const Objc3DispatchSurfaceClassificationContract &contract);
bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract);
std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract);
bool IsValidObjc3DispatchAbiMarshallingContract(
    const Objc3DispatchAbiMarshallingContract &contract);
std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract);
bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract);
std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract);
bool IsValidObjc3RuntimeLinkHostLinkContract(
    const Objc3RuntimeLinkHostLinkContract &contract);
std::string Objc3RuntimeLinkHostLinkReplayKey(
    const Objc3RuntimeLinkHostLinkContract &contract);
bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
bool IsValidObjc3OwnershipQualifierLoweringContract(
    const Objc3OwnershipQualifierLoweringContract &contract);
std::string Objc3OwnershipQualifierLoweringReplayKey(
    const Objc3OwnershipQualifierLoweringContract &contract);
bool IsValidObjc3RetainReleaseOperationLoweringContract(
    const Objc3RetainReleaseOperationLoweringContract &contract);
std::string Objc3RetainReleaseOperationLoweringReplayKey(
    const Objc3RetainReleaseOperationLoweringContract &contract);
bool IsValidObjc3AutoreleasePoolScopeLoweringContract(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
std::string Objc3AutoreleasePoolScopeLoweringReplayKey(
    const Objc3AutoreleasePoolScopeLoweringContract &contract);
bool IsValidObjc3WeakUnownedSemanticsLoweringContract(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
std::string Objc3WeakUnownedSemanticsLoweringReplayKey(
    const Objc3WeakUnownedSemanticsLoweringContract &contract);
bool IsValidObjc3ArcDiagnosticsFixitLoweringContract(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
std::string Objc3ArcDiagnosticsFixitLoweringReplayKey(
    const Objc3ArcDiagnosticsFixitLoweringContract &contract);
