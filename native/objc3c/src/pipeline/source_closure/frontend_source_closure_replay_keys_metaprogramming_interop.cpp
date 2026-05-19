#include "pipeline/frontend_source_closure_replay_keys.h"

#include <sstream>

namespace objc3c::pipeline::orchestration {

std::string BuildMetaprogrammingMetaprogrammingSourceClosureReplayKey(
    const Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";sites=" << summary.derive_marker_sites << ":"
      << summary.macro_marker_sites << ":" << summary.property_behavior_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildMetaprogrammingMacroPackageProvenanceSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":" << summary.macro_provenance_sites
      << ":" << summary.macro_cache_key_sites << ":"
      << summary.macro_sandbox_policy_sites << ":"
      << summary.expansion_visible_macro_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildMetaprogrammingPropertyBehaviorSourceCompletionReplayKey(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";sites=" << summary.property_behavior_sites
      << ":" << summary.interface_property_behavior_sites << ":"
      << summary.implementation_property_behavior_sites << ":"
      << summary.protocol_property_behavior_sites << ":"
      << summary.synthesized_binding_visible_sites << ":"
      << summary.synthesized_getter_visible_sites << ":"
      << summary.synthesized_setter_visible_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildInteropForeignImportSourceClosureReplayKey(
    const Objc3FrontendInteropForeignImportSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";sites=" << summary.foreign_callable_sites
      << ":" << summary.extern_foreign_callable_sites << ":"
      << summary.import_module_annotation_sites << ":"
      << summary.imported_module_name_sites << ":"
      << summary.export_header_annotation_sites << ":"
      << summary.export_header_name_sites << ":"
      << summary.mixed_image_annotation_sites << ":"
      << summary.mixed_image_name_sites << ":"
      << summary.package_entry_annotation_sites << ":"
      << summary.package_entry_name_sites << ":"
      << summary.interop_annotation_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildInteropCppSwiftInteropAnnotationSourceCompletionReplayKey(
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.swift_name_annotation_sites << ":"
      << summary.swift_private_annotation_sites << ":"
      << summary.cpp_name_annotation_sites << ":"
      << summary.header_name_annotation_sites << ":"
      << summary.abi_alignment_annotation_sites << ":"
      << summary.foreign_type_annotation_sites << ":"
      << summary.interop_metadata_annotation_sites << ":"
      << summary.named_annotation_payload_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::pipeline::orchestration
