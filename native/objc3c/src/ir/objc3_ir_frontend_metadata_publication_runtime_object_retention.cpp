#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_retention.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeObjectRetentionMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::ostringstream &out) {
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
}
