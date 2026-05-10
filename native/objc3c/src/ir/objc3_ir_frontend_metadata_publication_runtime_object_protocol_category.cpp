#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_protocol_category.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeProtocolCategoryMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  std::size_t runtime_metadata_protocol_bundle_count = 0;
  std::size_t runtime_metadata_protocol_inherited_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_protocol_bundles_lexicographic) {
    ++runtime_metadata_protocol_bundle_count;
    runtime_metadata_protocol_inherited_reference_total +=
        bundle.inherited_protocol_owner_identities_lexicographic.size();
  }
  std::size_t runtime_metadata_category_bundle_count = 0;
  std::size_t runtime_metadata_category_adopted_reference_total = 0;
  std::size_t runtime_metadata_category_attachment_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_category_bundles_lexicographic) {
    ++runtime_metadata_category_bundle_count;
    runtime_metadata_category_adopted_reference_total +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
    runtime_metadata_category_attachment_reference_total += 3u;
  }
  out << "!57 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_category_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_category_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_protocol_reference_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_category_attachment_model)
      << "\", i1 "
      << (metadata.runtime_metadata_protocol_category_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_protocol_category_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_protocol_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_protocol_inherited_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_category_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_category_adopted_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_category_attachment_reference_total)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_category_typed_handoff_replay_key)
      << "\"}\n";
}
