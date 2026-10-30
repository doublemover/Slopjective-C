#pragma once

#include <cstdint>

namespace objc3::clang_foundation {

inline constexpr std::uint64_t kInterfaceArtifactByteLimit = 32ull * 1024 * 1024;
inline constexpr const char *kInterfaceResourceDiagnostic = "O3IFC182";

// Independent budgets bound search work, compact metadata, the selected graph
// and its repeated parser mappings. They bound admitted inputs/work, rather
// than promising an exact process-RSS ceiling for Clang's internal ASTs.
struct InterfaceResourceLimits {
  std::uint64_t read_bytes = 512ull * 1024 * 1024;
  std::uint64_t scan_path_bytes = 16ull * 1024 * 1024;
  std::uint64_t index_string_bytes = 16ull * 1024 * 1024;
  std::uint64_t index_modules = 16384;
  std::uint64_t decoded_declarations = 250000;
  std::uint64_t selected_bytes = 64ull * 1024 * 1024;
  std::uint64_t selected_declarations = 100000;
  std::uint64_t selected_modules = 512;
  std::uint64_t dependency_edges = 8192;
  std::uint64_t dependency_depth = 128;
  std::uint64_t parser_input_bytes = 128ull * 1024 * 1024;
  std::uint64_t parser_declarations = 500000;
};

struct InterfaceResourceUsage {
  std::uint64_t read_bytes = 0;
  std::uint64_t scan_path_bytes = 0;
  std::uint64_t index_string_bytes = 0;
  std::uint64_t index_modules = 0;
  std::uint64_t candidate_pairs = 0;
  std::uint64_t coalesced_duplicates = 0;
  std::uint64_t decoded_declarations = 0;
  std::uint64_t selected_bytes = 0;
  std::uint64_t selected_declarations = 0;
  std::uint64_t selected_modules = 0;
  std::uint64_t dependency_edges = 0;
  std::uint64_t parser_input_bytes = 0;
  std::uint64_t parser_declarations = 0;
};

// Check before adding, including at UINT64_MAX. Failed charges leave the
// counter unchanged, so diagnostics and caller rollback stay deterministic.
inline bool ChargeInterfaceResource(std::uint64_t &used, std::uint64_t amount,
                                     std::uint64_t limit) {
  if (used > limit || amount > limit - used) return false;
  used += amount;
  return true;
}

} // namespace objc3::clang_foundation
