#include "pipeline/frontend_runtime_metadata_boundary_helpers.h"

#include <algorithm>
#include <cstddef>

namespace objc3c::pipeline::orchestration {

Objc3RuntimeMetadataSourceOwnershipBoundary
BuildRuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3RuntimeMetadataSourceOwnershipBoundary boundary;
  const std::size_t sema_interface_implementation_record_count =
      type_metadata_handoff.interfaces_lexicographic.size() +
      type_metadata_handoff.implementations_lexicographic.size();
  const bool sema_interface_implementation_record_count_present =
      sema_interface_implementation_record_count > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_interfaces > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_implementations > 0u;

  boundary.frontend_owns_runtime_metadata_source_records = true;
  boundary.runtime_metadata_source_records_ready_for_lowering = false;
  boundary.native_runtime_library_present = false;
  boundary.runtime_link_test_only = true;
  boundary.class_record_count = records.classes_lexicographic.size();
  boundary.protocol_record_count = records.protocols_lexicographic.size();
  boundary.category_interface_record_count =
      static_cast<std::size_t>(std::count_if(
          records.categories_lexicographic.begin(),
          records.categories_lexicographic.end(),
          [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
            return record.record_kind == "interface";
          }));
  boundary.category_implementation_record_count =
      static_cast<std::size_t>(std::count_if(
          records.categories_lexicographic.begin(),
          records.categories_lexicographic.end(),
          [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
            return record.record_kind == "implementation";
          }));
  boundary.property_record_count = records.properties_lexicographic.size();
  boundary.method_record_count = records.methods_lexicographic.size();
  boundary.ivar_record_count = records.ivars_lexicographic.size();

  // Diagnostic precision anchor: category containers are separate
  // runtime-metadata records, so the sema handoff is checked against class
  // source records only until category-specific sema ownership is wired.
  const std::size_t source_interface_implementation_record_count =
      boundary.class_record_count;
  const bool class_alignment_consistent =
      !sema_interface_implementation_record_count_present ||
      sema_interface_implementation_record_count ==
          source_interface_implementation_record_count;
  boundary.deterministic_source_schema =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(records) &&
      class_alignment_consistent &&
      boundary.ivar_record_count <= boundary.property_record_count &&
      !boundary.contract_id.empty() &&
      !boundary.canonical_source_schema.empty() &&
      !boundary.class_record_ast_anchor.empty() &&
      !boundary.protocol_record_ast_anchor.empty() &&
      !boundary.category_record_ast_anchor.empty() &&
      !boundary.property_record_ast_anchor.empty() &&
      !boundary.method_record_ast_anchor.empty() &&
      !boundary.ivar_record_ast_anchor.empty() &&
      !boundary.ivar_record_source_model.empty();
  boundary.fail_closed =
      boundary.frontend_owns_runtime_metadata_source_records &&
      !boundary.runtime_metadata_source_records_ready_for_lowering &&
      !boundary.native_runtime_library_present &&
      boundary.runtime_link_test_only;

  if (!class_alignment_consistent) {
    boundary.failure_reason = "AST/sema class metadata source counts diverged";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason = "ivar source records exceed property source records";
  } else if (!boundary.deterministic_source_schema) {
    boundary.failure_reason = "runtime metadata source schema anchors are incomplete";
  } else if (!boundary.fail_closed) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not fail-closed";
  }

  return boundary;
}

}  // namespace objc3c::pipeline::orchestration
