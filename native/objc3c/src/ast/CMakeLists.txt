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

add_library(objc3c_ast STATIC
  objc3_ast_block_surface.cpp
  objc3_ast_decl_surface.cpp
  objc3_ast_type_surface.cpp
)
objc3c_apply_build_defaults(objc3c_ast)
target_link_libraries(objc3c_ast PUBLIC
  objc3c_ast_core
  objc3c_ast_contracts
  objc3c_ast_declarations
)
