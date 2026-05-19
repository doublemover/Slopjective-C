#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_container_decl_nodes.h"
#include "ast/objc3_ast_function_decl_nodes.h"
#include "ast/objc3_ast_method_decl_nodes.h"
#include "ast/objc3_ast_property_decl_nodes.h"
#include "token/objc3_token_contract.h"


struct GlobalDecl {
  std::string name;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::string semantic_link_symbol;
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3DraftSyntaxSurfaceSummary {
  std::size_t block_literal_sites = 0;
  std::size_t block_explicit_capture_list_sites = 0;
  std::size_t block_explicit_capture_byref_sites = 0;
  std::size_t block_byref_capture_sites = 0;
  std::size_t block_heap_escape_candidate_sites = 0;
  std::size_t try_expression_sites = 0;
  std::size_t throw_statement_sites = 0;
  std::size_t do_catch_sites = 0;
  std::size_t error_catch_clause_sites = 0;
  std::size_t error_catch_binding_sites = 0;
  std::size_t error_catch_all_sites = 0;
  std::size_t error_bridge_payload_sites = 0;
  std::size_t error_foreign_boundary_annotation_sites = 0;
  std::size_t error_nested_cleanup_marker_sites = 0;
  std::size_t throws_callable_sites = 0;
  std::size_t async_callable_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_nonisolated_callable_sites = 0;
  std::size_t actor_isolation_marker_sites = 0;
  std::size_t actor_sendable_annotation_sites = 0;
  std::size_t task_runtime_construct_sites = 0;
  std::size_t task_cancellation_check_sites = 0;
  std::size_t task_suspension_point_sites = 0;
  std::size_t macro_attribute_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t macro_cache_key_sites = 0;
  std::size_t macro_sandbox_policy_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t property_attribute_sites = 0;
  std::size_t property_accessor_selector_sites = 0;
  std::size_t property_synthesis_metadata_sites = 0;
  std::size_t property_reflection_input_sites = 0;
  std::size_t property_ownership_nullability_sites = 0;
  std::size_t interop_attribute_sites = 0;
  std::size_t interop_import_module_sites = 0;
  std::size_t interop_swift_annotation_sites = 0;
  std::size_t interop_cxx_annotation_sites = 0;
  std::size_t interop_header_import_sites = 0;
  std::size_t interop_header_export_sites = 0;
  std::size_t interop_abi_alignment_sites = 0;
  std::size_t interop_foreign_type_sites = 0;
  std::size_t interop_mixed_image_sites = 0;
  std::size_t interop_package_entry_sites = 0;
  std::size_t interop_error_bridge_sites = 0;
  std::size_t draft_syntax_surface_sites = 0;
  bool normalized = false;
  std::string replay_key;
};

struct Objc3Program {
  std::string module_name = "objc3_module";
  std::vector<GlobalDecl> globals;
  std::vector<Objc3ProtocolDecl> protocols;
  std::vector<Objc3InterfaceDecl> interfaces;
  std::vector<Objc3ImplementationDecl> implementations;
  std::vector<FunctionDecl> functions;
  Objc3DraftSyntaxSurfaceSummary draft_syntax_surface_summary;
  std::vector<std::string> diagnostics;
};
