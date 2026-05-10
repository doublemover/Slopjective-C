#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_semantic_passes.h"
#include "support/objc3_runtime_metadata_record_set.h"

struct Objc3FrontendArtifactBundle;

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestPipelineStages(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactBundle &bundle);

void AppendObjc3FrontendArtifactManifestLoweringHeader(
    std::ostream &manifest,
    const Objc3FrontendOptions &options,
    std::size_t vector_signature_functions,
    const std::string &property_synthesis_ivar_binding_replay_key,
    const Objc3PropertySynthesisIvarBindingContract
        &property_synthesis_ivar_binding_contract);

void AppendObjc3FrontendArtifactManifestRecordArrays(
    std::ostream &manifest,
    const Objc3Program &program,
    const std::vector<int> &resolved_global_values,
    const std::vector<const FunctionDecl *> &manifest_functions,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

void PopulateObjc3FrontendArtifactBundleSummaryOutputs(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &runtime_aware_import_module_frontend_closure,
    const Objc3VersionedConformanceReportLoweringSummary
        &versioned_conformance_report_lowering,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBlockOwnershipArtifactPreservationSummary
        &runtime_block_ownership_artifact_preservation_summary,
    const Objc3RuntimeStorageReflectionArtifactPreservationSummary
        &runtime_storage_reflection_artifact_preservation_summary,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapLegalityFailureContractSummary
        &runtime_bootstrap_legality_failure_contract,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &frontend_compatibility_strictness_claim_semantics,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &tooling_legacy_canonical_migration_semantics_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &tooling_machine_readable_conformance_report_contract_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &tooling_feature_aware_conformance_report_emission_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &tooling_corpus_sharding_release_evidence_packaging_summary,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering);

}  // namespace objc3::artifacts::frontend
