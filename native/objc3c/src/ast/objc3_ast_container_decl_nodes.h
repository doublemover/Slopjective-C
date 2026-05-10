#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_method_decl_nodes.h"
#include "ast/objc3_ast_property_decl_nodes.h"

struct Objc3ProtocolDecl {
  std::string name;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<std::string> inherited_protocols;
  std::vector<std::string> inherited_protocols_lexicographic;
  std::string semantic_link_symbol;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
  std::vector<Objc3PropertyDecl> properties;
  std::vector<Objc3MethodDecl> methods;
  bool is_forward_declaration = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3GenericParamDecl {
  std::string name;
  std::string variance_spelling;
  bool has_constraint = false;
  std::string constraint_type_name;
  bool has_constraint_generic_suffix = false;
  bool constraint_generic_suffix_terminated = true;
  std::string constraint_generic_suffix_text;
  unsigned line = 1;
  unsigned column = 1;
  unsigned constraint_line = 1;
  unsigned constraint_column = 1;
};

struct Objc3InterfaceDecl {
  std::string name;
  std::string super_name;
  std::string category_name;
  bool has_category = false;
  bool is_actor = false;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<std::string> adopted_protocols;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::vector<Objc3GenericParamDecl> generic_params;
  std::string semantic_link_symbol;
  std::string semantic_link_super_symbol;
  std::string semantic_link_category_symbol;
  std::vector<std::string> property_synthesis_symbols_lexicographic;
  std::vector<std::string> ivar_binding_symbols_lexicographic;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
  bool prefixed_dispatch_control_attributes_declared = false;
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool objc_derive_declared = false;
  std::string objc_derive_name;
  std::vector<Objc3PropertyDecl> properties;
  std::vector<Objc3MethodDecl> methods;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ImplementationDecl {
  std::string name;
  std::string category_name;
  bool has_category = false;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::string semantic_link_symbol;
  std::string semantic_link_interface_symbol;
  std::string semantic_link_category_symbol;
  std::vector<std::string> property_synthesis_symbols_lexicographic;
  std::vector<std::string> ivar_binding_symbols_lexicographic;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
  std::vector<Objc3PropertyDecl> properties;
  std::vector<Objc3MethodDecl> methods;
  unsigned line = 1;
  unsigned column = 1;
};
