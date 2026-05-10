#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_publication.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeMetadataSectionPublicationNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!49 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_abi_contract_id)
      << "\", i1 "
      << (metadata.runtime_metadata_section_publication_emitted ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_publication_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_publication_uses_llvm_used ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_publication_image_info_emitted ? 1
                                                                           : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_class_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_protocol_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_category_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_property_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_ivar_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_total_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_total_retained_global_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_image_info_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_class_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_protocol_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_category_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_property_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_ivar_aggregate_symbol)
      << "\"}\n";
}
