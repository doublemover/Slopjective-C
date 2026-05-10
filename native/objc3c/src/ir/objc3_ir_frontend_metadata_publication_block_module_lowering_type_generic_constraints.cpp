#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_generic_constraints.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRTypeGenericConstraintLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!24 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_generic_constraint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_generic_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_normalized_constraint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.lightweight_generic_constraint_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_lightweight_generic_constraint_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
