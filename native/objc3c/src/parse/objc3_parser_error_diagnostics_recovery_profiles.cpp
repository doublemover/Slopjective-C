#include "parse/objc3_parser_error_diagnostics_recovery_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_error_diagnostics_recovery_site_collection.inc"

#include "parse/objc3_parser_error_diagnostics_recovery_profile_building.inc"

}  // namespace

#include "parse/objc3_parser_error_diagnostics_recovery_profiles_string_serialization.inc"
#include "parse/objc3_parser_error_diagnostics_recovery_profiles_normalization_predicate.inc"
#include "parse/objc3_parser_error_diagnostics_recovery_profiles_function_profile_construction.inc"
#include "parse/objc3_parser_error_diagnostics_recovery_profiles_opaque_method_profile_construction.inc"

}  // namespace objc3c::parse
