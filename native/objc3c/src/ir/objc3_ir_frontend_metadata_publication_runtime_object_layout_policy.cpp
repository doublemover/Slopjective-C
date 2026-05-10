#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_layout_policy.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"

void EmitObjc3IRRuntimeLayoutPolicyMetadataNode(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out) {
  out << "!55 = !{!\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.abi_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.scaffold_contract_id)
      << "\", i1 " << (runtime_metadata_layout_policy.ready ? 1 : 0)
      << ", i1 " << (runtime_metadata_layout_policy.fail_closed ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.family_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.descriptor_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.aggregate_relocation_policy)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.comdat_policy)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.visibility_spelling_policy)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.retention_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.object_format_policy_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.object_format_surface_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.object_format)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.section_spelling_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.retention_anchor_model)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.descriptor_linkage)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.aggregate_linkage)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.metadata_visibility)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.retention_root)
      << "\", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_layout_policy.total_retained_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataLayoutPolicyReplayKey(
             runtime_metadata_layout_policy))
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.failure_reason)
      << "\"}\n";
}
