#include "parse/objc3_parser_unsafe_pointer_profiles.h"

#include <memory>
#include <sstream>
#include <vector>

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_unsafe_pointer_profiles_ownership_qualifiers.inc"
#include "parse/objc3_parser_unsafe_pointer_profiles_type_site_counts.inc"

#include "parse/objc3_parser_unsafe_pointer_arithmetic_sites.inc"

#include "parse/objc3_parser_unsafe_pointer_profiles_site_count_profile_construction.inc"

}  // namespace

#include "parse/objc3_parser_unsafe_pointer_profiles_string_serialization.inc"
#include "parse/objc3_parser_unsafe_pointer_profiles_normalization_predicate.inc"
#include "parse/objc3_parser_unsafe_pointer_profiles_function_profile_construction.inc"
#include "parse/objc3_parser_unsafe_pointer_profiles_opaque_method_profile_construction.inc"

}  // namespace objc3c::parse
