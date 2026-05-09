#include "lower/objc3_lowering_contract.h"

#include <sstream>
#include <string>

std::string Objc3RuntimeMetadataObjectPackagingRetentionSummary() {
  std::ostringstream out;
  // object-packaging/retention freeze anchor: lane-D now freezes the
  // current produced-object handoff as module.obj plus retained aggregate
  // symbols. Later archive/link/startup-registration work must preserve this
  // boundary and may not silently replace llvm.used or aggregate symbol roots.
  out << "contract=" << kObjc3RuntimeObjectPackagingRetentionContractId
      << ";boundary_model="
      << kObjc3RuntimeObjectPackagingRetentionBoundaryModel
      << ";retention_anchor_model="
      << kObjc3RuntimeObjectPackagingRetentionAnchorModel
      << ";object_artifact=" << kObjc3RuntimeObjectPackagingRetentionArtifact
      << ";aggregate_symbol_prefix="
      << kObjc3RuntimeObjectPackagingRetentionSymbolPrefix
      << ";non_goals=no-archive-packaging-link-registration-or-startup-bootstrap";
  return out.str();
}

std::string Objc3RuntimeMetadataLinkerRetentionSummary() {
  std::ostringstream out;
  // linker-retention/dead-strip resistance anchor: lane-D adds one
  // real public linker anchor and one public discovery root, together with a
  // driver response-file payload that can force-retain the archived object that
  // owns the metadata sections. Multi-archive and multi-TU edge cases remain
  // deferred until the next runtime step.
  out << "contract=" << kObjc3RuntimeLinkerRetentionContractId
      << ";anchor_model=" << kObjc3RuntimeLinkerRetentionAnchorModel
      << ";discovery_model=" << kObjc3RuntimeLinkerDiscoveryModel
      << ";linker_anchor_logical_section="
      << kObjc3RuntimeLinkerAnchorLogicalSection
      << ";discovery_root_logical_section="
      << kObjc3RuntimeLinkerDiscoveryRootLogicalSection
      << ";linker_response_artifact_suffix="
      << kObjc3RuntimeLinkerResponseArtifactSuffix
      << ";discovery_artifact_suffix="
      << kObjc3RuntimeLinkerDiscoveryArtifactSuffix
      << ";coff_flag_model=" << kObjc3RuntimeLinkerRetentionCoffFlagModel
      << ";elf_flag_model=" << kObjc3RuntimeLinkerRetentionElfFlagModel
      << ";mach_o_flag_model=" << kObjc3RuntimeLinkerRetentionMachOFlagModel
      << ";non_goals=no-multi-archive-fan-in-or-cross-translation-unit-anchor-merging";
  return out.str();
}

std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary() {
  std::ostringstream out;
  // archive/static-link discovery anchor: lane-D extends the D002
  // object-level discovery path with translation-unit-stable public anchors and
  // one merged discovery/response artifact pair that downstream archive/static
  // link orchestration can consume deterministically across multiple TUs.
  out << "contract=" << kObjc3RuntimeArchiveStaticLinkDiscoveryContractId
      << ";anchor_seed_model="
      << kObjc3RuntimeArchiveStaticLinkAnchorSeedModel
      << ";translation_unit_identity_model="
      << kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel
      << ";merge_model=" << kObjc3RuntimeArchiveStaticLinkMergeModel
      << ";merged_linker_response_artifact_suffix="
      << kObjc3RuntimeMergedLinkerResponseArtifactSuffix
      << ";merged_discovery_artifact_suffix="
      << kObjc3RuntimeMergedDiscoveryArtifactSuffix
      << ";non_goals=no-runtime-registration-or-startup-bootstrap";
  return out.str();
}

