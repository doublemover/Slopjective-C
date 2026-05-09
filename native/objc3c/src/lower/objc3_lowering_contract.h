#pragma once

#include "lower/core/lowering_primitive_ops.h"
#include "lower/contracts/arc_boundary_lowering_contracts.h"
#include "lower/contracts/block_arc_lowering_plan.h"
#include "lower/contracts/block_runtime_lowering_contracts.h"
#include "lower/contracts/dispatch_control_lowering_contracts.h"
#include "lower/contracts/error_handling_lowering_contracts.h"
#include "lower/contracts/function_method_lowering_state.h"
#include "lower/contracts/interop_lowering_contracts.h"
#include "lower/contracts/lowering_concurrency_contracts.h"
#include "lower/contracts/lowering_diagnostics.h"
#include "lower/contracts/lowering_phase_io.h"
#include "lower/contracts/metaprogramming_lowering_contracts.h"
#include "lower/contracts/ownership_runtime_lowering_contracts.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "lower/contracts/runtime_dispatch_lowering_contracts.h"
#include "lower/contracts/runtime_bootstrap_lowering_contracts.h"
#include "lower/contracts/runtime_metadata_emission_contracts.h"
#include "lower/contracts/runtime_metadata_handoff.h"

#include <cstddef>
#include <string>

// executable object artifact lowering freeze anchor: lane-C now
// freezes the current binding surface where realized class/category metadata
// records consume owner-scoped method-list refs and implementation-backed
// method entries may point at concrete LLVM definition symbols. Later
// implementation must extend this one executable object surface rather than
// rediscovering bodies or realization edges out-of-band.
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringContractId =
    "objc3c.executable.object.artifact.lowering.v1";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodBodyBindingModel =
        "implementation-owner-identity-to-llvm-definition-symbol";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringRealizationRecordModel =
        "class-metaclass-and-category-descriptor-bundles-point-to-owner-scoped-method-list-ref-records";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringMethodEntryPayloadModel =
        "selector-owner-return-arity-implementation-symbol-has-body-direct-flag-final-flag";
inline constexpr const char *kObjc3ExecutableObjectArtifactLoweringScopeModel =
    "parser-source-identities-sema-realization-closure-ir-object-binding";
inline constexpr const char
    *kObjc3ExecutableObjectArtifactLoweringFailClosedModel =
        "no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families";
// accessor/layout lowering freeze anchor: lane-C now freezes the
// current property/ivar lowering surface where sema-approved property
// descriptor bundles, ivar layout symbols/slots/sizes/alignment, and
// synthesized binding identities are serialized into emitted metadata/object
// artifacts without yet synthesizing accessor bodies or runtime storage.
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringContractId =
        "objc3c.executable.property.accessor.layout.lowering.v1";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringPropertyTableModel =
        "property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringIvarLayoutModel =
        "ivar-descriptor-bundles-carry-sema-approved-layout-symbol-replay-key-slot-offset-size-alignment-padding-inheritance-owner-size-records";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringAccessorBindingModel =
        "effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringScopeModel =
        "ast-sema-property-layout-handoff-ir-object-metadata-publication";
inline constexpr const char
    *kObjc3ExecutablePropertyAccessorLayoutLoweringFailClosedModel =
        "no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation";
// ivar offset/layout emission anchor: lane-C extends the frozen
// accessor/layout handoff into real object payloads by emitting per-ivar
// offset globals, per-owner layout tables, and descriptor records that carry
// the sema-approved slot/offset/size/alignment tuple without yet allocating
// runtime instances or synthesizing accessor bodies.
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionContractId =
    "objc3c.executable.ivar.layout.emission.v1";
inline constexpr const char *kObjc3ExecutableIvarLayoutDescriptorModel =
    "ivar-descriptor-records-carry-layout-symbol-replay-key-offset-global-slot-offset-size-alignment-padding-inheritance-owner-size-ordering";
inline constexpr const char *kObjc3ExecutableIvarOffsetGlobalModel =
    "one-retained-i64-offset-global-per-emitted-ivar-binding";
inline constexpr const char *kObjc3ExecutableIvarLayoutTableModel =
    "declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size";
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionScopeModel =
    "sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation";
inline constexpr const char *kObjc3ExecutableIvarLayoutEmissionFailClosedModel =
    "no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis";
