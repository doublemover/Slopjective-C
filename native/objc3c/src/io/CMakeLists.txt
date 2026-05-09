add_library(objc3c_json STATIC
  json/json_equivalence.cpp
  json/json_equivalence_nodes.cpp
  json/json_error.cpp
  json/json_parser.cpp
  json/json_pointer.cpp
  json/json_pointer_tokens.cpp
  json/json_schema.cpp
  json/json_schema_validation.cpp
  json/json_schema_type.cpp
  json/json_schema_type_names.cpp
  json/json_string_array_writer.cpp
  json/json_value.cpp
  json/json_value_writer.cpp
  json/json_writer.cpp
  objc3_json.cpp
)
objc3c_apply_build_defaults(objc3c_json)
objc3c_apply_llvm_direct_config(objc3c_json)

add_library(objc3c_io STATIC
  objc3_artifact_paths.cpp
  objc3_artifact_writers.cpp
  objc3_conformance_artifact_paths.cpp
  objc3_conformance_artifact_writers.cpp
  objc3_diagnostics_artifact_document.cpp
  objc3_diagnostics_artifacts.cpp
  objc3_file_io.cpp
  objc3_file_payload_io.cpp
  objc3_line_join.cpp
  objc3_runtime_artifact_paths.cpp
  objc3_runtime_artifact_writers.cpp
)
objc3c_apply_build_defaults(objc3c_io)
objc3c_apply_llvm_direct_config(objc3c_io)

add_library(objc3c_runtime_abi STATIC
  objc3_conformance_dashboard_artifact_document.cpp
  objc3_conformance_release_artifact_document.cpp
  objc3_conformance_artifact_documents.cpp
  objc3_object_format.cpp
  objc3_process.cpp
  objc3_runtime_artifact_contracts.cpp
  objc3_runtime_artifact_document_renderers.cpp
  objc3_runtime_artifact_documents.cpp
)
objc3c_apply_build_defaults(objc3c_runtime_abi)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi)
target_link_libraries(objc3c_runtime_abi PUBLIC
  objc3c_json
  objc3c_support
)
target_link_libraries(objc3c_io PUBLIC
  objc3c_diag
  objc3c_json
  objc3c_runtime_abi
  objc3c_support
)
