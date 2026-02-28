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
  Kind kind = Kind::Number;
  int number = 0;
  bool bool_value = false;
  std::string ident;
  std::string selector;
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
  bool instancetype_spelling = false;
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
  std::string selector;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_vector_spelling = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool has_return_generic_suffix = false;
  bool return_generic_suffix_terminated = true;
  std::string return_generic_suffix_text;
  unsigned return_generic_line = 1;
  unsigned return_generic_column = 1;
  bool has_return_pointer_declarator = false;
  unsigned return_pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;
  bool is_class_method = false;
  bool has_body = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3InterfaceDecl {
  std::string name;
  std::string super_name;
  std::vector<Objc3MethodDecl> methods;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ImplementationDecl {
  std::string name;
  std::vector<Objc3MethodDecl> methods;
  unsigned line = 1;
  unsigned column = 1;
};

struct FunctionDecl {
  std::string name;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_vector_spelling = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
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
  std::vector<Objc3InterfaceDecl> interfaces;
  std::vector<Objc3ImplementationDecl> implementations;
  std::vector<FunctionDecl> functions;
  std::vector<std::string> diagnostics;
};
