#include "parse/objc3_type_feature_profiles.h"

#include "parse/objc3_parser_profile_helpers.h"

#include <sstream>

namespace objc3c::parse {

#include "parse/objc3_type_feature_module_profiles.inc"

std::string BuildThrowsDeclarationProfile(
    bool throws_declared, bool has_return_annotation, bool is_prototype,
    bool has_body, bool is_method_declaration, bool is_class_method,
    std::size_t parameter_count, std::size_t selector_piece_count,
    bool typed_throws_declared, const std::string &typed_error_type_spelling,
    bool typed_error_generic_suffix_terminated,
    unsigned typed_error_pointer_depth) {
  const bool declaration_shape_valid =
      (is_prototype && !has_body) || (!is_prototype && has_body);
  const bool method_selector_surface_ready =
      !is_method_declaration || selector_piece_count > 0;
  const bool typed_payload_ready =
      !typed_throws_declared ||
      (throws_declared && !typed_error_type_spelling.empty() &&
       typed_error_generic_suffix_terminated);
  const bool untyped_error_abi_ready = throws_declared && !typed_throws_declared;
  const bool typed_error_abi_ready = false;
  const bool propagation_ready =
      declaration_shape_valid && method_selector_surface_ready &&
      typed_payload_ready;
  const std::string typed_effect_signature =
      typed_throws_declared
          ? "throws:typed:" + typed_error_type_spelling
          : (throws_declared ? "throws:untyped:id<Error>" : "throws:none");
  const std::string callable_compatibility_policy =
      typed_throws_declared
          ? "typed-throws-exact-payload-match-lowering-deferred"
          : (throws_declared ? "untyped-throws-id-error-carrier"
                             : "nonthrowing-only");

  std::ostringstream out;
  out << "throws-declaration:declared=" << (throws_declared ? "true" : "false")
      << ";throws-kind="
      << (typed_throws_declared ? "typed" : (throws_declared ? "untyped" : "none"))
      << ";has-return-annotation=" << (has_return_annotation ? "true" : "false")
      << ";prototype=" << (is_prototype ? "true" : "false")
      << ";has-body=" << (has_body ? "true" : "false")
      << ";is-method-declaration=" << (is_method_declaration ? "true" : "false")
      << ";is-class-method=" << (is_class_method ? "true" : "false")
      << ";parameter-count=" << parameter_count
      << ";selector-piece-count=" << selector_piece_count
      << ";typed-error-type=" << typed_error_type_spelling
      << ";typed-error-pointer-depth=" << typed_error_pointer_depth
      << ";effect-signature=" << typed_effect_signature
      << ";callable-compatibility=" << callable_compatibility_policy
      << ";typed-payload-ready=" << (typed_payload_ready ? "true" : "false")
      << ";untyped-error-abi-ready="
      << (untyped_error_abi_ready ? "true" : "false")
      << ";typed-error-abi-ready="
      << (typed_error_abi_ready ? "true" : "false")
      << ";declaration-shape-valid=" << (declaration_shape_valid ? "true" : "false")
      << ";method-selector-surface-ready=" << (method_selector_surface_ready ? "true" : "false")
      << ";propagation-ready=" << (propagation_ready ? "true" : "false");
  return out.str();
}

bool IsThrowsDeclarationProfileNormalized(bool is_prototype, bool has_body,
                                          bool is_method_declaration,
                                          std::size_t selector_piece_count,
                                          bool throws_declared,
                                          bool typed_throws_declared,
                                          const std::string &typed_error_type_spelling,
                                          bool typed_error_generic_suffix_terminated) {
  const bool declaration_shape_valid =
      (is_prototype && !has_body) || (!is_prototype && has_body);
  if (!declaration_shape_valid) {
    return false;
  }
  if (typed_throws_declared &&
      (!throws_declared || typed_error_type_spelling.empty() ||
       !typed_error_generic_suffix_terminated)) {
    return false;
  }
  if (!is_method_declaration) {
    return true;
  }
  return selector_piece_count > 0;
}

}  // namespace objc3c::parse
