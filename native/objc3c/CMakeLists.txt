cmake_minimum_required(VERSION 3.20)
project(objc3c_native LANGUAGES CXX)

# M276-A001 incremental-build-parity anchor:
# This target graph is the reserved future candidate backend for the incremental
# native build surface, but it is not yet the authoritative default path.
# Later M276 issues must prove parity with scripts/build_objc3c_native.ps1 for:
# - source coverage
# - include/define/link behavior
# - final publication to artifacts/bin and artifacts/lib
# - truthful separation of binary builds from packet generation
#
# M276-A002 incremental-build-parity anchor:
# - compile database emission at `tmp/build-objc3c-native/compile_commands.json`
# - wrapper-owned `LLVM_ROOT` / `libclang` discovery before the default command flips
#
# M276-C001 native-binary-backend anchor:
# - this target graph now acts as the persistent native binary backend under
#   `tmp/build-objc3c-native`
# - canonical executables publish to `artifacts/bin`
# - canonical runtime archives publish to `artifacts/lib`
#
# M276-C001 persistent-cmake-ninja-incremental-backend anchor:
# - this file now carries the live persistent native build graph used by
#   scripts/build_objc3c_native.ps1
# - canonical final executable/archive outputs still publish under
#   `artifacts/bin` and `artifacts/lib`

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION
  "Enable llvm-direct object emission backend wiring for .objc3 IR compilation."
  ON
)
option(OBJC3C_ENABLE_WARNING_PARITY
  "Apply the warning-level parity flags used by scripts/build_objc3c_native.ps1."
  ON
)

set(OBJC3C_LLVM_ROOT "" CACHE PATH "Authoritative LLVM root propagated from the PowerShell wrapper.")
set(OBJC3C_LLVM_INCLUDE_DIR "" CACHE PATH "LLVM include directory resolved by the PowerShell wrapper.")
set(OBJC3C_LIBCLANG_LIBRARY "" CACHE FILEPATH "Path to libclang/clang import library resolved by the PowerShell wrapper.")
set(OBJC3C_RUNTIME_OUTPUT_DIR "" CACHE PATH "Canonical final runtime output directory for objc3c-native executables.")
set(OBJC3C_LIBRARY_OUTPUT_DIR "" CACHE PATH "Canonical final archive output directory for objc3_runtime.lib.")

if (NOT OBJC3C_LLVM_INCLUDE_DIR AND OBJC3C_LLVM_ROOT)
  set(OBJC3C_LLVM_INCLUDE_DIR "${OBJC3C_LLVM_ROOT}/include")
endif()

add_library(objc3c_build_defaults INTERFACE)
target_include_directories(objc3c_build_defaults INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)
if (OBJC3C_LLVM_INCLUDE_DIR)
  target_include_directories(objc3c_build_defaults INTERFACE
    ${OBJC3C_LLVM_INCLUDE_DIR}
  )
endif()
if (OBJC3C_ENABLE_WARNING_PARITY)
  target_compile_options(objc3c_build_defaults INTERFACE
    -Wall
    -Wextra
    -pedantic
  )
endif()

