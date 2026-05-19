#pragma once

#include <cstddef>
#include <string>

namespace objc3c::parse {

std::string BuildLightweightGenericConstraintProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text);
bool IsLightweightGenericConstraintProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated);

std::string BuildNullabilityFlowProfile(
    bool object_pointer_type_spelling, std::size_t nullability_suffix_count,
    bool has_pointer_declarator, bool has_generic_suffix,
    bool generic_suffix_terminated);
bool IsNullabilityFlowProfileNormalized(bool object_pointer_type_spelling,
                                        std::size_t nullability_suffix_count);

std::string BuildVarianceBridgeCastProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling);
bool IsVarianceBridgeCastProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling);

std::string BuildGenericMetadataAbiProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling);
bool IsGenericMetadataAbiProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text);

std::string BuildModuleImportGraphProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);
bool IsModuleImportGraphProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text);

std::string BuildNamespaceCollisionShadowingProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);
bool IsNamespaceCollisionShadowingProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);

std::string BuildPublicPrivateApiPartitionProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);
bool IsPublicPrivateApiPartitionProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);

std::string BuildIncrementalModuleCacheInvalidationProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);
bool IsIncrementalModuleCacheInvalidationProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);

std::string BuildCrossModuleConformanceProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);
bool IsCrossModuleConformanceProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &object_pointer_type_name);

std::string BuildThrowsDeclarationProfile(
    bool throws_declared, bool has_return_annotation, bool is_prototype,
    bool has_body, bool is_method_declaration, bool is_class_method,
    std::size_t parameter_count, std::size_t selector_piece_count);
bool IsThrowsDeclarationProfileNormalized(bool is_prototype, bool has_body,
                                          bool is_method_declaration,
                                          std::size_t selector_piece_count);

}  // namespace objc3c::parse
