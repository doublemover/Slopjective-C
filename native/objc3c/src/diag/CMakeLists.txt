add_library(objc3c_diag STATIC
  objc3_diag_utils.cpp
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)
