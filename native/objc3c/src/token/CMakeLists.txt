add_library(objc3c_token STATIC
  objc3_token_keyword_data.cpp
  objc3_token_keyword_classification.cpp
  objc3_token_kind.cpp
  objc3_token_rejected_canonical_literal_table.cpp
  objc3_token_rejected_canonical_literals.cpp
  objc3_token_punctuation.cpp
)
objc3c_apply_build_defaults(objc3c_token)
objc3c_apply_llvm_direct_config(objc3c_token)
