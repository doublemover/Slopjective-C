add_library(objc3c_support STATIC
  objc3_ascii_predicates.h
  objc3_concurrency_symbol_profiles.h
  objc3_file_reading.h
  objc3_identifier_safe_suffix.h
  objc3_ir_object_backend_token.h
  objc3_method_family.h
  objc3_property_storage_profile_helpers.h
  objc3_receiver_profile_helpers.h
  objc3_runtime_dispatch_symbol.h
  objc3_runtime_metadata_record_set.h
  objc3_string_predicates.h
  objc3_type_profile_helpers.h
  objc3_value_type_names.h
  selectors/selector_normalization.cpp
  selectors/selector_normalization.h
)
objc3c_apply_build_defaults(objc3c_support)
target_link_libraries(objc3c_support PUBLIC
  objc3c_ast_core
)
