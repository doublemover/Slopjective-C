add_library(objc3c_parse STATIC
  objc3_ast_builder.cpp
  objc3_ast_builder_contract.cpp
  objc3_diagnostic_grammar_hooks_core_feature.cpp
  objc3_diagnostic_source_precision_scaffold.cpp
  objc3_message_send_profiles.cpp
  objc3_parse_support.cpp
  objc3_parser.cpp
)
objc3c_apply_build_defaults(objc3c_parse)
objc3c_apply_llvm_direct_config(objc3c_parse)
target_link_libraries(objc3c_parse PUBLIC
  objc3c_lex
  objc3c_diag
)
