#include "parse/objc3_parser_inline_asm_intrinsic_profiles.h"

#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_inline_asm_intrinsic_site_collection.inc"

#include "parse/objc3_parser_inline_asm_intrinsic_profiles_site_count_profile_construction.inc"

}  // namespace

#include "parse/objc3_parser_inline_asm_intrinsic_profiles_string_serialization.inc"

#include "parse/objc3_parser_inline_asm_intrinsic_profiles_normalization_predicate.inc"

#include "parse/objc3_parser_inline_asm_intrinsic_profiles_function_profile_construction.inc"

#include "parse/objc3_parser_inline_asm_intrinsic_profiles_opaque_method_profile_construction.inc"

}  // namespace objc3c::parse
