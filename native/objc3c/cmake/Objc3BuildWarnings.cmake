# One warning policy for integrated and standalone native implementation builds.
function(objc3c_configure_build_warnings target)
  target_compile_options(${target} INTERFACE
    "$<BUILD_INTERFACE:-Wall;-Wextra;-pedantic;-ffile-prefix-map=${CMAKE_SOURCE_DIR}=.;-fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.>")
endfunction()
