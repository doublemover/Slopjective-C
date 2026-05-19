#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_generic_metadata_abi.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRTypeGenericMetadataAbiLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!28 = !{i64 "
      << static_cast<unsigned long long>(metadata.generic_metadata_abi_lowering_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_generic_suffix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_object_pointer_type_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_pointer_declarator_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.generic_metadata_abi_lowering_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.generic_metadata_abi_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_generic_metadata_abi_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
