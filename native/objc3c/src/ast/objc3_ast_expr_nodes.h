#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_value_type.h"

struct Stmt;

#define OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR 1

struct Expr {
#include "ast/objc3_ast_expr_block_contract_members.h"
#include "ast/objc3_ast_expr_arc_contract_members.h"
#include "ast/objc3_ast_expr_kind_members.h"
#include "ast/objc3_ast_expr_message_dispatch_members.h"
#include "ast/objc3_ast_expr_block_members.h"
#include "ast/objc3_ast_expr_control_flow_members.h"
};

#undef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
