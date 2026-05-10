#include "artifacts/objc3_frontend_artifact_runtime_metadata_contract_metadata.h"

#include <cstddef>

#include "artifacts/identity/artifact_identity.h"

namespace objc3::artifacts::frontend {
namespace {

#include "artifacts/objc3_frontend_runtime_metadata_contract_source_export.inc"
#include "artifacts/objc3_frontend_runtime_metadata_contract_sections.inc"
#include "artifacts/objc3_frontend_runtime_metadata_contract_executable_graph.inc"
#include "artifacts/objc3_frontend_runtime_metadata_contract_archive.inc"

}  // namespace

void ApplyObjc3FrontendRuntimeMetadataContractMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const std::filesystem::path &input_path,
    const std::string &parse_artifact_replay_key,
    const std::string &lowering_boundary_replay_key) {
  ApplyRuntimeMetadataSourceOwnershipContract(
      ir_frontend_metadata, runtime_metadata_source_ownership);
  ApplyRuntimeExportLegalityContract(
      ir_frontend_metadata, runtime_export_legality);
  ApplyRuntimeExportEnforcementContract(
      ir_frontend_metadata, runtime_export_enforcement);
  ApplyRuntimeMetadataSectionAbiContract(
      ir_frontend_metadata, runtime_metadata_section_abi);
  ApplyRuntimeMetadataSectionPublicationContract(
      ir_frontend_metadata, runtime_metadata_section_publication);
  ApplyRuntimeMetadataExecutableGraphContracts(
      ir_frontend_metadata, executable_metadata_typed_lowering_handoff);
  ApplyRuntimeMetadataArchiveStaticLinkDiscoveryContract(
      ir_frontend_metadata,
      input_path,
      parse_artifact_replay_key,
      lowering_boundary_replay_key);
}

}  // namespace objc3::artifacts::frontend
