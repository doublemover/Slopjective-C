function(objc3c_configure_compiler_options target_name)
  target_compile_features(${target_name} PUBLIC cxx_std_20)
  set_target_properties(${target_name} PROPERTIES
    CXX_EXTENSIONS OFF
  )
endfunction()
