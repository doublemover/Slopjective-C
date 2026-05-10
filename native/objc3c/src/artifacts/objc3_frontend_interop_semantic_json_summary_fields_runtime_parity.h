#pragma once

#include "artifacts/objc3_frontend_interop_semantic_json_summary_fields_common.h"

namespace objc3::artifacts::frontend::interop_semantic_json_detail {

inline void AppendInteropInteropRuntimeParitySummaryJsonFields(
    std::ostringstream &out,
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  out << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << summary.c_foreign_callable_sites
      << ",\"objc_method_foreign_callable_sites\":"
      << summary.objc_method_foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"import_module_foreign_callable_sites\":"
      << summary.import_module_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << summary.objc_runtime_parity_callable_sites
      << ",\"foreign_definition_rejection_sites\":"
      << summary.foreign_definition_rejection_sites
      << ",\"import_without_foreign_rejection_sites\":"
      << summary.import_without_foreign_rejection_sites
      << ",\"implementation_annotation_rejection_sites\":"
      << summary.implementation_annotation_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"declaration_only_foreign_c_enforced\":"
      << (summary.declaration_only_foreign_c_enforced ? "true" : "false")
      << ",\"import_module_requires_foreign_enforced\":"
      << (summary.import_module_requires_foreign_enforced ? "true" : "false")
      << ",\"implementation_annotations_fail_closed\":"
      << (summary.implementation_annotations_fail_closed ? "true" : "false")
      << ",\"objc_runtime_parity_classified\":"
      << (summary.objc_runtime_parity_classified ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"";
}

}  // namespace objc3::artifacts::frontend::interop_semantic_json_detail
