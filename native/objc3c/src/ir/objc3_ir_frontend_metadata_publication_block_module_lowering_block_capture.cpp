#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_capture.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRBlockCaptureLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!19 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_parameter_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_capture_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_body_statement_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_empty_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_non_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_literal_capture_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
