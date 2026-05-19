#include "pipeline/frontend_semantic_metadata_summary_helpers.h"

#include <cstddef>

#include "support/objc3_type_profile_helpers.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

void AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth,
    const std::vector<Objc3SemaTokenMetadata> &pointer_declarator_tokens,
    const std::vector<Objc3SemaTokenMetadata> &nullability_suffix_tokens,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  if (object_pointer_type_spelling) {
    ++summary.object_pointer_type_spellings;
  }
  summary.pointer_declarator_depth_total += pointer_declarator_depth;
  summary.pointer_declarator_token_entries += pointer_declarator_tokens.size();
  summary.nullability_suffix_entries += nullability_suffix_tokens.size();

  if (has_pointer_declarator) {
    ++summary.pointer_declarator_entries;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      objc3c::support::IsPointerDeclaratorDepthConsistent(
          has_pointer_declarator, pointer_declarator_depth) &&
      objc3c::support::IsObjectPointerTypeNameConsistent(
          object_pointer_type_spelling, object_pointer_type_name) &&
      pointer_declarator_tokens.size() ==
          static_cast<std::size_t>(pointer_declarator_depth);

  for (const auto &token : pointer_declarator_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::PointerDeclarator &&
        !token.text.empty();
  }
  for (const auto &token : nullability_suffix_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::NullabilitySuffix &&
        !token.text.empty();
  }

  if (has_generic_suffix) {
    ++summary.generic_suffix_entries;
    if (generic_suffix_terminated) {
      ++summary.terminated_generic_suffix_entries;
    } else {
      ++summary.unterminated_generic_suffix_entries;
    }
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        !generic_suffix_text.empty() && generic_suffix_text.front() == '<' &&
        (!generic_suffix_terminated || generic_suffix_text.back() == '>');
    return;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      generic_suffix_terminated && generic_suffix_text.empty();
}

void AccumulateObjectPointerNullabilityGenericsForMethod(
    const Objc3MethodDecl &method,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
      method.return_object_pointer_type_spelling,
      method.return_object_pointer_type_name,
      method.has_return_pointer_declarator,
      method.return_pointer_declarator_depth,
      method.return_pointer_declarator_tokens,
      method.return_nullability_suffix_tokens,
      method.has_return_generic_suffix,
      method.return_generic_suffix_terminated,
      method.return_generic_suffix_text,
      summary);
  for (const auto &param : method.params) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
        param.object_pointer_type_spelling,
        param.object_pointer_type_name,
        param.has_pointer_declarator,
        param.pointer_declarator_depth,
        param.pointer_declarator_tokens,
        param.nullability_suffix_tokens,
        param.has_generic_suffix,
        param.generic_suffix_terminated,
        param.generic_suffix_text,
        summary);
  }
}

template <typename Container>
void AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
    const Container &declarations,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
          property.object_pointer_type_spelling,
          property.object_pointer_type_name,
          property.has_pointer_declarator,
          property.pointer_declarator_depth,
          property.pointer_declarator_tokens,
          property.nullability_suffix_tokens,
          property.has_generic_suffix,
          property.generic_suffix_terminated,
          property.generic_suffix_text,
          summary);
    }
    for (const auto &method : declaration.methods) {
      AccumulateObjectPointerNullabilityGenericsForMethod(method, summary);
    }
  }
}

}  // namespace detail

Objc3FrontendObjectPointerNullabilityGenericsSummary
BuildObjectPointerNullabilityGenericsSummary(const Objc3Program &program) {
  Objc3FrontendObjectPointerNullabilityGenericsSummary summary;
  for (const auto &fn : program.functions) {
    detail::AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
        fn.return_object_pointer_type_spelling,
        fn.return_object_pointer_type_name,
        fn.has_return_pointer_declarator,
        fn.return_pointer_declarator_depth,
        fn.return_pointer_declarator_tokens,
        fn.return_nullability_suffix_tokens,
        fn.has_return_generic_suffix,
        fn.return_generic_suffix_terminated,
        fn.return_generic_suffix_text,
        summary);
    for (const auto &param : fn.params) {
      detail::AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
          param.object_pointer_type_spelling,
          param.object_pointer_type_name,
          param.has_pointer_declarator,
          param.pointer_declarator_depth,
          param.pointer_declarator_tokens,
          param.nullability_suffix_tokens,
          param.has_generic_suffix,
          param.generic_suffix_terminated,
          param.generic_suffix_text,
          summary);
    }
  }
  detail::AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
      program.protocols, summary);
  detail::AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
      program.interfaces, summary);
  detail::AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
      program.implementations, summary);

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      summary.terminated_generic_suffix_entries +
              summary.unterminated_generic_suffix_entries ==
          summary.generic_suffix_entries &&
      summary.pointer_declarator_entries <=
          summary.pointer_declarator_depth_total &&
      summary.pointer_declarator_entries <=
          summary.pointer_declarator_token_entries;
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
