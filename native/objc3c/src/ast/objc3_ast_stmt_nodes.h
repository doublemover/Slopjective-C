#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_expr_nodes.h"

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
  enum class Kind {
    Let,
    Assign,
    Return,
    If,
    DoWhile,
    For,
    Switch,
    While,
    Break,
    Continue,
    Empty,
    Block,
    Defer,
    Expr
  };
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
  bool cleanup_attribute_declared = false;
  bool cleanup_sugar_declared = false;
  std::string cleanup_function_symbol;
  bool resource_attribute_declared = false;
  bool resource_sugar_declared = false;
  std::string resource_close_symbol;
  std::string resource_invalid_expression;
  bool cleanup_profile_is_normalized = false;
  std::string cleanup_profile;
  bool resource_profile_is_normalized = false;
  std::string resource_profile;
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
  bool optional_binding_surface_enabled = false;
  bool guard_binding_surface_enabled = false;
  bool guard_condition_list_surface_enabled = false;
  std::size_t optional_binding_clause_count = 0;
  std::size_t guard_boolean_condition_clause_count = 0;
  std::vector<std::unique_ptr<Expr>> guard_condition_exprs;
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

enum class MatchPatternKind {
  None,
  LiteralInteger,
  LiteralBool,
  LiteralNil,
  Wildcard,
  Binding,
  ResultCase,
};

struct SwitchCase {
  bool is_default = false;
  int value = 0;
  unsigned value_line = 1;
  unsigned value_column = 1;
  bool match_pattern_enabled = false;
  MatchPatternKind match_pattern_kind = MatchPatternKind::None;
  bool match_binding_mutable = false;
  std::string match_binding_name;
  std::string match_result_case_name;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct SwitchStmt {
  std::unique_ptr<Expr> condition;
  std::vector<SwitchCase> cases;
  bool match_surface_enabled = false;
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
  struct CatchClause {
    bool catch_all = true;
    bool has_binding = false;
    std::string binding_name;
    std::string binding_type_spelling = "id<Error>";
    std::vector<std::unique_ptr<Stmt>> body;
    unsigned line = 1;
    unsigned column = 1;
  };
  std::vector<std::unique_ptr<Stmt>> body;
  bool is_autoreleasepool_scope = false;
  bool is_do_catch_scope = false;
  bool do_catch_is_normalized = false;
  std::vector<CatchClause> catch_clauses;
  std::string do_catch_profile;
  std::string autoreleasepool_scope_symbol;
  unsigned autoreleasepool_scope_depth = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct ExprStmt {
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};
