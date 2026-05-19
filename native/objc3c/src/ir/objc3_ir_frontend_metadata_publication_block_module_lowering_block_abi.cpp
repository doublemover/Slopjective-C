#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_abi.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRBlockAbiLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!20 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
