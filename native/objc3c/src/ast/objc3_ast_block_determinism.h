#pragma once

#include <cstddef>
#include <string>

std::size_t BuildBlockDeterminismPerfBaselineWeight(std::size_t parameter_count,
                                                    std::size_t capture_count,
                                                    std::size_t body_statement_count,
                                                    bool copy_helper_required,
                                                    bool dispose_helper_required);
std::string BuildBlockDeterminismPerfBaselineProfile(std::size_t parameter_count,
                                                     std::size_t capture_count,
                                                     std::size_t body_statement_count,
                                                     bool copy_helper_required,
                                                     bool dispose_helper_required,
                                                     bool deterministic_capture_set,
                                                     bool copy_dispose_profile_is_normalized,
                                                     std::size_t baseline_weight);
