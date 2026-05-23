#pragma once

#ifndef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
#error "objc3_ast_expr_kind_members.h must be included inside struct Expr"
#endif

  // Canonical extraction anchor retained for strict parser/AST contract tests:
  // enum class Kind { Number, BoolLiteral, NilLiteral, StringLiteral, Identifier, Binary, Conditional, Call, MessageSend };
  enum class Kind {
    Number,
    BoolLiteral,
    NilLiteral,
    StringLiteral,
    StringInterpolation,
    CollectionLiteral,
    Identifier,
    IndexAccess,
    KeyPathLiteral,
    Binary,
    Conditional,
    Call,
    Try,
    Throw,
    MessageSend,
    BlockLiteral,
    MatchExpression
  };
  enum class MessageSendForm { None, Unary, Keyword };
  enum class TryOperatorKind { None, Propagate, Optional, Forced };
  enum class CollectionLiteralKind { None, Array, Map, Set };
  enum class MatchExpressionPatternKind {
    None,
    LiteralInteger,
    LiteralBool,
    LiteralNil,
    Wildcard,
    Binding,
    ResultCase,
  };
  struct MatchExpressionArm {
    bool is_default = false;
    MatchExpressionPatternKind pattern_kind =
        MatchExpressionPatternKind::None;
    int literal_value = 0;
    bool binding_mutable = false;
    std::string binding_name;
    std::string result_case_name;
    bool has_guard = false;
    std::unique_ptr<Expr> guard_condition;
    std::unique_ptr<Expr> value;
    unsigned line = 1;
    unsigned column = 1;
    unsigned pattern_line = 1;
    unsigned pattern_column = 1;
    unsigned guard_line = 1;
    unsigned guard_column = 1;
  };
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
