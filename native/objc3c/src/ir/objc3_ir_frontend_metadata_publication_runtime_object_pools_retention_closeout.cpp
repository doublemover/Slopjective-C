#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_pools_retention_closeout.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimePoolRetentionCloseoutMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  out << "!59 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorStringPoolEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorPoolEmissionPayloadModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStringPoolEmissionPayloadModel)
      << "\", i64 "
      << static_cast<unsigned long long>(selector_pool_global_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_string_pool_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeSelectorPoolLogicalSection))
      << "\", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeStringPoolLogicalSection))
      << "\"}\n";
  out << "!60 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionHarnessContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionPositiveCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionNegativeCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSectionCommand)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSymbolCommand)
      << "\", i64 4, i64 1}\n";
  out << "!61 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionBoundaryModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionAnchorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionArtifact)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionSymbolPrefix)
      << "\"}\n";
  out << "!62 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionAnchorModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerAnchorLogicalSection)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
      << "\", !\"" << EscapeCStringLiteral(runtime_metadata_linker_anchor_symbol)
      << "\", !\"" << EscapeCStringLiteral(runtime_metadata_discovery_root_symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerResponseArtifactSuffix)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryArtifactSuffix)
      << "\"}\n";
  out << "!63 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_discovery_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_anchor_seed_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_merge_model)
      << "\", i1 "
      << (metadata.runtime_metadata_archive_static_link_discovery_ready ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_response_artifact_suffix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_key)
      << "\"}\n";
  out << "!64 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateFailureModel)
      << "\"}\n";
  out << "!65 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataObjectEmissionCloseoutContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel)
      << "\"}\n";
}
