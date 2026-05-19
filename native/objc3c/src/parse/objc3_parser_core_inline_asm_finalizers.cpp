#include "parse/objc3_parser_core_inline_asm_finalizers.h"

#include "parse/objc3_parser_inline_asm_intrinsic_profiles.h"

namespace objc3c::parse {

namespace {

#include "parse/objc3_parser_core_inline_asm_finalizers_profile_publication.inc"

#include "parse/objc3_parser_core_inline_asm_finalizers_diagnostics.inc"

}  // namespace

#include "parse/objc3_parser_core_inline_asm_finalizers_function.inc"

#include "parse/objc3_parser_core_inline_asm_finalizers_method.inc"

}  // namespace objc3c::parse
