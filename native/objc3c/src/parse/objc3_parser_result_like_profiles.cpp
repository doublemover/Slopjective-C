#include "parse/objc3_parser_result_like_profiles.h"

#include <sstream>

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_result_like_site_collection.inc"

}  // namespace

#include "parse/objc3_parser_result_like_profiles_string_serialization.inc"
#include "parse/objc3_parser_result_like_profiles_normalization_predicate.inc"
#include "parse/objc3_parser_result_like_profiles_body_profile_construction.inc"
#include "parse/objc3_parser_result_like_profiles_opaque_body_profile_construction.inc"

}  // namespace objc3c::parse