std::string Objc3RuntimeBootstrapLoweringBoundarySummary() {
  std::ostringstream out;
  // constructor-root/init-array lowering freeze anchor: the live
  // lowering path already materializes ctor-root/global_ctors/init-stub/
  // registration-table/image-local-init globals. This summary is the
  // canonical replay-stable description of that current boundary and the
  // registration-descriptor handoff it consumes.
  out << "contract=" << kObjc3RuntimeBootstrapLoweringContractId
      << ";boundary_model=" << kObjc3RuntimeBootstrapLoweringBoundaryModel
      << ";registration_descriptor_handoff_contract_id="
      << kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId
      << ";registration_descriptor_artifact="
      << kObjc3RuntimeBootstrapRegistrationDescriptorArtifact
      << ";registration_descriptor_handoff_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel
      << ";publication_owner_contract_id="
      << kObjc3RuntimeRegistrationPublicationContractId
      << ";metadata_handoff_owner=" << kObjc3RuntimeMetadataHandoffOwner
      << ";descriptor_lowering_owner="
      << kObjc3RuntimeRegistrationDescriptorLoweringOwner
      << ";constructor_root_publication_owner="
      << kObjc3RuntimeConstructorRootPublicationOwner
      << ";init_stub_publication_owner="
      << kObjc3RuntimeInitStubPublicationOwner
      << ";registration_table_publication_owner="
      << kObjc3RuntimeRegistrationTablePublicationOwner
      << ";constructor_root_symbol="
      << kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol
      << ";init_stub_symbol_prefix="
      << kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix
      << ";registration_table_symbol_prefix="
      << kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix
      << ";registration_entrypoint_symbol="
      << kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol
      << ";global_ctor_list_model="
      << kObjc3RuntimeBootstrapGlobalCtorListModel
      << ";constructor_root_emission_state="
      << kObjc3RuntimeBootstrapConstructorRootEmissionState
      << ";init_stub_emission_state="
      << kObjc3RuntimeBootstrapInitStubEmissionState
      << ";registration_table_emission_state="
      << kObjc3RuntimeBootstrapRegistrationTableEmissionState
      << ";non_goals=no-multi-image-root-fanout-no-runtime-replay-partitioning-no-late-linker-synthesis";
  return out.str();
}

std::string Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary() {
  std::ostringstream out;
  // registration-descriptor/image-root lowering anchor: this
  // summary freezes the first live binary lowering surface for the source
  // identifiers from the earlier runtime step. The IR/object path now materializes dedicated
  // registration-descriptor and image-root globals in their own sections
  // rather than treating those identifiers as sidecar-only metadata.
  out << "contract="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId
      << ";lowering_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringModel
      << ";registration_descriptor_logical_section="
      << kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection
      << ";image_root_logical_section="
      << kObjc3RuntimeBootstrapImageRootLogicalSection
      << ";registration_descriptor_symbol_prefix="
      << kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix
      << ";image_root_symbol_prefix="
      << kObjc3RuntimeBootstrapImageRootSymbolPrefix
      << ";registration_descriptor_payload_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorPayloadModel
      << ";image_root_payload_model="
      << kObjc3RuntimeBootstrapImageRootPayloadModel
      << ";publication_owner_contract_id="
      << kObjc3RuntimeRegistrationPublicationContractId
      << ";descriptor_lowering_owner="
      << kObjc3RuntimeRegistrationDescriptorLoweringOwner
      << ";non_goals=no-cross-translation-unit-root-deduplication-or-runtime-fanout-merge";
  return out.str();
}

std::string Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary() {
  std::ostringstream out;
  // archive/static-link bootstrap replay corpus anchor: this
  // summary binds the retained archive/static-link discovery path to
  // the live bootstrap reset/replay runtime so validation can prove
  // real startup registration and replay behavior over linked archives.
  out << "contract="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId
      << ";corpus_model="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel
      << ";binary_proof_model="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel
      << ";archive_static_link_discovery_contract_id="
      << kObjc3RuntimeArchiveStaticLinkDiscoveryContractId
      << ";bootstrap_failure_restart_contract_id="
      << kObjc3BootstrapFailureRestartSemanticsContractId
      << ";registration_descriptor_lowering_contract_id="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId
      << ";replay_registered_images_symbol="
      << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
      << ";reset_replay_state_snapshot_symbol="
      << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
      << ";non_goals=no-new-bootstrap-runtime-entrypoints-or-linker-merge-models";
  return out.str();
}
