add_library(objc3c_ast_core INTERFACE)
target_link_libraries(objc3c_ast_core INTERFACE
  objc3c_build_defaults
)
target_sources(objc3c_ast_core INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_callable_type_nodes.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_core.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_source_location.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_symbol_context.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_value_type.h
)

add_library(objc3c_ast_contracts INTERFACE)
target_link_libraries(objc3c_ast_contracts INTERFACE
  objc3c_build_defaults
)
target_sources(objc3c_ast_contracts INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_contracts.h
)

add_library(objc3c_ast_declarations INTERFACE)
target_link_libraries(objc3c_ast_declarations INTERFACE
  objc3c_ast_core
)
target_sources(objc3c_ast_declarations INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_declarations.h
)

add_library(objc3c_ast_ordering STATIC
  objc3_ast_ordering.cpp
)
target_sources(objc3c_ast_ordering PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_ordering.h
)
objc3c_apply_build_defaults(objc3c_ast_ordering)
target_link_libraries(objc3c_ast_ordering PUBLIC
  objc3c_ast_core
)

add_library(objc3c_ast_blocks STATIC
  objc3_ast_block_abi.cpp
  objc3_ast_block_capture.cpp
  objc3_ast_block_determinism.cpp
  objc3_ast_block_queries.cpp
  objc3_ast_block_signature.cpp
  objc3_ast_block_storage.cpp
)
target_sources(objc3c_ast_blocks PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_abi.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_capture.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_determinism.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_queries.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_signature.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_storage.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_ast_block_surface.h
)
objc3c_apply_build_defaults(objc3c_ast_blocks)
target_link_libraries(objc3c_ast_blocks PUBLIC
  objc3c_ast_core
  objc3c_ast_ordering
)

add_library(objc3c_ast STATIC
  objc3_ast_callable_type_surface.cpp
  objc3_ast_function_signature_surface.cpp
  objc3_ast_method_signature_surface.cpp
  objc3_ast_program_surface.cpp
  objc3_ast_property_lowering_surface.cpp
  objc3_ast_scope_lookup.cpp
  objc3_ast_scope_members.cpp
  objc3_ast_scope_paths.cpp
  objc3_ast_scope_semantic_links.cpp
  objc3_ast_value_type_queries.cpp
)
objc3c_apply_build_defaults(objc3c_ast)
target_link_libraries(objc3c_ast PUBLIC
  objc3c_ast_blocks
  objc3c_ast_core
  objc3c_ast_contracts
  objc3c_ast_declarations
  objc3c_ast_ordering
)
