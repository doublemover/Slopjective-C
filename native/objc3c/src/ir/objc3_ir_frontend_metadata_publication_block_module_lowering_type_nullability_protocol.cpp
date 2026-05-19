#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_nullability_protocol.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRTypeNullabilityProtocolLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!25 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_nullability_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_nullable_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_nonnull_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nullability_flow_warning_precision_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_nullability_flow_warning_precision_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!26 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.protocol_qualified_object_type_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_protocol_qualified_object_type_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
