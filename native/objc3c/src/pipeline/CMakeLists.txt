add_library(objc3c_pipeline STATIC
  objc3_frontend_pipeline.cpp
  objc3_runtime_import_surface.cpp
  objc3_typed_sema_to_lowering_contract_surface_core.h
  objc3_typed_sema_to_lowering_contract_surface.h
  objc3_ir_emission_completeness_scaffold.cpp
  objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp
  objc3_lowering_pipeline_pass_graph_scaffold.cpp
)
objc3c_apply_build_defaults(objc3c_pipeline)
objc3c_apply_llvm_direct_config(objc3c_pipeline)
target_link_libraries(objc3c_pipeline PUBLIC
  objc3c_parse
  objc3c_sema
  objc3c_lower
  objc3c_ir
  objc3c_diag
  objc3c_json
  objc3c_pipeline_results
  objc3c_support
)
