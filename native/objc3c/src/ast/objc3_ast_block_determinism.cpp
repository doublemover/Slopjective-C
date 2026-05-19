#include "ast/objc3_ast_block_determinism.h"

#include <sstream>

std::size_t BuildBlockDeterminismPerfBaselineWeight(std::size_t parameter_count,
                                                    std::size_t capture_count,
                                                    std::size_t body_statement_count,
                                                    bool copy_helper_required,
                                                    bool dispose_helper_required) {
  std::size_t weight = parameter_count * 2u + capture_count * 8u +
                       body_statement_count * 4u;
  if (copy_helper_required) {
    weight += 6u;
  }
  if (dispose_helper_required) {
    weight += 6u;
  }
  return weight;
}

std::string BuildBlockDeterminismPerfBaselineProfile(std::size_t parameter_count,
                                                     std::size_t capture_count,
                                                     std::size_t body_statement_count,
                                                     bool copy_helper_required,
                                                     bool dispose_helper_required,
                                                     bool deterministic_capture_set,
                                                     bool copy_dispose_profile_is_normalized,
                                                     std::size_t baseline_weight) {
  const char *tier =
      baseline_weight <= 24u ? "light" : (baseline_weight <= 64u ? "medium" : "heavy");
  std::ostringstream out;
  out << "block-det-perf-baseline:params=" << parameter_count
      << ";captures=" << capture_count
      << ";body-statements=" << body_statement_count
      << ";copy-helper=" << (copy_helper_required ? "enabled" : "elided")
      << ";dispose-helper=" << (dispose_helper_required ? "enabled" : "elided")
      << ";deterministic-captures=" << (deterministic_capture_set ? "true" : "false")
      << ";normalized=" << (copy_dispose_profile_is_normalized ? "true" : "false")
      << ";weight=" << baseline_weight
      << ";tier=" << tier;
  return out.str();
}
