#include "ir/objc3_ir_lowering_extension_metadata_publication_metaprogramming.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRMetaprogrammingLoweringMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!104 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_derive_inventory_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_derived_selector_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_macro_replay_visible_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_property_behavior_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_synthesized_binding_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_synthesized_getter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_synthesized_setter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_replay_visible_metadata_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_expansion_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_metaprogramming_expansion_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!105 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_metaprogramming_synthesized_emission_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_derive_method_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_macro_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_synthesized_emitted_property_behavior_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_global_artifact_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_emitted_runtime_method_list_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_synthesized_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_metaprogramming_synthesized_emission_handoff ? 1 : 0)
      << "}\n\n";
  out << "!106 = !{!\""
      << EscapeCStringLiteral(
             metadata.lowering_metaprogramming_module_interface_replay_preservation_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_local_derive_method_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_local_macro_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_local_interface_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_local_implementation_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_local_runtime_method_list_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_module_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_derive_method_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_macro_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_imported_interface_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.metaprogramming_module_replay_imported_runtime_method_list_count)
      << ", i1 "
      << (metadata.metaprogramming_module_replay_runtime_import_artifact_ready ? 1 : 0)
      << ", i1 "
      << (metadata
                  .metaprogramming_module_replay_separate_compilation_preservation_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata.deterministic_metaprogramming_module_interface_replay_handoff ? 1 : 0)
      << "}\n\n";
  out << "!107 = !{!\""
      << EscapeCStringLiteral(Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary())
      << "\", i1 1, i1 0, i1 0, i1 0, i1 1}\n\n";
}