// synthesized accessor/property lowering anchor: lane-C upgrades the
// frozen property/layout handoff into executable accessor support by
// materializing missing implementation-owned getter/setter method entries,
// emitting deterministic storage globals keyed by synthesized binding symbols,
// and widening property descriptor payloads with effective accessor and layout
// attachment records while still deferring true runtime instance allocation to
// later lane-D work.
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId =
        "objc3c.executable.synthesized.accessor.property.lowering.v1";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel =
        "implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel =
        "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel =
        "property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers";
inline constexpr const char
    *kObjc3ExecutableSynthesizedAccessorPropertyLoweringFailClosedModel =
        "no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-shared-storage-bypasses";
// runtime property/layout consumption freeze anchor: lane-D now
// freezes the truthful runtime boundary above C003. Runtime consumes emitted
// synthesized accessor implementation pointers plus property/layout attachment
// records through the existing lookup/dispatch ABI, but alloc/new still
// materialize one canonical realized instance identity per class and accessor
// execution still uses the lane-C storage globals until D002 introduces real
// per-instance slot allocation.
inline constexpr const char *kObjc3RuntimePropertyLayoutConsumptionContractId =
    "objc3c.runtime.property.layout.consumption.freeze.v1";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionDescriptorModel =
        "runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionAllocatorModel =
        "alloc-new-consume-realized-class-layout-and-handoff-directly-to-runtime-owned-instance-slot-allocation";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionStorageModel =
        "synthesized-accessor-execution-consumes-runtime-owned-per-instance-slots-selected-by-the-dispatch-frame-property-context";
inline constexpr const char
    *kObjc3RuntimePropertyLayoutConsumptionFailClosedModel =
        "no-layout-rederivation-no-shared-storage-bypasses-no-reflective-property-registration";
// instance-allocation-layout-runtime anchor: lane-D upgrades the
// frozen D001 runtime-consumption boundary into true per-instance allocation
// backed by realized class layout, emitted ivar offsets, and runtime-owned slot
// storage without reopening source-driven layout recovery.
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportContractId =
        "objc3c.runtime.instance.allocation.layout.support.v1";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportDescriptorModel =
        "runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportAllocatorModel =
        "alloc-new-materialize-distinct-runtime-instance-identities-backed-by-realized-class-layout";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportStorageModel =
        "synthesized-accessor-execution-reads-and-writes-per-instance-slot-storage-using-emitted-ivar-offset-layout-records";
inline constexpr const char
    *kObjc3RuntimeInstanceAllocationLayoutSupportFailClosedModel =
        "no-layout-rederivation-no-shared-global-property-storage-no-reflective-property-registration-yet";
// property-metadata-reflection anchor: lane-D now freezes the
// private reflective helper surface over the realized property metadata graph
// so diagnostics and tests can query property/accessor/layout facts without
// widening the public runtime ABI or rediscovering metadata from source.
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionContractId =
        "objc3c.runtime.property.metadata.reflection.v1";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionRegistrationModel =
        "runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionQueryModel =
        "private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts";
inline constexpr const char
    *kObjc3RuntimePropertyMetadataReflectionFailClosedModel =
        "no-public-reflection-abi-no-reflective-source-recovery-no-property-query-success-without-realized-runtime-layout";
inline constexpr const char
    *kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId =
        "objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1";
inline constexpr const char
    *kObjc3AccessorStorageLoweringMetadataModel =
        "runtime-metadata-and-executable-graph-property-records-publish-synthesized-accessor-lowering-helper-selection-through-the-live-compiler-path";
inline constexpr const char
    *kObjc3AccessorStorageLoweringHelperSelectionModel =
        "plain-accessors-use-current-property-read-write-helpers-strong-owned-setters-use-exchange-and-weak-accessors-use-weak-current-property-helpers";
// executable method-body binding implementation anchor: lane-C now
// hardens the existing executable object surface so implementation-owned
// method entries must bind to exactly one concrete LLVM definition symbol and
// object emission fails closed when that attachment is missing or ambiguous.
inline constexpr const char *kObjc3ExecutableMethodBodyBindingContractId =
    "objc3c.executable.method.body.binding.v1";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingSourceModel =
    "implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingRuntimeModel =
    "emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3ExecutableMethodBodyBindingFailClosedModel =
    "error-on-missing-or-duplicate-implementation-binding";
// executable realization-record expansion anchor: lane-C extends the
// executable object surface with realization-ready class, protocol, and
// category records that preserve the frontend-owned owner/super/adoption edges
// directly in emitted artifacts instead of forcing later runtime work to
// recover them out-of-band.
inline constexpr const char *kObjc3ExecutableRealizationRecordsContractId =
    "objc3c.executable.realization.records.v1";
