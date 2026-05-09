add_library(objc3c_config INTERFACE)
target_link_libraries(objc3c_config INTERFACE
  objc3c_build_defaults
)

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

# Dependency-direction authority is defined in `ARCHITECTURE.md` and checked
# by `scripts/check_objc3c_dependency_boundaries.py`. The hard-cutover graph uses
# concrete stage targets below while module extraction continues.

add_executable(objc3c-native
  main.cpp
)
objc3c_apply_build_defaults(objc3c-native)
objc3c_apply_llvm_direct_config(objc3c-native)
objc3c_apply_runtime_output(objc3c-native)

# Placeholder interface targets so boundary rules can map to concrete target
# names before extraction work lands.
add_library(objc3c_lex STATIC
  lex/objc3_lexer.cpp
)
objc3c_apply_build_defaults(objc3c_lex)
objc3c_apply_llvm_direct_config(objc3c_lex)

add_library(objc3c_diag STATIC
  diag/objc3_diag_utils.cpp
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)

target_link_libraries(objc3c_lex PUBLIC
  objc3c_config
  objc3c_diag
)

add_subdirectory(parse)
add_subdirectory(sema)

add_subdirectory(lower)

add_subdirectory(ir)

add_subdirectory(io)

add_subdirectory(pipeline)

add_subdirectory(artifacts)

add_subdirectory(runtime)

add_library(objc3c_diagnostics_modes STATIC
  diagnostics/modes/objc3_removed_mode_options.cpp
)
objc3c_apply_build_defaults(objc3c_diagnostics_modes)
objc3c_apply_llvm_direct_config(objc3c_diagnostics_modes)

add_library(objc3c_frontend STATIC
  libobjc3c_frontend/c_api.cpp
  libobjc3c_frontend/frontend_anchor.cpp
  libobjc3c_frontend/objc3_cli_frontend.cpp
)
objc3c_apply_build_defaults(objc3c_frontend)
objc3c_apply_llvm_direct_config(objc3c_frontend)
target_link_libraries(objc3c_frontend PUBLIC
  objc3c_artifacts
  objc3c_pipeline
  objc3c_runtime_abi
  objc3c_json
)

add_library(objc3c_driver STATIC
  driver/objc3_cli_options.cpp
  driver/objc3_driver_main.cpp
  driver/objc3_driver_shell.cpp
  driver/objc3_frontend_options.cpp
  driver/objc3_llvm_capability_routing.cpp
  driver/objc3_objc3_conformance_path.cpp
  driver/objc3_objc3_path.cpp
  driver/objc3_objectivec_path.cpp
  driver/objc3_compilation_driver.cpp
)
objc3c_apply_build_defaults(objc3c_driver)
objc3c_apply_llvm_direct_config(objc3c_driver)
target_link_libraries(objc3c_driver PUBLIC
  objc3c_config
  objc3c_diag
  objc3c_diagnostics_modes
  objc3c_frontend
  objc3c_io
)
if (TARGET objc3c_libclang)
  target_link_libraries(objc3c_driver PUBLIC
    objc3c_libclang
  )
endif()

target_link_libraries(objc3c-native PRIVATE
  objc3c_driver
)

add_executable(objc3c_tools_frontend_c_api_runner
  tools/objc3c_frontend_c_api_runner.cpp
)
objc3c_apply_build_defaults(objc3c_tools_frontend_c_api_runner)
objc3c_apply_llvm_direct_config(objc3c_tools_frontend_c_api_runner)
objc3c_apply_runtime_output(objc3c_tools_frontend_c_api_runner)
set_target_properties(objc3c_tools_frontend_c_api_runner PROPERTIES
  OUTPUT_NAME objc3c-frontend-c-api-runner
)
target_link_libraries(objc3c_tools_frontend_c_api_runner PRIVATE
  objc3c_frontend
  objc3c_driver
  objc3c_diagnostics_modes
  objc3c_json
)
