#include "parse/objc3_type_feature_profiles.h"

#include "parse/objc3_parser_profile_helpers.h"

#include <sstream>

namespace objc3c::parse {

std::string BuildLightweightGenericConstraintProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  const bool generic_instantiation_valid =
      !has_generic_suffix || (generic_suffix_terminated && object_pointer_type_spelling);
  std::ostringstream out;
  out << "lightweight-generics:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";suffix-bytes=" << generic_suffix_text.size()
      << ";instantiation-valid=" << (generic_instantiation_valid ? "true" : "false");
  return out.str();
}

bool IsLightweightGenericConstraintProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated) {
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

std::string BuildNullabilityFlowProfile(
    bool object_pointer_type_spelling, std::size_t nullability_suffix_count,
    bool has_pointer_declarator, bool has_generic_suffix,
    bool generic_suffix_terminated) {
  const bool flow_precision_valid =
      nullability_suffix_count == 0 || object_pointer_type_spelling;
  std::ostringstream out;
  out << "nullability-flow:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";suffix-count=" << nullability_suffix_count
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";generic-terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";flow-precision-valid=" << (flow_precision_valid ? "true" : "false");
  return out.str();
}

bool IsNullabilityFlowProfileNormalized(bool object_pointer_type_spelling,
                                        std::size_t nullability_suffix_count) {
  if (nullability_suffix_count == 0) {
    return true;
  }
  return object_pointer_type_spelling;
}

#include "parse/objc3_type_generic_variance_metadata_profiles.inc"

}  // namespace objc3c::parse
