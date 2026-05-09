add_library(objc3c_support_ascii STATIC
  ascii/character_predicates.h
  objc3_ascii_case_transform.cpp
  objc3_ascii_case_transform.h
  objc3_ascii_predicates.h
)
objc3c_apply_build_defaults(objc3c_support_ascii)

add_library(objc3c_support_strings STATIC
  objc3_string_affix_predicates.cpp
  objc3_string_affix_predicates.h
  objc3_string_predicates.h
  objc3_string_search_predicates.cpp
  objc3_string_search_predicates.h
  strings/string_predicates.h
)
objc3c_apply_build_defaults(objc3c_support_strings)
target_link_libraries(objc3c_support_strings PUBLIC
  objc3c_support_ascii
)

add_library(objc3c_support_profile_matchers STATIC
  objc3_profile_any_token_match.cpp
  objc3_profile_any_token_match.h
  objc3_profile_lowercase_token_match.cpp
  objc3_profile_lowercase_token_match.h
  objc3_profile_single_token_match.cpp
  objc3_profile_single_token_match.h
  objc3_profile_token_match.h
)
objc3c_apply_build_defaults(objc3c_support_profile_matchers)
target_link_libraries(objc3c_support_profile_matchers PUBLIC
  objc3c_support_ascii
  objc3c_support_strings
)

add_library(objc3c_support_identifier_io STATIC
  objc3_file_reading.cpp
  objc3_file_reading.h
  objc3_identifier_safe_suffix_builder.cpp
  objc3_identifier_safe_suffix_builder.h
  objc3_identifier_safe_suffix_characters.cpp
  objc3_identifier_safe_suffix_characters.h
  objc3_identifier_safe_suffix.h
  objc3_ir_object_backend_kind.h
  objc3_ir_object_backend_parser.cpp
  objc3_ir_object_backend_parser.h
  objc3_ir_object_backend_token.h
)
objc3c_apply_build_defaults(objc3c_support_identifier_io)
target_link_libraries(objc3c_support_identifier_io PUBLIC
  objc3c_support_strings
)

add_library(objc3c_support_method_family STATIC
  objc3_method_family.cpp
  objc3_method_family.h
  objc3_method_family_known_names.cpp
  objc3_method_family_known_names.h
  objc3_method_family_profile_builder.cpp
  objc3_method_family_profile_builder.h
  objc3_method_family_profile_record.h
  objc3_method_family_profiles.h
  objc3_method_family_related_result.cpp
  objc3_method_family_related_result.h
  objc3_method_family_retained_result.cpp
  objc3_method_family_retained_result.h
  objc3_method_family_traits.h
  objc3_nil_receiver_profile.cpp
  objc3_nil_receiver_profile.h
  objc3_receiver_dispatch_profiles.h
  objc3_receiver_profile_helpers.h
  objc3_super_dispatch_profile.cpp
  objc3_super_dispatch_profile.h
  profiles/receiver_profile_helpers.h
)
objc3c_apply_build_defaults(objc3c_support_method_family)
target_link_libraries(objc3c_support_method_family PUBLIC
  objc3c_support_strings
)

add_library(objc3c_support_type_surfaces STATIC
  objc3_objc_param_type_surface_profiles.h
  objc3_objc_property_type_surface_profiles.h
  objc3_objc_reference_annotation_sites.cpp
  objc3_objc_reference_annotation_sites.h
  objc3_objc_reference_type_profiles.h
  objc3_objc_reference_value_types.cpp
  objc3_objc_reference_value_types.h
  objc3_objc_return_type_surface_profiles.h
  objc3_objc_runtime_type_surface.cpp
  objc3_objc_runtime_type_surface.h
  objc3_objc_runtime_type_surface_profiles.h
  objc3_objc_type_surface_profiles.h
  objc3_objc_value_type_names.cpp
  objc3_objc_value_type_names.h
  objc3_object_pointer_type_name_consistency.cpp
  objc3_object_pointer_type_name_consistency.h
  objc3_pointer_declarator_depth_consistency.cpp
  objc3_pointer_declarator_depth_consistency.h
  objc3_scalar_value_type_names.cpp
  objc3_scalar_value_type_names.h
  objc3_type_spelling_consistency.h
  objc3_type_profile_helpers.h
  objc3_value_type_names.cpp
  objc3_value_type_names.h
  profiles/type_profile_helpers.h
)
objc3c_apply_build_defaults(objc3c_support_type_surfaces)
target_link_libraries(objc3c_support_type_surfaces PUBLIC
  objc3c_ast_core
)

