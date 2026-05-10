#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_class_metaclass.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeClassMetaclassMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  std::size_t runtime_metadata_class_bundle_count = 0;
  std::size_t runtime_metadata_instance_method_reference_total = 0;
  std::size_t runtime_metadata_class_method_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_class_metaclass_bundles_lexicographic) {
    ++runtime_metadata_class_bundle_count;
    runtime_metadata_instance_method_reference_total +=
        bundle.instance_method_count;
    runtime_metadata_class_method_reference_total += bundle.class_method_count;
  }
  out << "!56 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_class_metaclass_name_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_super_link_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_method_list_reference_model)
      << "\", i1 "
      << (metadata.runtime_metadata_class_metaclass_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_class_metaclass_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_class_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_instance_method_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_class_method_reference_total)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key)
      << "\"}\n";
}
