#pragma once

#include <string>
#include <utility>
#include <vector>

// Lexer output contract consumed across parser/lowering/IR boundaries.
enum class Objc3LexTokenKind {
  Eof,
  Identifier,
  Number,
  KwModule,
  KwLet,
  KwFn,
  KwPure,
  KwExtern,
  KwReturn,
  KwIf,
  KwElse,
  KwDo,
  KwFor,
  KwSwitch,
  KwCase,
  KwDefault,
  KwWhile,
  KwBreak,
  KwContinue,
  KwI32,
  KwBool,
  KwBOOL,
  KwNSInteger,
  KwNSUInteger,
  KwVoid,
  KwId,
  KwClass,
  KwSEL,
  KwProtocol,
  KwInstancetype,
  KwTrue,
  KwFalse,
  KwNil,
  KwAtInterface,
  KwAtImplementation,
  KwAtEnd,
  LParen,
  RParen,
  LBracket,
  RBracket,
  LBrace,
  RBrace,
  Comma,
  Colon,
  Semicolon,
  Equal,
  PlusEqual,
  MinusEqual,
  StarEqual,
  SlashEqual,
  PercentEqual,
  AmpersandEqual,
  PipeEqual,
  CaretEqual,
  LessLessEqual,
  GreaterGreaterEqual,
  PlusPlus,
  MinusMinus,
  EqualEqual,
  Bang,
  BangEqual,
  Less,
  LessLess,
  LessEqual,
  Greater,
  GreaterGreater,
  GreaterEqual,
  Ampersand,
  Pipe,
  Caret,
  AndAnd,
  OrOr,
  Question,
  Tilde,
  Plus,
  Minus,
  Star,
  Slash,
  Percent
};

struct Objc3LexToken {
  Objc3LexTokenKind kind = Objc3LexTokenKind::Eof;
  std::string text;
  unsigned line = 1;
  unsigned column = 1;
};

using Objc3LexTokenStream = std::vector<Objc3LexToken>;

enum class Objc3SemaTokenKind {
  PointerDeclarator,
  NullabilitySuffix,
};

struct Objc3SemaTokenMetadata {
  Objc3SemaTokenKind kind = Objc3SemaTokenKind::PointerDeclarator;
  std::string text;
  unsigned line = 1;
  unsigned column = 1;
};

inline Objc3SemaTokenMetadata MakeObjc3SemaTokenMetadata(Objc3SemaTokenKind kind, std::string text, unsigned line,
                                                         unsigned column) {
  Objc3SemaTokenMetadata metadata;
  metadata.kind = kind;
  metadata.text = std::move(text);
  metadata.line = line;
  metadata.column = column;
  return metadata;
}
