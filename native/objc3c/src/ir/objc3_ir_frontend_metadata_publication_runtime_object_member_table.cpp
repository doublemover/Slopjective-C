#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_member_table.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeMemberTableMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  std::size_t runtime_metadata_method_list_bundle_count = 0;
  std::size_t runtime_metadata_method_entry_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_method_list_bundles_lexicographic) {
    ++runtime_metadata_method_list_bundle_count;
    runtime_metadata_method_entry_total += bundle.entries_lexicographic.size();
  }
  out << "!58 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_member_table_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_method_list_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_method_list_grouping_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_property_descriptor_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_ivar_descriptor_emission_payload_model)
      << "\", i1 "
      << (metadata.runtime_metadata_member_table_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_member_table_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_method_list_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_method_entry_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_property_bundles_lexicographic.size())
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_ivar_bundles_lexicographic.size())
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_member_table_typed_handoff_replay_key)
      << "\"}\n";
}
