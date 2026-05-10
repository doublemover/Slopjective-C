#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchSemanticsCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!11 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_message_send_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.nil_receiver_semantics_foldability_enabled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.nil_receiver_semantics_foldability_foldable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_contract_violation_sites)
      << ", i1 " << (metadata.deterministic_nil_receiver_semantics_foldability_handoff ? 1 : 0)
      << "}\n\n";
  out << "!12 = !{i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_message_send_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_receiver_super_identifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_enabled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_requires_class_context_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_init_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.super_dispatch_method_family_copy_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_mutable_copy_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.super_dispatch_method_family_new_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.super_dispatch_method_family_none_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_returns_retained_result_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_returns_related_result_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_contract_violation_sites)
      << ", i1 " << (metadata.deterministic_super_dispatch_method_family_handoff ? 1 : 0)
      << "}\n\n";
  out << "!13 = !{i64 "
      << static_cast<unsigned long long>(metadata.runtime_link_host_link_message_send_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.runtime_link_host_link_required_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.runtime_link_host_link_elided_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_link_host_link_runtime_dispatch_arg_slots)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_link_host_link_runtime_dispatch_declaration_parameter_count)
      << ", !\"" << EscapeCStringLiteral(metadata.runtime_link_host_link_runtime_dispatch_symbol)
      << "\", i1 "
      << (metadata.runtime_link_host_link_default_runtime_dispatch_symbol_binding ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_link_host_link_contract_violation_sites)
      << ", i1 " << (metadata.deterministic_runtime_link_host_link_handoff ? 1 : 0)
      << "}\n\n";
}
