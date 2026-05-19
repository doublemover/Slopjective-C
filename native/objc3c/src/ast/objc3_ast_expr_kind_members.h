#pragma once

#ifndef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
#error "objc3_ast_expr_kind_members.h must be included inside struct Expr"
#endif

  // Canonical extraction anchor retained for strict parser/AST contract tests:
  // enum class Kind { Number, BoolLiteral, NilLiteral, Identifier, Binary, Conditional, Call, MessageSend };
  enum class Kind {
    Number,
    BoolLiteral,
    NilLiteral,
    Identifier,
    KeyPathLiteral,
    Binary,
    Conditional,
    Call,
    Try,
    Throw,
    MessageSend,
    BlockLiteral
  };
  enum class MessageSendForm { None, Unary, Keyword };
  enum class TryOperatorKind { None, Propagate, Optional, Forced };
  enum class DispatchSurfaceKind {
    Unclassified,
    Instance,
    Class,
    Super,
    Direct,
    Dynamic
  };
  struct MessageSendSelectorPiece {
    std::string keyword;
    bool has_argument = false;
    unsigned line = 1;
    unsigned column = 1;
  };
  struct BlockParameter {
    std::string name;
    ValueType type = ValueType::Unknown;
  };
  struct ExplicitBlockCaptureItem {
    std::string name;
    std::string mode;
  };
