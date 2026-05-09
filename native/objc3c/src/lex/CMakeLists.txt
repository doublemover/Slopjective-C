add_library(objc3c_lex STATIC
  objc3_lexer_char_class.cpp
  objc3_lexer.cpp
  ../token/objc3_token_kind.cpp
  ../token/objc3_token_punctuation.cpp
)
objc3c_apply_build_defaults(objc3c_lex)
objc3c_apply_llvm_direct_config(objc3c_lex)
target_link_libraries(objc3c_lex PUBLIC
  objc3c_config
  objc3c_diag
  objc3c_support
)
