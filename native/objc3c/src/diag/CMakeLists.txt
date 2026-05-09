add_library(objc3c_diag STATIC
  objc3_diag_format.cpp
  objc3_diag_sort.cpp
  objc3_diag_catalog.cpp
  objc3_diag_parse.cpp
  objc3_diag_text.cpp
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)
