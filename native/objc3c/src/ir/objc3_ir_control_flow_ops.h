#pragma once

#include <string>

std::string BuildObjc3IRLabelLine(const std::string &label);
std::string BuildObjc3IRBranchLine(const std::string &target_label);
std::string BuildObjc3IRConditionalBranchLine(const std::string &condition,
                                              const std::string &true_label,
                                              const std::string &false_label);
std::string BuildObjc3IRI32IsZeroLine(const std::string &result,
                                      const std::string &value);
std::string BuildObjc3IRI32PhiLine(const std::string &result,
                                   const std::string &first_value,
                                   const std::string &first_label,
                                   const std::string &second_value,
                                   const std::string &second_label);
