#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_executable_layout.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRExecutableLayoutAccessorMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  out << "!67 = !{!\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_descriptor_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_offset_global_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_table_model)
      << "\", i1 " << (metadata.executable_ivar_layout_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.executable_ivar_layout_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_offset_global_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_table_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_owner_entries)
      << ", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_replay_key)
      << "\"}\n";
  out << "!68 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
}
