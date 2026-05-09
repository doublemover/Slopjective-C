#include "ir/objc3_ir_control_flow_ops.h"

std::string BuildObjc3IRLabelLine(const std::string &label) {
  return label + ":";
}

std::string BuildObjc3IRBranchLine(const std::string &target_label) {
  return "  br label %" + target_label;
}

std::string BuildObjc3IRConditionalBranchLine(const std::string &condition,
                                              const std::string &true_label,
                                              const std::string &false_label) {
  return "  br i1 " + condition + ", label %" + true_label + ", label %" +
         false_label;
}

std::string BuildObjc3IRI32IsZeroLine(const std::string &result,
                                      const std::string &value) {
  return "  " + result + " = icmp eq i32 " + value + ", 0";
}

std::string BuildObjc3IRI32PhiLine(const std::string &result,
                                   const std::string &first_value,
                                   const std::string &first_label,
                                   const std::string &second_value,
                                   const std::string &second_label) {
  return "  " + result + " = phi i32 [" + first_value + ", %" +
         first_label + "], [" + second_value + ", %" + second_label + "]";
}
