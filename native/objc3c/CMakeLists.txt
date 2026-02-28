cmake_minimum_required(VERSION 3.20)
project(objc3c_native LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION
  "Enable llvm-direct object emission backend wiring for .objc3 IR compilation."
  ON
)

# M132-A002 target decomposition contract.
# Current state: monolithic `src/main.cpp`.
# Planned steady-state split:
#   objc3c_lex      -> lex/*
#   objc3c_parse    -> parse/* (depends on objc3c_lex)
#   objc3c_sema     -> sema/* (depends on objc3c_parse)
#   objc3c_lower    -> lower/* (depends on objc3c_sema)
#   objc3c_ir       -> ir/* (depends on objc3c_lower)
#   objc3c_pipeline -> pipeline/* (depends on stage libs)
#   objc3c_frontend -> libobjc3c_frontend/* (depends on objc3c_pipeline)
#   objc3c_driver   -> driver/* + io/* + frontend API
#
# Dependency-direction authority is defined in `src/ARCHITECTURE.md` and checked
# by `scripts/check_objc3c_dependency_boundaries.py`.

add_executable(objc3c-native
  src/main.cpp
)

# Placeholder interface targets so boundary rules can map to concrete target
# names before extraction work lands.
add_library(objc3c_lex STATIC
  src/lex/objc3_lexer.cpp
)
target_include_directories(objc3c_lex PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_library(objc3c_diag STATIC
  src/diag/objc3_diag_utils.cpp
)
target_include_directories(objc3c_diag PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_library(objc3c_parse STATIC
  src/parse/objc3_ast_builder.cpp
  src/parse/objc3_ast_builder_contract.cpp
  src/parse/objc3_parser.cpp
)
target_include_directories(objc3c_parse PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_parse PUBLIC
  objc3c_lex
)
add_library(objc3c_sema STATIC
  src/sema/objc3_sema_diagnostics_bus.cpp
  src/sema/objc3_sema_pass_manager.cpp
  src/sema/objc3_semantic_passes.cpp
  src/sema/objc3_static_analysis.cpp
  src/sema/objc3_pure_contract.cpp
)
target_include_directories(objc3c_sema PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_sema PUBLIC
  objc3c_parse
  objc3c_diag
)
# M141-B001 sema/type-system surface target for downstream topology hardening.
add_library(objc3c_sema_type_system INTERFACE)
target_link_libraries(objc3c_sema_type_system INTERFACE
  objc3c_sema
)
add_library(objc3c_lower STATIC
  src/lower/objc3_lowering_contract.cpp
)
target_include_directories(objc3c_lower PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_lower PUBLIC
  objc3c_sema_type_system
)
add_library(objc3c_ir STATIC
  src/ir/objc3_ir_emitter.cpp
)
target_include_directories(objc3c_ir PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_ir PUBLIC
  objc3c_lower
  objc3c_sema_type_system
)
add_library(objc3c_pipeline STATIC
  src/pipeline/objc3_frontend_pipeline.cpp
  src/pipeline/objc3_frontend_artifacts.cpp
)
target_include_directories(objc3c_pipeline PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_pipeline PUBLIC
  objc3c_parse
  objc3c_sema
  objc3c_lower
  objc3c_ir
)
add_library(objc3c_io STATIC
  src/io/objc3_diagnostics_artifacts.cpp
  src/io/objc3_file_io.cpp
  src/io/objc3_manifest_artifacts.cpp
)
target_include_directories(objc3c_io PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_library(objc3c_runtime_abi STATIC
  src/io/objc3_process.cpp
)
target_include_directories(objc3c_runtime_abi PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if (OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  target_compile_definitions(objc3c_runtime_abi PUBLIC
    OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1
  )
endif()
target_link_libraries(objc3c_io PUBLIC
  objc3c_runtime_abi
)
add_library(objc3c_frontend STATIC
  src/libobjc3c_frontend/c_api.cpp
  src/libobjc3c_frontend/frontend_anchor.cpp
  src/libobjc3c_frontend/objc3_cli_frontend.cpp
)
target_include_directories(objc3c_frontend PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_frontend PUBLIC
  objc3c_pipeline
  objc3c_runtime_abi
)
add_library(objc3c_driver STATIC
  src/driver/objc3_cli_options.cpp
  src/driver/objc3_driver_main.cpp
  src/driver/objc3_driver_shell.cpp
  src/driver/objc3_frontend_options.cpp
  src/driver/objc3_llvm_capability_routing.cpp
  src/driver/objc3_objc3_path.cpp
  src/driver/objc3_objectivec_path.cpp
  src/driver/objc3_compilation_driver.cpp
)
target_include_directories(objc3c_driver PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(objc3c_driver PUBLIC
  objc3c_diag
  objc3c_frontend
  objc3c_io
)

target_link_libraries(objc3c-native PRIVATE
  objc3c_driver
)

add_executable(objc3c-frontend-c-api-runner
  src/tools/objc3c_frontend_c_api_runner.cpp
)
target_link_libraries(objc3c-frontend-c-api-runner PRIVATE
  objc3c_frontend
)
