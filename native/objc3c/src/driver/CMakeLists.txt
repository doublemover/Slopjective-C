add_library(objc3c_driver STATIC
  objc3_cli_options.cpp
  objc3_cli_environment.cpp
  objc3_cli_value_parsers.cpp
  objc3_driver_command_dispatch.cpp
  objc3_driver_frontend_artifact_handoff.cpp
  objc3_driver_object_backend.cpp
  objc3_driver_main.cpp
  objc3_driver_shell.cpp
  objc3_frontend_options.cpp
  objc3_llvm_capability_routing.cpp
  objc3_objectivec_symbol_manifest.cpp
  objc3_objc3_conformance_path.cpp
  objc3_objc3_path.cpp
  objc3_objectivec_path.cpp
  objc3_compilation_driver.cpp
)
objc3c_apply_build_defaults(objc3c_driver)
objc3c_apply_llvm_direct_config(objc3c_driver)
target_link_libraries(objc3c_driver PUBLIC
  objc3c_ast
  objc3c_config
  objc3c_diag
  objc3c_diagnostics_modes
  objc3c_frontend
  objc3c_io
  objc3c_support
)
if (TARGET objc3c_libclang)
  target_link_libraries(objc3c_driver PUBLIC
    objc3c_libclang
  )
endif()