inline constexpr const char *kObjc3ExecutableRealizationClassRecordModel =
    "class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs";
inline constexpr const char *kObjc3ExecutableRealizationProtocolRecordModel =
    "protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts";
inline constexpr const char *kObjc3ExecutableRealizationCategoryRecordModel =
    "category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges";
inline constexpr const char *kObjc3ExecutableRealizationFailClosedModel =
    "no-identity-edge-elision-no-out-of-band-graph-reconstruction";
// class-realization-runtime freeze anchor: lane-D now freezes the
// current runtime-owned class realization surface that consumes emitted
// realization records, walks class/metaclass chains, attaches preferred
// category implementation records, and uses protocol records as
// declaration-aware negative lookup evidence only.
inline constexpr const char *kObjc3RuntimeClassRealizationContractId =
    "objc3c.runtime.class.realization.freeze.v1";
inline constexpr const char *kObjc3RuntimeClassRealizationModel =
    "registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name";
inline constexpr const char *kObjc3RuntimeMetaclassGraphModel =
    "known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain";
inline constexpr const char *kObjc3RuntimeClassRealizationCategoryAttachmentModel =
    "preferred-category-implementation-records-attach-after-class-bundle-resolution";
inline constexpr const char *kObjc3RuntimeProtocolCheckModel =
    "adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks";
inline constexpr const char *kObjc3RuntimeClassRealizationFailClosedModel =
    "invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed";
// metaclass-graph-root-class anchor: lane-D now promotes the frozen
// D001 runtime boundary into a runtime-owned realized class graph keyed by
// stable receiver identities, with explicit root-class publication and
// metaclass-edge inventory available to later runtime lanes.
inline constexpr const char *kObjc3RuntimeMetaclassGraphRootClassContractId =
    "objc3c.runtime.metaclass.graph.root.class.baseline.v1";
inline constexpr const char *kObjc3RuntimeRealizedClassGraphModel =
    "runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records";
inline constexpr const char *kObjc3RuntimeRootClassBaselineModel =
    "root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch";
inline constexpr const char *kObjc3RuntimeRealizedClassGraphFailClosedModel =
    "missing-receiver-bindings-or-broken-realized-superclass-links-publish-strict-dispatch-error";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId =
    "objc3c.runtime.category.attachment.protocol.conformance.v1";
inline constexpr const char *kObjc3RuntimeCategoryAttachmentRealizedGraphModel =
    "realized-class-nodes-own-preferred-category-attachments-after-registration";
inline constexpr const char *kObjc3RuntimeProtocolConformanceQueryModel =
    "runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures";
inline constexpr const char *kObjc3RuntimeAttachmentConformanceFailClosedModel =
    "invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId =
    "objc3c.runtime.canonical.runnable.object.sample.support.v1";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectExecutionModel =
    "canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel =
    "metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits";
inline constexpr const char *kObjc3RuntimeCanonicalRunnableObjectFailClosedModel =
    "metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open";
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
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringLaneContract =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringLaneContract =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3LightweightGenericsConstraintLoweringLaneContract =
    "objc3c.lightweight.generics.constraint.lowering.v1";
inline constexpr const char *kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract =
    "objc3c.nullability.flow.warning.precision.lowering.v1";
inline constexpr const char *kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract =
    "objc3c.protocol.qualified.object.type.lowering.v1";
inline constexpr const char *kObjc3VarianceBridgeCastLoweringLaneContract =
    "objc3c.variance.bridge.cast.lowering.v1";
inline constexpr const char *kObjc3GenericMetadataAbiLoweringLaneContract =
    "objc3c.generic.metadata.abi.lowering.v1";
inline constexpr const char *kObjc3ModuleImportGraphLoweringLaneContract =
    "objc3c.module.import.graph.lowering.v1";
inline constexpr const char *kObjc3NamespaceCollisionShadowingLoweringLaneContract =
    "objc3c.namespace.collision.shadowing.lowering.v1";
inline constexpr const char *kObjc3PublicPrivateApiPartitionLoweringLaneContract =
    "objc3c.public.private.api.partition.lowering.v1";
inline constexpr const char *kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract =
    "objc3c.incremental.module.cache.invalidation.lowering.v1";
inline constexpr const char *kObjc3CrossModuleConformanceLoweringLaneContract =
    "objc3c.cross.module.conformance.lowering.v1";
