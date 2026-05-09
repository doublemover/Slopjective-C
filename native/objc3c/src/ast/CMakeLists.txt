add_library(objc3c_ast_core INTERFACE)
target_link_libraries(objc3c_ast_core INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_ast_contracts INTERFACE)
target_link_libraries(objc3c_ast_contracts INTERFACE
  objc3c_build_defaults
)

add_library(objc3c_ast_declarations INTERFACE)
target_link_libraries(objc3c_ast_declarations INTERFACE
  objc3c_ast_core
)

add_library(objc3c_ast INTERFACE)
target_link_libraries(objc3c_ast INTERFACE
  objc3c_ast_core
  objc3c_ast_contracts
  objc3c_ast_declarations
)
