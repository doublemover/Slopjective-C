add_library(objc3c_diagnostics_modes STATIC
  modes/canonical_rejections.cpp
)
target_sources(objc3c_diagnostics_modes PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/modes/objc3_mode_option_diagnostic.h
  ${CMAKE_CURRENT_LIST_DIR}/modes/canonical_rejections.h
)
objc3c_apply_build_defaults(objc3c_diagnostics_modes)
objc3c_apply_llvm_direct_config(objc3c_diagnostics_modes)
target_link_libraries(objc3c_diagnostics_modes PUBLIC
  objc3c_config
)
