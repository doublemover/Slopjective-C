#pragma once

#ifndef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
#error "objc3_ast_expr_control_flow_members.h must be included inside struct Expr"
#endif

  bool typed_keypath_literal_enabled = false;
  std::string string_literal_value;
  int string_literal_byte_count = 0;
  int string_literal_unit_count = 0;
  bool string_literal_valid_utf8 = false;
  std::vector<std::string> string_interpolation_segments;
  mutable std::vector<bool> string_interpolation_payload_is_text;
  CollectionLiteralKind collection_literal_kind =
      CollectionLiteralKind::None;
  bool collection_literal_mutable = false;
  std::vector<std::unique_ptr<Expr>> collection_keys;
  std::vector<std::unique_ptr<Expr>> collection_values;
  bool typed_keypath_root_is_self = false;
  std::string typed_keypath_root_name;
  std::vector<std::string> typed_keypath_components;
  std::string typed_keypath_literal_profile;
  bool typed_keypath_literal_is_normalized = false;
  bool try_expression_enabled = false;
  TryOperatorKind try_operator_kind = TryOperatorKind::None;
  bool try_expression_requires_throwing_context = false;
  bool try_expression_is_normalized = false;
  std::string try_expression_profile;
  bool await_expression_enabled = false;
  bool throw_statement_enabled = false;
  bool throw_statement_is_normalized = false;
  std::string throw_statement_profile;
  std::vector<std::unique_ptr<Stmt>> block_body;
  std::string op = "+";
  std::unique_ptr<Expr> receiver;
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  std::unique_ptr<Expr> third;
  std::vector<std::unique_ptr<Expr>> args;
  unsigned line = 1;
  unsigned column = 1;
