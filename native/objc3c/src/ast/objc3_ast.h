#pragma once

#include <memory>
#include <string>
#include <vector>

#include "token/objc3_token_contract.h"

struct SymbolRow {
  std::string kind;
  std::string name;
  unsigned line;
  unsigned column;
};

struct SymbolContext {
  std::vector<SymbolRow> rows;
};

struct Expr {
  enum class Kind { Number, BoolLiteral, NilLiteral, Identifier, Binary, Conditional, Call, MessageSend };
  enum class MessageSendForm { None, Unary, Keyword };
  struct MessageSendSelectorPiece {
    std::string keyword;
    bool has_argument = false;
    unsigned line = 1;
    unsigned column = 1;
  };
  Kind kind = Kind::Number;
  int number = 0;
  bool bool_value = false;
  std::string ident;
  std::string selector;
  MessageSendForm message_send_form = MessageSendForm::None;
  std::string message_send_form_symbol;
  std::vector<MessageSendSelectorPiece> selector_lowering_pieces;
  std::string selector_lowering_symbol;
  bool selector_lowering_is_normalized = false;
  std::string op = "+";
  std::unique_ptr<Expr> receiver;
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  std::unique_ptr<Expr> third;
  std::vector<std::unique_ptr<Expr>> args;
  unsigned line = 1;
  unsigned column = 1;
};

enum class ValueType { Unknown, I32, Bool, Void, Function };

struct LetStmt;
struct AssignStmt;
struct ReturnStmt;
struct IfStmt;
struct DoWhileStmt;
struct ForStmt;
struct SwitchStmt;
struct WhileStmt;
struct BlockStmt;
struct ExprStmt;

struct Stmt {
  enum class Kind { Let, Assign, Return, If, DoWhile, For, Switch, While, Break, Continue, Empty, Block, Expr };
  Kind kind = Kind::Expr;
  std::unique_ptr<LetStmt> let_stmt;
  std::unique_ptr<AssignStmt> assign_stmt;
  std::unique_ptr<ReturnStmt> return_stmt;
  std::unique_ptr<IfStmt> if_stmt;
  std::unique_ptr<DoWhileStmt> do_while_stmt;
  std::unique_ptr<ForStmt> for_stmt;
  std::unique_ptr<SwitchStmt> switch_stmt;
  std::unique_ptr<WhileStmt> while_stmt;
  std::unique_ptr<BlockStmt> block_stmt;
  std::unique_ptr<ExprStmt> expr_stmt;
  unsigned line = 1;
  unsigned column = 1;
};

struct LetStmt {
  std::string name;
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct AssignStmt {
  std::string name;
  std::string op = "=";
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct ReturnStmt {
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct IfStmt {
  std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> then_body;
  std::vector<std::unique_ptr<Stmt>> else_body;
  unsigned line = 1;
  unsigned column = 1;
};

struct DoWhileStmt {
  std::vector<std::unique_ptr<Stmt>> body;
  std::unique_ptr<Expr> condition;
  unsigned line = 1;
  unsigned column = 1;
};

struct ForClause {
  enum class Kind { None, Let, Assign, Expr };
  Kind kind = Kind::None;
  std::string name;
  std::string op = "=";
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct ForStmt {
  ForClause init;
  std::unique_ptr<Expr> condition;
  ForClause step;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct SwitchCase {
  bool is_default = false;
  int value = 0;
  unsigned value_line = 1;
  unsigned value_column = 1;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct SwitchStmt {
  std::unique_ptr<Expr> condition;
  std::vector<SwitchCase> cases;
  unsigned line = 1;
  unsigned column = 1;
};

struct WhileStmt {
  std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct BlockStmt {
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct ExprStmt {
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct FuncParam {
  std::string name;
  ValueType type = ValueType::I32;
  bool vector_spelling = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  std::string object_pointer_type_name;
  std::string typecheck_family_symbol;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3MethodDecl {
  struct SelectorPiece {
    std::string keyword;
    std::string parameter_name;
    bool has_parameter = false;
    unsigned line = 1;
    unsigned column = 1;
  };

  std::string selector;
  std::vector<SelectorPiece> selector_pieces;
  bool selector_is_normalized = false;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_vector_spelling = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_sel_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  std::string return_object_pointer_type_name;
  std::string return_typecheck_family_symbol;
  bool has_return_generic_suffix = false;
  bool return_generic_suffix_terminated = true;
  std::string return_generic_suffix_text;
  unsigned return_generic_line = 1;
  unsigned return_generic_column = 1;
  bool has_return_pointer_declarator = false;
  unsigned return_pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;
  std::string scope_owner_symbol;
  std::string scope_path_symbol;
  std::string method_lookup_symbol;
  std::string override_lookup_symbol;
  std::string conflict_lookup_symbol;
  bool is_class_method = false;
  bool has_body = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3PropertyAttributeDecl {
  std::string name;
  std::string value;
  bool has_value = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3PropertyDecl {
  std::string name;
  ValueType type = ValueType::Unknown;
  bool vector_spelling = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  std::string object_pointer_type_name;
  std::string typecheck_family_symbol;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;
  std::vector<Objc3PropertyAttributeDecl> attributes;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_assign = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::string scope_owner_symbol;
  std::string scope_path_symbol;
  std::string property_synthesis_symbol;
  std::string ivar_binding_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

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

struct Objc3InterfaceDecl {
  std::string name;
  std::string super_name;
  std::string category_name;
  bool has_category = false;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<std::string> adopted_protocols;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::string semantic_link_symbol;
  std::string semantic_link_super_symbol;
  std::string semantic_link_category_symbol;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
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

struct FunctionDecl {
  std::string name;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_vector_spelling = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_sel_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  std::string return_object_pointer_type_name;
  std::string return_typecheck_family_symbol;
  bool has_return_generic_suffix = false;
  bool return_generic_suffix_terminated = true;
  std::string return_generic_suffix_text;
  unsigned return_generic_line = 1;
  unsigned return_generic_column = 1;
  bool has_return_pointer_declarator = false;
  unsigned return_pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;
  bool is_prototype = false;
  bool is_pure = false;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct GlobalDecl {
  std::string name;
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3Program {
  std::string module_name = "objc3_module";
  std::vector<GlobalDecl> globals;
  std::vector<Objc3ProtocolDecl> protocols;
  std::vector<Objc3InterfaceDecl> interfaces;
  std::vector<Objc3ImplementationDecl> implementations;
  std::vector<FunctionDecl> functions;
  std::vector<std::string> diagnostics;
};
