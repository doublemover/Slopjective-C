add_library(objc3c_diag_base STATIC
  objc3_diag_category_code.cpp
  objc3_diag_category_frontend.cpp
  objc3_diag_category_names.cpp
  objc3_diag_category_subsystem.cpp
  objc3_diag_code_membership.cpp
  objc3_diag_code_parse.cpp
  objc3_diag_coordinate.cpp
  objc3_diag_severity_failure.cpp
  objc3_diag_severity_parse.cpp
  objc3_diag_severity_rank.cpp
  objc3_diag_severity_spelling.cpp
  objc3_diag_subsystem_names.cpp
  objc3_diag_subsystem_prefix.cpp
  objc3_diag_text_case.cpp
  objc3_diag_text_prefix.cpp
)
target_sources(objc3c_diag_base PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_category.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_code.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_coordinate.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_severity.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_text.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_types.h
)
objc3c_apply_build_defaults(objc3c_diag_base)
objc3c_apply_llvm_direct_config(objc3c_diag_base)

add_library(objc3c_diag_catalog STATIC
  objc3_diag_catalog_artifact_tooling_data.cpp
  objc3_diag_catalog_family.cpp
  objc3_diag_catalog_frontend_data.cpp
  objc3_diag_catalog_lookup.cpp
  objc3_diag_catalog_membership.cpp
  objc3_diag_catalog_runtime_api_data.cpp
  objc3_diag_catalog_summary.cpp
  objc3_diag_catalog_table.cpp
)
target_sources(objc3c_diag_catalog PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_catalog.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_catalog_artifact_tooling_data.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_catalog_contract.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_catalog_frontend_data.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_catalog_runtime_api_data.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_catalog_table.h
)
objc3c_apply_build_defaults(objc3c_diag_catalog)
objc3c_apply_llvm_direct_config(objc3c_diag_catalog)
target_link_libraries(objc3c_diag_catalog PUBLIC
  objc3c_diag_base
)

add_library(objc3c_diag STATIC
  objc3_diag_format.cpp
  objc3_diag_parse_coordinate_code.cpp
  objc3_diag_parse_rendered.cpp
  objc3_diag_parse_segments.cpp
  objc3_diag_record_factory.cpp
  objc3_diag_record_validation.cpp
  objc3_diag_render.cpp
  objc3_diag_route.cpp
  objc3_diag_sink.cpp
  objc3_diag_sort_key.cpp
  objc3_diag_sort_normalize.cpp
)
target_sources(objc3c_diag PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_format.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_parse.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_record.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_render.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_route.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_sink.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_sort.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_diag_utils.h
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)
target_link_libraries(objc3c_diag PUBLIC
  objc3c_diag_base
  objc3c_diag_catalog
)
