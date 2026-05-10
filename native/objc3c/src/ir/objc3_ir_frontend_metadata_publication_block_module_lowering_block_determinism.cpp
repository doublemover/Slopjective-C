#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_determinism.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRBlockDeterminismLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!23 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_baseline_weight_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_determinism_perf_baseline_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