inline constexpr const char *kObjc3ErrorDiagnosticsRecoveryLoweringLaneContract =
    "objc3c.error.diagnostics.recovery.lowering.v1";
inline constexpr const char *kObjc3UnsafePointerExtensionLoweringLaneContract =
    "objc3c.unsafe.pointer.extension.gating.lowering.v1";
inline constexpr const char *kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract =
    "objc3c.inline.asm.intrinsic.governance.lowering.v1";

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

struct Objc3TypeSystemOptionalKeypathLoweringContract {
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t live_optional_lowering_sites = 0;
  std::size_t single_evaluation_nil_short_circuit_sites = 0;
  std::size_t live_typed_keypath_artifact_sites = 0;
  std::size_t deferred_typed_keypath_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ControlFlowControlFlowSafetyLoweringContract {
  std::size_t guard_statement_sites = 0;
  std::size_t guard_clause_sites = 0;
  std::size_t match_statement_sites = 0;
  std::size_t defer_statement_sites = 0;
  std::size_t live_guard_short_circuit_sites = 0;
  std::size_t live_match_dispatch_sites = 0;
  std::size_t live_defer_cleanup_sites = 0;
  std::size_t fail_closed_guard_short_circuit_sites = 0;
  std::size_t fail_closed_match_dispatch_sites = 0;
  std::size_t fail_closed_defer_cleanup_sites = 0;
  std::size_t deterministic_fail_closed_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3LightweightGenericsConstraintLoweringContract {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NullabilityFlowWarningPrecisionLoweringContract {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ProtocolQualifiedObjectTypeLoweringContract {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3VarianceBridgeCastLoweringContract {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3GenericMetadataAbiLoweringContract {
  std::size_t generic_metadata_abi_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ModuleImportGraphLoweringContract {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingLoweringContract {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionLoweringContract {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3IncrementalModuleCacheInvalidationLoweringContract {
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3CrossModuleConformanceLoweringContract {
  std::size_t cross_module_conformance_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ErrorDiagnosticsRecoveryLoweringContract {
  std::size_t error_diagnostic_sites = 0;
  std::size_t parser_diagnostic_sites = 0;
  std::size_t semantic_diagnostic_sites = 0;
  std::size_t fixit_hint_sites = 0;
  std::size_t recovery_candidate_sites = 0;
  std::size_t recovery_applied_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3UnsafePointerExtensionLoweringContract {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InlineAsmIntrinsicGovernanceLoweringContract {
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
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
std::string Objc3ExecutableObjectArtifactLoweringSummary();
std::string Objc3ExecutablePropertyAccessorLayoutLoweringSummary();
std::string Objc3ExecutableIvarLayoutEmissionSummary();
std::string Objc3ExecutableSynthesizedAccessorPropertyLoweringSummary();
std::string Objc3RuntimePropertyLayoutConsumptionSummary();
std::string Objc3RuntimeInstanceAllocationLayoutSupportSummary();
std::string Objc3RuntimePropertyMetadataReflectionSummary();
// Part 3 lowering freeze anchor: native lowering now
// truthfully freezes the live optional binding/send/optional-member-access/
// coalescing path with single-evaluation nil short-circuit semantics and, as
// of the later lowering step, lowers validated typed key-path literals into retained native
// descriptor artifacts with stable runtime handles while broader key-path
// execution remains a later runtime milestone.
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringContractId =
    "objc3c.type_system.optional.keypath.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringContractId =
    "objc3c.control_flow.control.flow.safety.lowering.v1";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_control_flow_control_flow_safety_lowering_contract";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringOptionalModel =
    "optional-bindings-sends-optional-member-access-and-coalescing-lower-natively-with-single-evaluation-and-nil-short-circuit";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel =
    "validated-single-component-typed-keypath-literals-lower-to-canonical-runtime-descriptor-handles-with-generic-metadata-preservation";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel =
    "type_system-semantic-summary-plus-message-send-selector-dispatch-and-nil-receiver-lowering-contracts";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel =
    "native-lowering-fails-closed-on-lowering-contract-drift-and-on-semantically-unsupported-typed-keypath-shapes";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringGuardModel =
    "native-lowering-executes-guard-clauses-via-short-circuit-control-flow-and-else-edge-cleanup";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringMatchModel =
    "native-lowering-executes-literal-default-wildcard-and-binding-match-arms-while-result-case-patterns-remain-explicitly-fail-closed";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringDeferModel =
    "native-lowering-registers-defer-cleanups-per-scope-and-emits-lifo-cleanup-insertion-on-scope-exit";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringAuthorityModel =
    "control_flow-source-closure-plus-control_flow-semantic-model-own-the-current-lowering-boundary";
inline constexpr const char *kObjc3ControlFlowControlFlowSafetyLoweringFailClosedModel =
    "native-ir-emission-fails-closed-with-o3l300-on-result-case-match-patterns-until-a-runtime-result-payload-abi-lands";
inline constexpr const char *kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId =
    "objc3c.type_system.optional.keypath.runtime.helper.contract.v1";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_runtime_helper_contract";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel =
        "optional-send-and-optional-member-access-sites-use-lowering-owned-nil-short-circuit-plus-public-runtime-selector-lookup-dispatch";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel =
        "validated-single-component-typed-keypath-sites-publish-stable-descriptor-handles-and-retained-descriptor-sections-while-runtime-evaluation-helpers-remain-a-follow-on-private-runtime-step";
inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel =
        "unsupported-typed-keypath-shapes-and-non-objc-optional-member-access-fail-closed-before-runtime";
std::string Objc3TypeSystemOptionalKeypathLoweringSummary();
std::string Objc3ControlFlowControlFlowSafetyLoweringSummary();
std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary();
std::string Objc3ExecutableMethodBodyBindingSummary();
std::string Objc3ExecutableRealizationRecordsSummary();
std::string Objc3RuntimeClassRealizationSummary();
std::string Objc3RuntimeMetaclassGraphRootClassSummary();
std::string Objc3RuntimeCategoryAttachmentProtocolConformanceSummary();
std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary();
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
bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
bool IsValidObjc3LightweightGenericsConstraintLoweringContract(
    const Objc3LightweightGenericsConstraintLoweringContract &contract);
std::string Objc3LightweightGenericsConstraintLoweringReplayKey(
    const Objc3LightweightGenericsConstraintLoweringContract &contract);
bool IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract);
std::string Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
    const Objc3NullabilityFlowWarningPrecisionLoweringContract &contract);
bool IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract);
std::string Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
    const Objc3ProtocolQualifiedObjectTypeLoweringContract &contract);
bool IsValidObjc3VarianceBridgeCastLoweringContract(
    const Objc3VarianceBridgeCastLoweringContract &contract);
std::string Objc3VarianceBridgeCastLoweringReplayKey(
    const Objc3VarianceBridgeCastLoweringContract &contract);
bool IsValidObjc3GenericMetadataAbiLoweringContract(
    const Objc3GenericMetadataAbiLoweringContract &contract);
std::string Objc3GenericMetadataAbiLoweringReplayKey(
    const Objc3GenericMetadataAbiLoweringContract &contract);
bool IsValidObjc3ModuleImportGraphLoweringContract(
    const Objc3ModuleImportGraphLoweringContract &contract);
std::string Objc3ModuleImportGraphLoweringReplayKey(
    const Objc3ModuleImportGraphLoweringContract &contract);
bool IsValidObjc3NamespaceCollisionShadowingLoweringContract(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract);
std::string Objc3NamespaceCollisionShadowingLoweringReplayKey(
    const Objc3NamespaceCollisionShadowingLoweringContract &contract);
bool IsValidObjc3PublicPrivateApiPartitionLoweringContract(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract);
std::string Objc3PublicPrivateApiPartitionLoweringReplayKey(
    const Objc3PublicPrivateApiPartitionLoweringContract &contract);
bool IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract);
std::string Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
    const Objc3IncrementalModuleCacheInvalidationLoweringContract &contract);
bool IsValidObjc3CrossModuleConformanceLoweringContract(
    const Objc3CrossModuleConformanceLoweringContract &contract);
std::string Objc3CrossModuleConformanceLoweringReplayKey(
    const Objc3CrossModuleConformanceLoweringContract &contract);
bool IsValidObjc3ErrorDiagnosticsRecoveryLoweringContract(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract);
std::string Objc3ErrorDiagnosticsRecoveryLoweringReplayKey(
    const Objc3ErrorDiagnosticsRecoveryLoweringContract &contract);
bool IsValidObjc3UnsafePointerExtensionLoweringContract(
    const Objc3UnsafePointerExtensionLoweringContract &contract);
std::string Objc3UnsafePointerExtensionLoweringReplayKey(
    const Objc3UnsafePointerExtensionLoweringContract &contract);
bool IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract);
std::string Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract);
