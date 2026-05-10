#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!31 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.public_private_api_partition_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_public_private_api_partition_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!32 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.incremental_module_cache_invalidation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_incremental_module_cache_invalidation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!33 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_namespace_segment_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_import_edge_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_cache_invalidation_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.cross_module_conformance_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_cross_module_conformance_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