add_library(objc3c_support_properties STATIC
  objc3_current_property_runtime_profiles.h
  objc3_property_accessor_profile_builder.cpp
  objc3_property_accessor_profile_builder.h
  objc3_property_accessor_profiles.h
  objc3_property_accessor_rebuild_policy.cpp
  objc3_property_accessor_rebuild_policy.h
  objc3_property_attribute_profile_match.cpp
  objc3_property_attribute_profile_match.h
  objc3_property_ownership_application.h
  objc3_property_ownership_classification.cpp
  objc3_property_ownership_classification.h
  objc3_property_ownership_kinds.h
  objc3_property_ownership_profile_application.h
  objc3_property_ownership_profile_tokens.cpp
  objc3_property_ownership_profile_tokens.h
  objc3_property_ownership_profiles.h
  objc3_property_profile_tokens.h
  objc3_property_storage_modifier_counts.cpp
  objc3_property_storage_modifier_counts.h
  objc3_property_storage_modifier_spelling.cpp
  objc3_property_storage_modifier_spelling.h
  objc3_property_storage_modifier_support.cpp
  objc3_property_storage_modifier_support.h
  objc3_property_storage_modifiers.h
  objc3_property_storage_profile_helpers.h
  objc3_strong_owned_property_exchange_profile.cpp
  objc3_strong_owned_property_exchange_profile.h
  objc3_weak_property_runtime_helper_profile.cpp
  objc3_weak_property_runtime_helper_profile.h
  profiles/property_profile_helpers.h
  properties/current_property_runtime_profiles.h
)
objc3c_apply_build_defaults(objc3c_support_properties)
target_link_libraries(objc3c_support_properties PUBLIC
  objc3c_support_profile_matchers
  objc3c_support_type_surfaces
)

add_library(objc3c_support_concurrency STATIC
  concurrency/symbol_profiles.h
  concurrency/task_group_symbols.h
  concurrency/task_symbols.h
  objc3_concurrency_cancellation_symbols.cpp
  objc3_concurrency_cancellation_symbols.h
  objc3_concurrency_detached_task_symbols.cpp
  objc3_concurrency_detached_task_symbols.h
  objc3_concurrency_profile_tokens.cpp
  objc3_concurrency_profile_tokens.h
  objc3_concurrency_symbol_profiles.h
  objc3_concurrency_task_creation_symbols.cpp
  objc3_concurrency_task_creation_symbols.h
  objc3_concurrency_task_group_add_task_symbols.cpp
  objc3_concurrency_task_group_add_task_symbols.h
  objc3_concurrency_task_group_cancel_symbols.cpp
  objc3_concurrency_task_group_cancel_symbols.h
  objc3_concurrency_task_group_scope_symbols.cpp
  objc3_concurrency_task_group_scope_symbols.h
  objc3_concurrency_task_group_symbols.h
  objc3_concurrency_task_group_wait_symbols.cpp
  objc3_concurrency_task_group_wait_symbols.h
  objc3_concurrency_task_symbols.h
)
objc3c_apply_build_defaults(objc3c_support_concurrency)
target_link_libraries(objc3c_support_concurrency PUBLIC
  objc3c_support_profile_matchers
  objc3c_support_strings
)

add_library(objc3c_support_runtime_dispatch STATIC
  objc3_runtime_dispatch_symbol.h
  objc3_runtime_dispatch_symbol_characters.cpp
  objc3_runtime_dispatch_symbol_characters.h
  objc3_runtime_dispatch_symbol_validation.h
  runtime_dispatch/symbol_validation.cpp
  runtime_dispatch/symbol_validation.h
)
objc3c_apply_build_defaults(objc3c_support_runtime_dispatch)
target_link_libraries(objc3c_support_runtime_dispatch PUBLIC
  objc3c_support_ascii
)

add_library(objc3c_support_selectors STATIC
  selectors/metadata_selector_validation.cpp
  selectors/metadata_selector_validation.h
  selectors/selector_argument_slots.cpp
  selectors/selector_argument_slots.h
  selectors/selector_code_units.cpp
  selectors/selector_code_units.h
  selectors/selector_identity_normalization.cpp
  selectors/selector_identity_normalization.h
  selectors/selector_normalization.h
  selectors/selector_spelling.h
  selectors/selector_spelling_validation.cpp
  selectors/selector_spelling_validation.h
  selectors/selector_validation.h
)
objc3c_apply_build_defaults(objc3c_support_selectors)

add_library(objc3c_support INTERFACE)
target_link_libraries(objc3c_support INTERFACE
  objc3c_support_ascii
  objc3c_support_concurrency
  objc3c_support_identifier_io
  objc3c_support_method_family
  objc3c_support_profile_matchers
  objc3c_support_properties
  objc3c_support_runtime_dispatch
  objc3c_support_selectors
  objc3c_support_strings
  objc3c_support_type_surfaces
)
target_sources(objc3c_support INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/metadata/record_set_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/reference_counts/category_record_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/reference_counts/class_record_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/reference_counts/method_record_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/reference_counts/property_record_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/reference_counts/protocol_record_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/reference_counts/source_record_set_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/metadata/source_record_set_declaration_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_category_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_class_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_declaration_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_method_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_property_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_protocol_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_reference_counts.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_runtime_metadata_record_set.h
)
