add_library(objc3c_config STATIC
  objc3_command_option_keys.cpp
  objc3_config_text.cpp
  objc3_config_unsigned.cpp
  objc3_config_validation_accepted.cpp
  objc3_config_validation_invalid.cpp
  objc3_config_validation_rejected.cpp
  objc3_feature_state_catalog.cpp
  objc3_feature_state_core_data.cpp
  objc3_feature_state_names.cpp
  objc3_feature_state_predicates.cpp
  objc3_feature_state_query.cpp
  objc3_feature_state_table_contract.cpp
  objc3_feature_state_tables.cpp
  objc3_language_profile_data.cpp
  objc3_language_profile_names.cpp
  objc3_language_profile_query.cpp
  objc3_language_profile_validation.cpp
  objc3_language_version_messages.cpp
  objc3_language_version_parse.cpp
  objc3_language_version_validation.cpp
  objc3_removed_command_option_list.cpp
  objc3_removed_command_option_query.cpp
  objc3_removed_command_option_table.cpp
  objc3_removed_command_option_validation.cpp
  objc3_rejected_command_option_language_data.cpp
  objc3_rejected_command_option_reporting_data.cpp
  objc3_rejected_command_option_runtime_data.cpp
  objc3_rejected_feature_state_data.cpp
)
objc3c_apply_build_defaults(objc3c_config)

add_library(objc3c_pipeline_results INTERFACE)
target_link_libraries(objc3c_pipeline_results INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_sema_model INTERFACE)
target_link_libraries(objc3c_sema_model INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_lower_model INTERFACE)
target_link_libraries(objc3c_lower_model INTERFACE
  objc3c_build_defaults
)