add_library(objc3c_llvm_direct_config INTERFACE)
if (OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  target_compile_definitions(objc3c_llvm_direct_config INTERFACE
    OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1
  )
endif()

if (OBJC3C_LIBCLANG_LIBRARY)
  add_library(objc3c_libclang UNKNOWN IMPORTED)
  set_target_properties(objc3c_libclang PROPERTIES
    IMPORTED_LOCATION "${OBJC3C_LIBCLANG_LIBRARY}"
  )
endif()

function(objc3c_apply_build_defaults target_name)
  target_link_libraries(${target_name} PUBLIC
    objc3c_build_defaults
  )
endfunction()

function(objc3c_apply_llvm_direct_config target_name)
  target_link_libraries(${target_name} PUBLIC
    objc3c_llvm_direct_config
  )
endfunction()

function(objc3c_apply_runtime_output target_name)
  if (OBJC3C_RUNTIME_OUTPUT_DIR)
    set_target_properties(${target_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${OBJC3C_RUNTIME_OUTPUT_DIR}"
    )
  endif()
endfunction()

function(objc3c_apply_archive_output target_name)
  if (OBJC3C_LIBRARY_OUTPUT_DIR)
    set_target_properties(${target_name} PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${OBJC3C_LIBRARY_OUTPUT_DIR}"
    )
  endif()
endfunction()

set(OBJC3_RUNTIME_LIBRARY_TARGET objc3_runtime)
set(OBJC3_RUNTIME_LIBRARY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/runtime)
set(OBJC3_RUNTIME_LIBRARY_PUBLIC_HEADER ${OBJC3_RUNTIME_LIBRARY_SOURCE_DIR}/objc3_runtime.h)
set(OBJC3_RUNTIME_LIBRARY_KIND STATIC)
set(OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME objc3_runtime)
# M251-D001 freeze: reserve the canonical in-tree native runtime library
# target/header/source-root surface before M251-D002 lands the actual library
# skeleton and M251-D003 wires the driver/link path to consume it.

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
objc3c_apply_build_defaults(objc3c-native)
objc3c_apply_llvm_direct_config(objc3c-native)
objc3c_apply_runtime_output(objc3c-native)

# Placeholder interface targets so boundary rules can map to concrete target
# names before extraction work lands.
add_library(objc3c_lex STATIC
  src/lex/objc3_lexer.cpp
)
objc3c_apply_build_defaults(objc3c_lex)
objc3c_apply_llvm_direct_config(objc3c_lex)

add_library(objc3c_diag STATIC
  src/diag/objc3_diag_utils.cpp
)
objc3c_apply_build_defaults(objc3c_diag)
objc3c_apply_llvm_direct_config(objc3c_diag)

add_library(objc3c_parse STATIC
  src/parse/objc3_ast_builder.cpp
  src/parse/objc3_ast_builder_contract.cpp
  src/parse/objc3_diagnostic_grammar_hooks_core_feature.cpp
  src/parse/objc3_diagnostic_source_precision_scaffold.cpp
  src/parse/objc3_parse_support.cpp
  src/parse/objc3_parser.cpp
)
objc3c_apply_build_defaults(objc3c_parse)
objc3c_apply_llvm_direct_config(objc3c_parse)
target_link_libraries(objc3c_parse PUBLIC
  objc3c_lex
)

add_library(objc3c_sema STATIC
  src/sema/objc3_sema_diagnostics_bus.cpp
  src/sema/objc3_sema_pass_flow_scaffold.cpp
  src/sema/objc3_sema_pass_manager.cpp
  src/sema/objc3_semantic_passes.cpp
  src/sema/objc3_type_form_scaffold.cpp
  src/sema/objc3_static_analysis.cpp
  src/sema/objc3_pure_contract.cpp
)
objc3c_apply_build_defaults(objc3c_sema)
objc3c_apply_llvm_direct_config(objc3c_sema)
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
objc3c_apply_build_defaults(objc3c_lower)
objc3c_apply_llvm_direct_config(objc3c_lower)
target_link_libraries(objc3c_lower PUBLIC
  objc3c_sema_type_system
)

add_library(objc3c_ir STATIC
  src/ir/objc3_ir_emitter.cpp
)
objc3c_apply_build_defaults(objc3c_ir)
objc3c_apply_llvm_direct_config(objc3c_ir)
target_link_libraries(objc3c_ir PUBLIC
  objc3c_lower
  objc3c_sema_type_system
)

add_library(objc3c_pipeline STATIC
  src/pipeline/objc3_frontend_pipeline.cpp
  src/pipeline/objc3_frontend_artifacts.cpp
  src/pipeline/objc3_runtime_import_surface.cpp
  src/pipeline/objc3_ir_emission_completeness_scaffold.cpp
  src/pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp
  src/pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp
)
objc3c_apply_build_defaults(objc3c_pipeline)
objc3c_apply_llvm_direct_config(objc3c_pipeline)
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
objc3c_apply_build_defaults(objc3c_io)
objc3c_apply_llvm_direct_config(objc3c_io)

add_library(objc3_runtime STATIC
  src/runtime/objc3_runtime.cpp
  src/runtime/objc3_runtime.h
)
objc3c_apply_build_defaults(objc3_runtime)
set_target_properties(objc3_runtime PROPERTIES
  OUTPUT_NAME ${OBJC3_RUNTIME_LIBRARY_ARCHIVE_BASENAME}
)
objc3c_apply_archive_output(objc3_runtime)
add_library(objc3c::runtime ALIAS objc3_runtime)

add_library(objc3c_runtime_abi STATIC
  src/io/objc3_process.cpp
)
objc3c_apply_build_defaults(objc3c_runtime_abi)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi)
target_link_libraries(objc3c_io PUBLIC
  objc3c_runtime_abi
)

add_library(objc3c_frontend STATIC
  src/libobjc3c_frontend/c_api.cpp
  src/libobjc3c_frontend/frontend_anchor.cpp
  src/libobjc3c_frontend/objc3_cli_frontend.cpp
)
objc3c_apply_build_defaults(objc3c_frontend)
objc3c_apply_llvm_direct_config(objc3c_frontend)
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
objc3c_apply_build_defaults(objc3c_driver)
objc3c_apply_llvm_direct_config(objc3c_driver)
target_link_libraries(objc3c_driver PUBLIC
  objc3c_diag
  objc3c_frontend
  objc3c_io
)
if (TARGET objc3c_libclang)
  target_link_libraries(objc3c_driver PUBLIC
    objc3c_libclang
  )
endif()

target_link_libraries(objc3c-native PRIVATE
  objc3c_driver
)

add_executable(objc3c-frontend-c-api-runner
  src/tools/objc3c_frontend_c_api_runner.cpp
)
objc3c_apply_build_defaults(objc3c-frontend-c-api-runner)
objc3c_apply_llvm_direct_config(objc3c-frontend-c-api-runner)
objc3c_apply_runtime_output(objc3c-frontend-c-api-runner)
target_link_libraries(objc3c-frontend-c-api-runner PRIVATE
  objc3c_driver
)
