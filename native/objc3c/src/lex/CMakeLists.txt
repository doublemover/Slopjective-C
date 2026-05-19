add_library(objc3c_lex STATIC
  objc3_lexer_char_class.cpp
  objc3_lexer.cpp
  objc3_lexer_pragmas.cpp
  objc3_lexer_run.cpp
  objc3_lexer_scanning.cpp
)
objc3c_apply_build_defaults(objc3c_lex)
objc3c_apply_llvm_direct_config(objc3c_lex)
target_link_libraries(objc3c_lex PUBLIC
  objc3c_config
  objc3c_diag
  objc3c_support
  objc3c_token
)
