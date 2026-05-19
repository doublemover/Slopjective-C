#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!29 = !{i64 "
      << static_cast<unsigned long long>(metadata.module_import_graph_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.module_import_graph_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.module_import_graph_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_module_import_graph_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!30 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.namespace_collision_shadowing_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_namespace_collision_shadowing_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
