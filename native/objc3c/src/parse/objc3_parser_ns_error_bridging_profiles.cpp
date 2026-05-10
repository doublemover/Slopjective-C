#include "parse/objc3_parser_ns_error_bridging_profiles.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_ns_error_bridging_profiles_site_count_profile_construction.inc"

#include "parse/objc3_parser_ns_error_failable_call_sites.inc"

}  // namespace

#include "parse/objc3_parser_ns_error_bridging_profiles_string_serialization.inc"

#include "parse/objc3_parser_ns_error_bridging_profiles_normalization_predicate.inc"

#include "parse/objc3_parser_ns_error_bridging_profiles_function_profile_construction.inc"

#include "parse/objc3_parser_ns_error_bridging_profiles_opaque_method_profile_construction.inc"

}  // namespace objc3c::parse
