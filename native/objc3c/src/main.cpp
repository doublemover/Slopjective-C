#include <clang-c/Index.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <limits>
#include <map>
#include <process.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

struct SymbolRow {
  std::string kind;
  std::string name;
  unsigned line;
  unsigned column;
};

struct SymbolContext {
  std::vector<SymbolRow> rows;
};

enum class TokenKind {
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

struct Token {
  TokenKind kind;
  std::string text;
  unsigned line;
  unsigned column;
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
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  std::vector<Token> nullability_suffix_tokens;
  unsigned line = 1;
  unsigned column = 1;
};

struct FunctionDecl {
  std::string name;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool has_return_generic_suffix = false;
  bool return_generic_suffix_terminated = true;
  std::string return_generic_suffix_text;
  unsigned return_generic_line = 1;
  unsigned return_generic_column = 1;
  std::vector<Token> return_nullability_suffix_tokens;
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
  std::vector<FunctionDecl> functions;
  std::vector<std::string> diagnostics;
};

static std::string ToString(CXString value) {
  const char *raw = clang_getCString(value);
  std::string text = raw == nullptr ? "" : std::string(raw);
  clang_disposeString(value);
  return text;
}

static bool IsIdentStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

static bool IsIdentBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

static bool IsHexDigit(char c) {
  return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

static bool IsBinaryDigit(char c) {
  return c == '0' || c == '1';
}

static bool IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

static bool IsDigitSeparator(char c) {
  return c == '_';
}

static bool IsDigitForBase(char c, int base) {
  switch (base) {
  case 2:
    return IsBinaryDigit(c);
  case 8:
    return IsOctalDigit(c);
  case 10:
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  case 16:
    return IsHexDigit(c);
  default:
    return false;
  }
}

static bool NormalizeIntegerDigits(const std::string &digits, int base, std::string &normalized) {
  normalized.clear();
  if (digits.empty()) {
    return false;
  }

  bool previous_was_digit = false;
  for (std::size_t i = 0; i < digits.size(); ++i) {
    const char c = digits[i];
    if (IsDigitSeparator(c)) {
      if (!previous_was_digit || i + 1 >= digits.size() || !IsDigitForBase(digits[i + 1], base)) {
        return false;
      }
      previous_was_digit = false;
      continue;
    }

    if (!IsDigitForBase(c, base)) {
      return false;
    }
    normalized.push_back(c);
    previous_was_digit = true;
  }

  return !normalized.empty() && previous_was_digit;
}

static std::string ToLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return value;
}

static bool ParseIntegerLiteralValue(const std::string &text, int &value) {
  if (text.empty()) {
    return false;
  }

  int base = 10;
  std::string digit_text = text;
  if (text.size() > 2 && text[0] == '0' && (text[1] == 'b' || text[1] == 'B')) {
    base = 2;
    digit_text = text.substr(2);
  } else if (text.size() > 2 && text[0] == '0' && (text[1] == 'o' || text[1] == 'O')) {
    base = 8;
    digit_text = text.substr(2);
  } else if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
    base = 16;
    digit_text = text.substr(2);
  }

  std::string normalized_digits;
  if (!NormalizeIntegerDigits(digit_text, base, normalized_digits)) {
    return false;
  }

  errno = 0;
  char *end = nullptr;
  const char *raw = normalized_digits.c_str();
  const long parsed = std::strtol(raw, &end, base);
  if (end == raw || end == nullptr || *end != '\0' || errno == ERANGE ||
      parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max()) {
    return false;
  }

  value = static_cast<int>(parsed);
  return true;
}

static bool IsRuntimeDispatchSymbolStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

static bool IsRuntimeDispatchSymbolBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

static bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol[0])) {
    return false;
  }
  for (std::size_t i = 1; i < symbol.size(); ++i) {
    if (!IsRuntimeDispatchSymbolBody(symbol[i])) {
      return false;
    }
  }
  return true;
}

static const char *TypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    default:
      return "unknown";
  }
}

static bool IsCompoundAssignmentOperator(const std::string &op) {
  return op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=" || op == "&=" || op == "|=" ||
         op == "^=" || op == "<<=" || op == ">>=";
}

static bool TryGetCompoundAssignmentBinaryOpcode(const std::string &op, std::string &opcode) {
  if (op == "+=") {
    opcode = "add";
    return true;
  }
  if (op == "-=") {
    opcode = "sub";
    return true;
  }
  if (op == "*=") {
    opcode = "mul";
    return true;
  }
  if (op == "/=") {
    opcode = "sdiv";
    return true;
  }
  if (op == "%=") {
    opcode = "srem";
    return true;
  }
  if (op == "&=") {
    opcode = "and";
    return true;
  }
  if (op == "|=") {
    opcode = "or";
    return true;
  }
  if (op == "^=") {
    opcode = "xor";
    return true;
  }
  if (op == "<<=") {
    opcode = "shl";
    return true;
  }
  if (op == ">>=") {
    opcode = "ashr";
    return true;
  }
  return false;
}

static std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

struct DiagSortKey {
  unsigned severity_rank = std::numeric_limits<unsigned>::max();
  std::string severity = "unknown";
  unsigned line = std::numeric_limits<unsigned>::max();
  unsigned column = std::numeric_limits<unsigned>::max();
  std::string code;
  std::string message;
  std::string raw;
};

static unsigned DiagSeverityRank(const std::string &severity) {
  const std::string normalized = ToLower(severity);
  if (normalized == "fatal") {
    return 0;
  }
  if (normalized == "error") {
    return 1;
  }
  if (normalized == "warning") {
    return 2;
  }
  if (normalized == "note") {
    return 3;
  }
  if (normalized == "ignored") {
    return 4;
  }
  return 5;
}

static bool IsNativeDiagCode(const std::string &candidate) {
  if (candidate.size() != 6) {
    return false;
  }
  if (candidate[0] != 'O' || candidate[1] != '3') {
    return false;
  }
  if (std::isupper(static_cast<unsigned char>(candidate[2])) == 0) {
    return false;
  }
  return std::isdigit(static_cast<unsigned char>(candidate[3])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[4])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[5])) != 0;
}

static DiagSortKey ParseDiagSortKey(const std::string &diag) {
  DiagSortKey key;
  key.raw = diag;

  const std::size_t severity_end = diag.find(':');
  if (severity_end == std::string::npos) {
    key.message = diag;
    return key;
  }
  key.severity = diag.substr(0, severity_end);
  key.severity_rank = DiagSeverityRank(key.severity);

  const std::size_t line_end = diag.find(':', severity_end + 1);
  const std::size_t column_end =
      line_end == std::string::npos ? std::string::npos : diag.find(':', line_end + 1);
  if (line_end == std::string::npos || column_end == std::string::npos) {
    key.message = diag;
    return key;
  }

  try {
    key.line = static_cast<unsigned>(std::stoul(diag.substr(severity_end + 1, line_end - (severity_end + 1))));
    key.column =
        static_cast<unsigned>(std::stoul(diag.substr(line_end + 1, column_end - (line_end + 1))));
  } catch (...) {
    key.line = std::numeric_limits<unsigned>::max();
    key.column = std::numeric_limits<unsigned>::max();
  }

  std::size_t message_begin = column_end + 1;
  while (message_begin < diag.size() && std::isspace(static_cast<unsigned char>(diag[message_begin])) != 0) {
    ++message_begin;
  }
  const std::size_t code_begin = diag.rfind(" [");
  if (code_begin != std::string::npos && code_begin > message_begin && diag.back() == ']') {
    const std::string candidate_code = diag.substr(code_begin + 2, diag.size() - (code_begin + 3));
    if (IsNativeDiagCode(candidate_code)) {
      key.message = diag.substr(message_begin, code_begin - message_begin);
      key.code = candidate_code;
      return key;
    }
  }
  key.message = diag.substr(message_begin);
  return key;
}

static void NormalizeDiagnostics(std::vector<std::string> &diagnostics) {
  std::vector<DiagSortKey> rows;
  rows.reserve(diagnostics.size());
  for (const auto &diag : diagnostics) {
    rows.push_back(ParseDiagSortKey(diag));
  }

  std::stable_sort(rows.begin(), rows.end(), [](const DiagSortKey &a, const DiagSortKey &b) {
    if (a.line != b.line) {
      return a.line < b.line;
    }
    if (a.column != b.column) {
      return a.column < b.column;
    }
    if (a.severity_rank != b.severity_rank) {
      return a.severity_rank < b.severity_rank;
    }
    if (a.code != b.code) {
      return a.code < b.code;
    }
    if (a.message != b.message) {
      return a.message < b.message;
    }
    return a.raw < b.raw;
  });

  diagnostics.clear();
  diagnostics.reserve(rows.size());
  for (const auto &row : rows) {
    if (diagnostics.empty() || diagnostics.back() != row.raw) {
      diagnostics.push_back(row.raw);
    }
  }
}

class Objc3Lexer {
 public:
  explicit Objc3Lexer(const std::string &source) : source_(source) {}

  std::vector<Token> Run(std::vector<std::string> &diagnostics) {
    std::vector<Token> tokens;
    while (true) {
      SkipTrivia(diagnostics);
      if (index_ >= source_.size()) {
        tokens.push_back(Token{TokenKind::Eof, "", line_, column_});
        break;
      }

      const unsigned token_line = line_;
      const unsigned token_column = column_;
      const char c = source_[index_];
      if (IsIdentStart(c)) {
        std::string ident = ConsumeIdentifier();
        TokenKind kind = TokenKind::Identifier;
        if (ident == "module") {
          kind = TokenKind::KwModule;
        } else if (ident == "let") {
          kind = TokenKind::KwLet;
        } else if (ident == "fn") {
          kind = TokenKind::KwFn;
        } else if (ident == "pure") {
          kind = TokenKind::KwPure;
        } else if (ident == "extern") {
          kind = TokenKind::KwExtern;
        } else if (ident == "return") {
          kind = TokenKind::KwReturn;
        } else if (ident == "if") {
          kind = TokenKind::KwIf;
        } else if (ident == "else") {
          kind = TokenKind::KwElse;
        } else if (ident == "do") {
          kind = TokenKind::KwDo;
        } else if (ident == "for") {
          kind = TokenKind::KwFor;
        } else if (ident == "switch") {
          kind = TokenKind::KwSwitch;
        } else if (ident == "case") {
          kind = TokenKind::KwCase;
        } else if (ident == "default") {
          kind = TokenKind::KwDefault;
        } else if (ident == "while") {
          kind = TokenKind::KwWhile;
        } else if (ident == "break") {
          kind = TokenKind::KwBreak;
        } else if (ident == "continue") {
          kind = TokenKind::KwContinue;
        } else if (ident == "i32") {
          kind = TokenKind::KwI32;
        } else if (ident == "bool") {
          kind = TokenKind::KwBool;
        } else if (ident == "BOOL") {
          kind = TokenKind::KwBOOL;
        } else if (ident == "NSInteger") {
          kind = TokenKind::KwNSInteger;
        } else if (ident == "NSUInteger") {
          kind = TokenKind::KwNSUInteger;
        } else if (ident == "void") {
          kind = TokenKind::KwVoid;
        } else if (ident == "id") {
          kind = TokenKind::KwId;
        } else if (ident == "Class") {
          kind = TokenKind::KwClass;
        } else if (ident == "SEL") {
          kind = TokenKind::KwSEL;
        } else if (ident == "Protocol") {
          kind = TokenKind::KwProtocol;
        } else if (ident == "instancetype") {
          kind = TokenKind::KwInstancetype;
        } else if (ident == "true") {
          kind = TokenKind::KwTrue;
        } else if (ident == "false") {
          kind = TokenKind::KwFalse;
        } else if (ident == "nil") {
          kind = TokenKind::KwNil;
        } else if (ident == "YES") {
          kind = TokenKind::KwTrue;
        } else if (ident == "NO") {
          kind = TokenKind::KwFalse;
        } else if (ident == "NULL") {
          kind = TokenKind::KwNil;
        }
        tokens.push_back(Token{kind, ident, token_line, token_column});
        continue;
      }

      if (std::isdigit(static_cast<unsigned char>(c)) != 0) {
        tokens.push_back(Token{TokenKind::Number, ConsumeNumber(), token_line, token_column});
        continue;
      }

      Advance();
      switch (c) {
        case '(':
          tokens.push_back(Token{TokenKind::LParen, "(", token_line, token_column});
          break;
        case ')':
          tokens.push_back(Token{TokenKind::RParen, ")", token_line, token_column});
          break;
        case '[':
          tokens.push_back(Token{TokenKind::LBracket, "[", token_line, token_column});
          break;
        case ']':
          tokens.push_back(Token{TokenKind::RBracket, "]", token_line, token_column});
          break;
        case '{':
          tokens.push_back(Token{TokenKind::LBrace, "{", token_line, token_column});
          break;
        case '}':
          tokens.push_back(Token{TokenKind::RBrace, "}", token_line, token_column});
          break;
        case ',':
          tokens.push_back(Token{TokenKind::Comma, ",", token_line, token_column});
          break;
        case ':':
          tokens.push_back(Token{TokenKind::Colon, ":", token_line, token_column});
          break;
        case ';':
          tokens.push_back(Token{TokenKind::Semicolon, ";", token_line, token_column});
          break;
        case '=':
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::EqualEqual, "==", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Equal, "=", token_line, token_column});
          }
          break;
        case '!':
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::BangEqual, "!=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Bang, "!", token_line, token_column});
          }
          break;
        case '<':
          if (MatchChar('<')) {
            if (MatchChar('=')) {
              tokens.push_back(Token{TokenKind::LessLessEqual, "<<=", token_line, token_column});
            } else {
              tokens.push_back(Token{TokenKind::LessLess, "<<", token_line, token_column});
            }
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::LessEqual, "<=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Less, "<", token_line, token_column});
          }
          break;
        case '>':
          if (MatchChar('>')) {
            if (MatchChar('=')) {
              tokens.push_back(Token{TokenKind::GreaterGreaterEqual, ">>=", token_line, token_column});
            } else {
              tokens.push_back(Token{TokenKind::GreaterGreater, ">>", token_line, token_column});
            }
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::GreaterEqual, ">=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Greater, ">", token_line, token_column});
          }
          break;
        case '&':
          if (MatchChar('&')) {
            tokens.push_back(Token{TokenKind::AndAnd, "&&", token_line, token_column});
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::AmpersandEqual, "&=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Ampersand, "&", token_line, token_column});
          }
          break;
        case '|':
          if (MatchChar('|')) {
            tokens.push_back(Token{TokenKind::OrOr, "||", token_line, token_column});
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::PipeEqual, "|=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Pipe, "|", token_line, token_column});
          }
          break;
        case '^':
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::CaretEqual, "^=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Caret, "^", token_line, token_column});
          }
          break;
        case '?':
          tokens.push_back(Token{TokenKind::Question, "?", token_line, token_column});
          break;
        case '~':
          tokens.push_back(Token{TokenKind::Tilde, "~", token_line, token_column});
          break;
        case '+':
          if (MatchChar('+')) {
            tokens.push_back(Token{TokenKind::PlusPlus, "++", token_line, token_column});
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::PlusEqual, "+=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Plus, "+", token_line, token_column});
          }
          break;
        case '-':
          if (MatchChar('-')) {
            tokens.push_back(Token{TokenKind::MinusMinus, "--", token_line, token_column});
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::MinusEqual, "-=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Minus, "-", token_line, token_column});
          }
          break;
        case '*':
          if (MatchChar('/')) {
            diagnostics.push_back(
                MakeDiag(token_line, token_column, "O3L004", "stray block comment terminator"));
          } else if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::StarEqual, "*=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Star, "*", token_line, token_column});
          }
          break;
        case '/':
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::SlashEqual, "/=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Slash, "/", token_line, token_column});
          }
          break;
        case '%':
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::PercentEqual, "%=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::Percent, "%", token_line, token_column});
          }
          break;
        default:
          diagnostics.push_back(MakeDiag(token_line, token_column, "O3L001",
                                         std::string("unexpected character '") + c + "'"));
          break;
      }
    }
    return tokens;
  }

 private:
  void SkipTrivia(std::vector<std::string> &diagnostics) {
    while (index_ < source_.size()) {
      const char c = source_[index_];
      if (std::isspace(static_cast<unsigned char>(c)) != 0) {
        Advance();
        continue;
      }
      if (c == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '/') {
        while (index_ < source_.size() && source_[index_] != '\n') {
          Advance();
        }
        continue;
      }
      if (c == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '*') {
        const unsigned comment_line = line_;
        const unsigned comment_column = column_;
        Advance();
        Advance();
        bool terminated = false;
        while (index_ < source_.size()) {
          if (source_[index_] == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '*') {
            diagnostics.push_back(
                MakeDiag(line_, column_, "O3L003", "nested block comments are unsupported"));
            index_ = source_.size();
            return;
          }
          if (source_[index_] == '*' && index_ + 1 < source_.size() && source_[index_ + 1] == '/') {
            Advance();
            Advance();
            terminated = true;
            break;
          }
          Advance();
        }
        if (!terminated) {
          diagnostics.push_back(
              MakeDiag(comment_line, comment_column, "O3L002", "unterminated block comment"));
          index_ = source_.size();
          return;
        }
        continue;
      }
      break;
    }
  }

  std::string ConsumeIdentifier() {
    const std::size_t begin = index_;
    Advance();
    while (index_ < source_.size() && IsIdentBody(source_[index_])) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }

  std::string ConsumeNumber() {
    const std::size_t begin = index_;
    if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
        (source_[index_ + 1] == 'b' || source_[index_ + 1] == 'B')) {
      Advance();
      Advance();
      while (index_ < source_.size() &&
             (IsBinaryDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
        Advance();
      }
      return source_.substr(begin, index_ - begin);
    }
    if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
        (source_[index_ + 1] == 'o' || source_[index_ + 1] == 'O')) {
      Advance();
      Advance();
      while (index_ < source_.size() &&
             (IsOctalDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
        Advance();
      }
      return source_.substr(begin, index_ - begin);
    }
    if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
        (source_[index_ + 1] == 'x' || source_[index_ + 1] == 'X')) {
      Advance();
      Advance();
      while (index_ < source_.size() &&
             (IsHexDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
        Advance();
      }
      return source_.substr(begin, index_ - begin);
    }
    while (index_ < source_.size() &&
           (std::isdigit(static_cast<unsigned char>(source_[index_])) != 0 ||
            IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }

  void Advance() {
    if (index_ >= source_.size()) {
      return;
    }
    if (source_[index_] == '\n') {
      ++line_;
      column_ = 1;
    } else {
      ++column_;
    }
    ++index_;
  }

  bool MatchChar(char expected) {
    if (index_ >= source_.size() || source_[index_] != expected) {
      return false;
    }
    Advance();
    return true;
  }

  const std::string &source_;
  std::size_t index_ = 0;
  unsigned line_ = 1;
  unsigned column_ = 1;
};

class Objc3Parser {
 public:
  explicit Objc3Parser(const std::vector<Token> &tokens) : tokens_(tokens) {}

  Objc3Program Parse() {
    Objc3Program program;
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::KwModule)) {
        ParseModule(program);
      } else if (Match(TokenKind::KwLet)) {
        auto decl = ParseGlobalLet();
        if (decl != nullptr) {
          program.globals.push_back(std::move(*decl));
        }
      } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern) || At(TokenKind::KwFn)) {
        ParseTopLevelFunctionDecl(program);
      } else {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100",
                                        "unsupported Objective-C 3 statement"));
        SynchronizeTopLevel();
      }
    }
    return program;
  }

  std::vector<std::string> TakeDiagnostics() { return diagnostics_; }

 private:
  bool At(TokenKind kind) const { return tokens_[index_].kind == kind; }

  const Token &Peek() const { return tokens_[index_]; }

  const Token &Previous() const { return tokens_[index_ - 1]; }

  const Token &Advance() {
    if (!At(TokenKind::Eof)) {
      ++index_;
    }
    return Previous();
  }

  bool Match(TokenKind kind) {
    if (At(kind)) {
      Advance();
      return true;
    }
    return false;
  }

  void ParseTopLevelFunctionDecl(Objc3Program &program) {
    bool is_pure = false;
    bool is_extern = false;
    std::optional<TokenKind> trailing_qualifier = std::nullopt;

    while (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      if (Match(TokenKind::KwPure)) {
        if (is_pure) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'pure' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_pure = true;
        trailing_qualifier = TokenKind::KwPure;
        continue;
      }

      if (Match(TokenKind::KwExtern)) {
        if (is_extern) {
          const Token &token = Previous();
          diagnostics_.push_back(
              MakeDiag(token.line, token.column, "O3P100", "duplicate 'extern' qualifier in function declaration"));
          SynchronizeTopLevel();
          return;
        }
        is_extern = true;
        trailing_qualifier = TokenKind::KwExtern;
      }
    }

    if (!Match(TokenKind::KwFn)) {
      const Token &token = Peek();
      const std::string message = (trailing_qualifier.has_value() && trailing_qualifier.value() == TokenKind::KwExtern)
                                      ? "expected 'fn' after 'extern'"
                                      : "expected 'fn' after 'pure'";
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", message));
      SynchronizeTopLevel();
      return;
    }

    auto fn = ParseFunction();
    if (fn == nullptr) {
      return;
    }

    fn->is_pure = is_pure;
    if (is_extern && !fn->is_prototype) {
      diagnostics_.push_back(MakeDiag(fn->line, fn->column, "O3P104", "missing ';' after extern function declaration"));
      return;
    }

    program.functions.push_back(std::move(*fn));
  }

  bool AtIdentifierColon() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Colon;
  }

  static bool IsAssignmentOperatorToken(TokenKind kind) {
    return kind == TokenKind::Equal || kind == TokenKind::PlusEqual || kind == TokenKind::MinusEqual ||
           kind == TokenKind::StarEqual || kind == TokenKind::SlashEqual || kind == TokenKind::PercentEqual ||
           kind == TokenKind::AmpersandEqual || kind == TokenKind::PipeEqual || kind == TokenKind::CaretEqual ||
           kind == TokenKind::LessLessEqual || kind == TokenKind::GreaterGreaterEqual;
  }

  static bool IsUpdateOperatorToken(TokenKind kind) {
    return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
  }

  bool AtIdentifierAssignment() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           IsAssignmentOperatorToken(tokens_[index_ + 1].kind);
  }

  bool AtIdentifierUpdate() const {
    return At(TokenKind::Identifier) && (index_ + 1 < tokens_.size()) &&
           IsUpdateOperatorToken(tokens_[index_ + 1].kind);
  }

  bool AtPrefixUpdate() const {
    return IsUpdateOperatorToken(Peek().kind) && (index_ + 1 < tokens_.size()) &&
           tokens_[index_ + 1].kind == TokenKind::Identifier;
  }

  bool MatchAssignmentOperator(std::string &op) {
    if (Match(TokenKind::Equal)) {
      op = "=";
      return true;
    }
    if (Match(TokenKind::PlusEqual)) {
      op = "+=";
      return true;
    }
    if (Match(TokenKind::MinusEqual)) {
      op = "-=";
      return true;
    }
    if (Match(TokenKind::StarEqual)) {
      op = "*=";
      return true;
    }
    if (Match(TokenKind::SlashEqual)) {
      op = "/=";
      return true;
    }
    if (Match(TokenKind::PercentEqual)) {
      op = "%=";
      return true;
    }
    if (Match(TokenKind::AmpersandEqual)) {
      op = "&=";
      return true;
    }
    if (Match(TokenKind::PipeEqual)) {
      op = "|=";
      return true;
    }
    if (Match(TokenKind::CaretEqual)) {
      op = "^=";
      return true;
    }
    if (Match(TokenKind::LessLessEqual)) {
      op = "<<=";
      return true;
    }
    if (Match(TokenKind::GreaterGreaterEqual)) {
      op = ">>=";
      return true;
    }
    return false;
  }

  bool MatchUpdateOperator(std::string &op) {
    if (Match(TokenKind::PlusPlus)) {
      op = "++";
      return true;
    }
    if (Match(TokenKind::MinusMinus)) {
      op = "--";
      return true;
    }
    return false;
  }

  void ParseModule(Objc3Program &program) {
    const Token &name_token = Peek();
    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P101", "invalid module identifier"));
      SynchronizeTopLevel();
      return;
    }
    const std::string module_name = Advance().text;
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P104", "missing ';' after module declaration"));
      SynchronizeTopLevel();
      return;
    }
    if (saw_module_declaration_) {
      diagnostics_.push_back(
          MakeDiag(name_token.line, name_token.column, "O3S200", "duplicate module '" + module_name + "'"));
      return;
    }
    saw_module_declaration_ = true;
    program.module_name = module_name;
  }

  std::unique_ptr<GlobalDecl> ParseGlobalLet() {
    auto decl = std::make_unique<GlobalDecl>();
    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid declaration identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    decl->name = Previous().text;
    decl->line = Previous().line;
    decl->column = Previous().column;

    if (!Match(TokenKind::Equal)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
      SynchronizeTopLevel();
      return nullptr;
    }

    decl->value = ParseExpression();
    if (decl->value == nullptr) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                      "missing ';' after declaration"));
      SynchronizeTopLevel();
      return nullptr;
    }
    return decl;
  }

  std::unique_ptr<FunctionDecl> ParseFunction() {
    auto fn = std::make_unique<FunctionDecl>();
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after 'fn'"
                                      : "unexpected qualifier 'extern' after 'fn'";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    const Token &name_token = Peek();
    if (!Match(TokenKind::Identifier)) {
      diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                      "invalid function identifier"));
      SynchronizeTopLevel();
      return nullptr;
    }
    fn->name = Previous().text;
    fn->line = Previous().line;
    fn->column = Previous().column;

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after function name"
                                      : "unexpected qualifier 'extern' after function name";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::LParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after function name"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!ParseFunctionParameters(*fn)) {
      SynchronizeTopLevel();
      return nullptr;
    }

    if (!Match(TokenKind::RParen)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after parameters"));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after parameter list"
                                      : "unexpected qualifier 'extern' after parameter list";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Minus)) {
      const Token arrow_start = Previous();
      if (!Match(TokenKind::Greater)) {
        diagnostics_.push_back(
            MakeDiag(arrow_start.line, arrow_start.column, "O3P114", "missing '>' in function return annotation"));
        SynchronizeFunctionTail();
        return nullptr;
      }
      if (!ParseFunctionReturnType(*fn)) {
        SynchronizeFunctionTail();
        return nullptr;
      }
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' after function return annotation"
                                      : "unexpected qualifier 'extern' after function return annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      SynchronizeTopLevel();
      return nullptr;
    }

    if (Match(TokenKind::Semicolon)) {
      fn->is_prototype = true;
      return fn;
    }

    if (!At(TokenKind::LBrace)) {
      const Token &token = Peek();
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern) || At(TokenKind::Eof)) {
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P104", "missing ';' after function prototype declaration"));
      } else {
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      }
      SynchronizeTopLevel();
      return nullptr;
    }

    fn->body = ParseBlock();
    if (block_failed_) {
      block_failed_ = false;
      SynchronizeTopLevel();
      return nullptr;
    }
    return fn;
  }

  bool ParseFunctionParameters(FunctionDecl &fn) {
    if (At(TokenKind::RParen)) {
      return true;
    }

    while (true) {
      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' in parameter identifier position"
                                        : "unexpected qualifier 'extern' in parameter identifier position";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!At(TokenKind::Identifier)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P101", "invalid parameter identifier"));
        return false;
      }

      FuncParam param;
      param.name = Advance().text;
      param.line = Previous().line;
      param.column = Previous().column;

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' after parameter name"
                                        : "unexpected qualifier 'extern' after parameter name";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      if (!Match(TokenKind::Colon)) {
        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P107", "missing ':' after parameter name"));
        return false;
      }
      if (!ParseParameterType(param)) {
        return false;
      }

      if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
        const Token qualifier = Advance();
        const std::string message = qualifier.kind == TokenKind::KwPure
                                        ? "unexpected qualifier 'pure' after parameter type annotation"
                                        : "unexpected qualifier 'extern' after parameter type annotation";
        diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
        return false;
      }

      fn.params.push_back(std::move(param));
      if (!Match(TokenKind::Comma)) {
        return true;
      }
    }
  }

  bool ParseFunctionReturnType(FunctionDecl &fn) {
    fn.return_id_spelling = false;
    fn.return_class_spelling = false;
    fn.return_instancetype_spelling = false;
    fn.has_return_generic_suffix = false;
    fn.return_generic_suffix_terminated = true;
    fn.return_generic_suffix_text.clear();
    fn.return_generic_line = 1;
    fn.return_generic_column = 1;
    fn.return_nullability_suffix_tokens.clear();

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in function return type annotation"
                                      : "unexpected qualifier 'extern' in function return type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    if (Match(TokenKind::KwI32)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      fn.return_type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwVoid)) {
      fn.return_type = ValueType::Void;
    } else if (Match(TokenKind::KwId)) {
      fn.return_type = ValueType::I32;
      fn.return_id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      fn.return_type = ValueType::I32;
      fn.return_class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwProtocol)) {
      fn.return_type = ValueType::I32;
    } else if (Match(TokenKind::KwInstancetype)) {
      fn.return_type = ValueType::I32;
      fn.return_instancetype_spelling = true;
    } else {
      if (At(TokenKind::Identifier)) {
        const Token type_token = Advance();
        diagnostics_.push_back(MakeDiag(type_token.line, type_token.column, "O3P114",
                                        "unsupported function return type '" + type_token.text +
                                            "' (expected 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', "
                                            "'id', 'Class', 'SEL', 'Protocol', or 'instancetype')"));
        return false;
      }
      const Token &token = Peek();
      diagnostics_.push_back(
          MakeDiag(token.line, token.column, "O3P114",
                   "expected function return type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'void', 'id', or "
                   "'Class', 'SEL', 'Protocol', or 'instancetype'"));
      return false;
    }

    if (Match(TokenKind::Less)) {
      const Token &open = Previous();
      fn.has_return_generic_suffix = true;
      fn.return_generic_suffix_terminated = false;
      fn.return_generic_line = open.line;
      fn.return_generic_column = open.column;
      fn.return_generic_suffix_text = "<";
      int depth = 1;
      while (depth > 0 && !At(TokenKind::Eof)) {
        if (Match(TokenKind::Less)) {
          fn.return_generic_suffix_text += "<";
          ++depth;
          continue;
        }
        if (Match(TokenKind::Greater)) {
          fn.return_generic_suffix_text += ">";
          --depth;
          if (depth == 0) {
            fn.return_generic_suffix_terminated = true;
          }
          continue;
        }
        fn.return_generic_suffix_text += Advance().text;
      }
      if (!fn.return_generic_suffix_terminated) {
        diagnostics_.push_back(
            MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3P114",
                     "unterminated generic function return type suffix"));
        return false;
      }
    }

    while (At(TokenKind::Question) || At(TokenKind::Bang)) {
      fn.return_nullability_suffix_tokens.push_back(Advance());
    }

    return true;
  }

  bool ParseParameterType(FuncParam &param) {
    param.id_spelling = false;
    param.class_spelling = false;
    param.instancetype_spelling = false;
    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in parameter type annotation"
                                      : "unexpected qualifier 'extern' in parameter type annotation";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return false;
    }

    if (Match(TokenKind::KwI32)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwBool)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwBOOL)) {
      param.type = ValueType::Bool;
    } else if (Match(TokenKind::KwNSInteger) || Match(TokenKind::KwNSUInteger)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwId)) {
      param.type = ValueType::I32;
      param.id_spelling = true;
    } else if (Match(TokenKind::KwClass)) {
      param.type = ValueType::I32;
      param.class_spelling = true;
    } else if (Match(TokenKind::KwSEL)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwProtocol)) {
      param.type = ValueType::I32;
    } else if (Match(TokenKind::KwInstancetype)) {
      param.type = ValueType::I32;
      param.instancetype_spelling = true;
    } else if (At(TokenKind::Identifier)) {
      const Token type_token = Advance();
      FuncParam ignored_suffix;
      ParseParameterTypeSuffix(ignored_suffix);
      if (!ignored_suffix.generic_suffix_terminated) {
        return false;
      }
      diagnostics_.push_back(MakeDiag(type_token.line, type_token.column, "O3P108",
                                      "unsupported parameter type '" + type_token.text +
                                          "' (expected 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', 'id', "
                                          "'Class', 'SEL', 'Protocol', or 'instancetype')"));
      return false;
    } else {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P108",
                                      "expected parameter type 'i32', 'bool', 'BOOL', 'NSInteger', 'NSUInteger', or "
                                      "'id', 'Class', 'SEL', 'Protocol', or 'instancetype'"));
      return false;
    }

    ParseParameterTypeSuffix(param);
    if (!param.generic_suffix_terminated) {
      return false;
    }

    return true;
  }

  void ParseParameterTypeSuffix(FuncParam &param) {
    if (Match(TokenKind::Less)) {
      const Token &open = Previous();
      param.has_generic_suffix = true;
      param.generic_suffix_terminated = false;
      param.generic_line = open.line;
      param.generic_column = open.column;
      param.generic_suffix_text = "<";
      int depth = 1;
      while (depth > 0 && !At(TokenKind::Eof)) {
        if (Match(TokenKind::Less)) {
          param.generic_suffix_text += "<";
          ++depth;
          continue;
        }
        if (Match(TokenKind::Greater)) {
          param.generic_suffix_text += ">";
          --depth;
          if (depth == 0) {
            param.generic_suffix_terminated = true;
          }
          continue;
        }
        param.generic_suffix_text += Advance().text;
      }
      if (!param.generic_suffix_terminated) {
        diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P108",
                                        "unterminated generic parameter type suffix"));
      }
    }
    while (At(TokenKind::Question) || At(TokenKind::Bang)) {
      param.nullability_suffix_tokens.push_back(Advance());
    }
  }

  std::vector<std::unique_ptr<Stmt>> ParseBlock() {
    std::vector<std::unique_ptr<Stmt>> body;
    if (!Match(TokenKind::LBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' to start block"));
      block_failed_ = true;
      return {};
    }

    while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
      auto stmt = ParseStatement();
      if (stmt != nullptr) {
        body.push_back(std::move(stmt));
      } else {
        SynchronizeStatement();
      }
    }

    if (!Match(TokenKind::RBrace)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' to end block"));
      block_failed_ = true;
      return {};
    }

    return body;
  }

  std::vector<std::unique_ptr<Stmt>> ParseControlBody() {
    if (At(TokenKind::LBrace)) {
      return ParseBlock();
    }
    std::vector<std::unique_ptr<Stmt>> body;
    auto stmt = ParseStatement();
    if (stmt == nullptr) {
      block_failed_ = true;
      return {};
    }
    body.push_back(std::move(stmt));
    return body;
  }

  void SynchronizeTopLevel() {
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (At(TokenKind::KwModule) || At(TokenKind::KwLet) || At(TokenKind::KwFn) || At(TokenKind::KwPure) ||
          At(TokenKind::KwExtern)) {
        return;
      }
      Advance();
    }
  }

  void SynchronizeFunctionTail() {
    if (At(TokenKind::LBrace)) {
      int depth = 0;
      while (!At(TokenKind::Eof)) {
        if (Match(TokenKind::LBrace)) {
          ++depth;
          continue;
        }
        if (Match(TokenKind::RBrace)) {
          --depth;
          if (depth <= 0) {
            return;
          }
          continue;
        }
        Advance();
      }
      return;
    }
    SynchronizeTopLevel();
  }

  void SynchronizeStatement() {
    while (!At(TokenKind::Eof)) {
      if (Match(TokenKind::Semicolon)) {
        return;
      }
      if (At(TokenKind::KwLet) || At(TokenKind::KwReturn) || At(TokenKind::KwIf) || At(TokenKind::KwDo) ||
          At(TokenKind::KwFor) || At(TokenKind::KwSwitch) || At(TokenKind::KwWhile) || At(TokenKind::KwBreak) ||
          At(TokenKind::KwContinue) || AtIdentifierAssignment() || AtIdentifierUpdate() || AtPrefixUpdate() ||
          At(TokenKind::RBrace)) {
        return;
      }
      Advance();
    }
  }

  std::unique_ptr<Stmt> ParseStatement() {
    if (At(TokenKind::LBrace)) {
      const Token open = Peek();
      auto body = ParseBlock();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Block;
      stmt->line = open.line;
      stmt->column = open.column;
      stmt->block_stmt = std::make_unique<BlockStmt>();
      stmt->block_stmt->line = open.line;
      stmt->block_stmt->column = open.column;
      stmt->block_stmt->body = std::move(body);
      return stmt;
    }

    if (Match(TokenKind::Semicolon)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Empty;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      return stmt;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in statement position"
                                      : "unexpected qualifier 'extern' in statement position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    if (Match(TokenKind::KwLet)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Let;
      stmt->let_stmt = std::make_unique<LetStmt>();
      const Token &name_token = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                        "invalid declaration identifier"));
        return nullptr;
      }
      stmt->let_stmt->name = Previous().text;
      stmt->let_stmt->line = Previous().line;
      stmt->let_stmt->column = Previous().column;
      stmt->line = Previous().line;
      stmt->column = Previous().column;

      if (!Match(TokenKind::Equal)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
        return nullptr;
      }

      stmt->let_stmt->value = ParseExpression();
      if (stmt->let_stmt->value == nullptr) {
        return nullptr;
      }

      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104",
                                        "missing ';' after declaration"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwReturn)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Return;
      stmt->return_stmt = std::make_unique<ReturnStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->return_stmt->line = Previous().line;
      stmt->return_stmt->column = Previous().column;
      if (Match(TokenKind::Semicolon)) {
        return stmt;
      }
      stmt->return_stmt->value = ParseExpression();
      if (stmt->return_stmt->value == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after return"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwIf)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::If;
      stmt->if_stmt = std::make_unique<IfStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->if_stmt->line = Previous().line;
      stmt->if_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after if"));
        return nullptr;
      }
      stmt->if_stmt->condition = ParseExpression();
      if (stmt->if_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after if condition"));
        return nullptr;
      }

      stmt->if_stmt->then_body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      if (Match(TokenKind::KwElse)) {
        stmt->if_stmt->else_body = ParseControlBody();
        if (block_failed_) {
          block_failed_ = false;
          return nullptr;
        }
      }
      return stmt;
    }

    if (Match(TokenKind::KwDo)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::DoWhile;
      stmt->do_while_stmt = std::make_unique<DoWhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->do_while_stmt->line = Previous().line;
      stmt->do_while_stmt->column = Previous().column;

      stmt->do_while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }

      if (!Match(TokenKind::KwWhile)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P100", "missing 'while' after do block"));
        return nullptr;
      }
      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->do_while_stmt->condition = ParseExpression();
      if (stmt->do_while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after do-while condition"));
        return nullptr;
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after do-while"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwFor)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::For;
      stmt->for_stmt = std::make_unique<ForStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->for_stmt->line = Previous().line;
      stmt->for_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after for"));
        return nullptr;
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->init.kind = ForClause::Kind::None;
      } else {
        if (Match(TokenKind::KwLet)) {
          stmt->for_stmt->init.kind = ForClause::Kind::Let;
          const Token &name_token = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name_token.line, name_token.column, "O3P101",
                                            "invalid declaration identifier"));
            return nullptr;
          }
          stmt->for_stmt->init.name = Previous().text;
          stmt->for_stmt->init.line = Previous().line;
          stmt->for_stmt->init.column = Previous().column;

          if (!Match(TokenKind::Equal)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P102", "missing '='"));
            return nullptr;
          }

          stmt->for_stmt->init.value = ParseExpression();
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        } else if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->init.value = nullptr;
          } else {
            stmt->for_stmt->init.value = ParseExpression();
            if (stmt->for_stmt->init.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->init.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->init.name = name.text;
          stmt->for_stmt->init.op = op;
          stmt->for_stmt->init.line = name.line;
          stmt->for_stmt->init.column = name.column;
          stmt->for_stmt->init.value = nullptr;
        } else {
          stmt->for_stmt->init.kind = ForClause::Kind::Expr;
          stmt->for_stmt->init.line = Peek().line;
          stmt->for_stmt->init.column = Peek().column;
          stmt->for_stmt->init.value = ParseExpression();
          if (stmt->for_stmt->init.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for init"));
          return nullptr;
        }
      }

      if (Match(TokenKind::Semicolon)) {
        stmt->for_stmt->condition = nullptr;
      } else {
        stmt->for_stmt->condition = ParseExpression();
        if (stmt->for_stmt->condition == nullptr) {
          return nullptr;
        }
        if (!Match(TokenKind::Semicolon)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after for condition"));
          return nullptr;
        }
      }

      if (Match(TokenKind::RParen)) {
        stmt->for_stmt->step.kind = ForClause::Kind::None;
      } else {
        if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          const Token name = Advance();
          std::string op = "=";
          if (!MatchAssignmentOperator(op)) {
            (void)MatchUpdateOperator(op);
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          if (op == "++" || op == "--") {
            stmt->for_stmt->step.value = nullptr;
          } else {
            stmt->for_stmt->step.value = ParseExpression();
            if (stmt->for_stmt->step.value == nullptr) {
              return nullptr;
            }
          }
        } else if (AtPrefixUpdate()) {
          stmt->for_stmt->step.kind = ForClause::Kind::Assign;
          std::string op = "++";
          (void)MatchUpdateOperator(op);
          const Token name = Peek();
          if (!Match(TokenKind::Identifier)) {
            diagnostics_.push_back(MakeDiag(name.line, name.column, "O3P101",
                                            "invalid assignment target"));
            return nullptr;
          }
          stmt->for_stmt->step.name = name.text;
          stmt->for_stmt->step.op = op;
          stmt->for_stmt->step.line = name.line;
          stmt->for_stmt->step.column = name.column;
          stmt->for_stmt->step.value = nullptr;
        } else {
          stmt->for_stmt->step.kind = ForClause::Kind::Expr;
          stmt->for_stmt->step.line = Peek().line;
          stmt->for_stmt->step.column = Peek().column;
          stmt->for_stmt->step.value = ParseExpression();
          if (stmt->for_stmt->step.value == nullptr) {
            return nullptr;
          }
        }
        if (!Match(TokenKind::RParen)) {
          const Token &token = Peek();
          diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after for clauses"));
          return nullptr;
        }
      }

      stmt->for_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwSwitch)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Switch;
      stmt->switch_stmt = std::make_unique<SwitchStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->switch_stmt->line = Previous().line;
      stmt->switch_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after switch"));
        return nullptr;
      }
      stmt->switch_stmt->condition = ParseExpression();
      if (stmt->switch_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after switch condition"));
        return nullptr;
      }
      if (!Match(TokenKind::LBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P110", "missing '{' for switch body"));
        return nullptr;
      }

      while (!At(TokenKind::RBrace) && !At(TokenKind::Eof)) {
        if (Match(TokenKind::KwCase)) {
          SwitchCase case_stmt;
          case_stmt.line = Previous().line;
          case_stmt.column = Previous().column;
          case_stmt.is_default = false;

          if (Match(TokenKind::Number)) {
            case_stmt.value_line = Previous().line;
            case_stmt.value_column = Previous().column;
            case_stmt.value = std::atoi(Previous().text.c_str());
          } else if (Match(TokenKind::Minus) || Match(TokenKind::Plus)) {
            const Token sign = Previous();
            if (!Match(TokenKind::Number)) {
              diagnostics_.push_back(MakeDiag(sign.line, sign.column, "O3P103", "invalid case label expression"));
              return nullptr;
            }
            case_stmt.value_line = sign.line;
            case_stmt.value_column = sign.column;
            const int magnitude = std::atoi(Previous().text.c_str());
            case_stmt.value = sign.kind == TokenKind::Minus ? -magnitude : magnitude;
          } else if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
            case_stmt.value_line = Previous().line;
            case_stmt.value_column = Previous().column;
            case_stmt.value = Previous().kind == TokenKind::KwTrue ? 1 : 0;
          } else if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
            const Token qualifier = Advance();
            const std::string message = qualifier.kind == TokenKind::KwPure
                                            ? "unexpected qualifier 'pure' in case label expression"
                                            : "unexpected qualifier 'extern' in case label expression";
            diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
            return nullptr;
          } else {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid case label expression"));
            return nullptr;
          }

          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after case label"));
            return nullptr;
          }

          while (!At(TokenKind::KwCase) && !At(TokenKind::KwDefault) && !At(TokenKind::RBrace) &&
                 !At(TokenKind::Eof)) {
            std::unique_ptr<Stmt> body_stmt = ParseStatement();
            if (body_stmt != nullptr) {
              case_stmt.body.push_back(std::move(body_stmt));
              continue;
            }
            SynchronizeStatement();
            if (At(TokenKind::Eof)) {
              break;
            }
          }
          stmt->switch_stmt->cases.push_back(std::move(case_stmt));
          continue;
        }

        if (Match(TokenKind::KwDefault)) {
          SwitchCase default_stmt;
          default_stmt.line = Previous().line;
          default_stmt.column = Previous().column;
          default_stmt.is_default = true;
          default_stmt.value = 0;
          default_stmt.value_line = Previous().line;
          default_stmt.value_column = Previous().column;

          if (!Match(TokenKind::Colon)) {
            const Token &token = Peek();
            diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' after default"));
            return nullptr;
          }

          while (!At(TokenKind::KwCase) && !At(TokenKind::KwDefault) && !At(TokenKind::RBrace) &&
                 !At(TokenKind::Eof)) {
            std::unique_ptr<Stmt> body_stmt = ParseStatement();
            if (body_stmt != nullptr) {
              default_stmt.body.push_back(std::move(body_stmt));
              continue;
            }
            SynchronizeStatement();
            if (At(TokenKind::Eof)) {
              break;
            }
          }
          stmt->switch_stmt->cases.push_back(std::move(default_stmt));
          continue;
        }

        const Token &token = Peek();
        diagnostics_.push_back(
            MakeDiag(token.line, token.column, "O3P100", "expected 'case' or 'default' in switch body"));
        Advance();
      }

      if (!Match(TokenKind::RBrace)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P111", "missing '}' after switch body"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwWhile)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::While;
      stmt->while_stmt = std::make_unique<WhileStmt>();
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      stmt->while_stmt->line = Previous().line;
      stmt->while_stmt->column = Previous().column;

      if (!Match(TokenKind::LParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P106", "missing '(' after while"));
        return nullptr;
      }
      stmt->while_stmt->condition = ParseExpression();
      if (stmt->while_stmt->condition == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after while condition"));
        return nullptr;
      }

      stmt->while_stmt->body = ParseControlBody();
      if (block_failed_) {
        block_failed_ = false;
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwBreak)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Break;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after break"));
        return nullptr;
      }
      return stmt;
    }

    if (Match(TokenKind::KwContinue)) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Continue;
      stmt->line = Previous().line;
      stmt->column = Previous().column;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after continue"));
        return nullptr;
      }
      return stmt;
    }

    if (AtIdentifierAssignment() || AtIdentifierUpdate()) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Assign;
      stmt->assign_stmt = std::make_unique<AssignStmt>();
      const Token name = Advance();
      std::string op = "=";
      if (!MatchAssignmentOperator(op)) {
        (void)MatchUpdateOperator(op);
      }
      stmt->line = name.line;
      stmt->column = name.column;
      stmt->assign_stmt->line = name.line;
      stmt->assign_stmt->column = name.column;
      stmt->assign_stmt->name = name.text;
      stmt->assign_stmt->op = op;
      if (op == "++" || op == "--") {
        stmt->assign_stmt->value = nullptr;
      } else {
        stmt->assign_stmt->value = ParseExpression();
        if (stmt->assign_stmt->value == nullptr) {
          return nullptr;
        }
      }
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after assignment"));
        return nullptr;
      }
      return stmt;
    }

    if (AtPrefixUpdate()) {
      auto stmt = std::make_unique<Stmt>();
      stmt->kind = Stmt::Kind::Assign;
      stmt->assign_stmt = std::make_unique<AssignStmt>();
      std::string op = "++";
      const Token op_token = Peek();
      (void)MatchUpdateOperator(op);
      const Token name = Peek();
      if (!Match(TokenKind::Identifier)) {
        diagnostics_.push_back(MakeDiag(op_token.line, op_token.column, "O3P101", "invalid assignment target"));
        return nullptr;
      }
      stmt->line = name.line;
      stmt->column = name.column;
      stmt->assign_stmt->line = name.line;
      stmt->assign_stmt->column = name.column;
      stmt->assign_stmt->name = name.text;
      stmt->assign_stmt->op = op;
      stmt->assign_stmt->value = nullptr;
      if (!Match(TokenKind::Semicolon)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after assignment"));
        return nullptr;
      }
      return stmt;
    }

    auto stmt = std::make_unique<Stmt>();
    stmt->kind = Stmt::Kind::Expr;
    stmt->expr_stmt = std::make_unique<ExprStmt>();
    stmt->line = Peek().line;
    stmt->column = Peek().column;
    stmt->expr_stmt->line = Peek().line;
    stmt->expr_stmt->column = Peek().column;
    stmt->expr_stmt->value = ParseExpression();
    if (stmt->expr_stmt->value == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Semicolon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P104", "missing ';' after expression"));
      return nullptr;
    }
    return stmt;
  }

  std::unique_ptr<Expr> ParseExpression() { return ParseConditional(); }

  std::unique_ptr<Expr> ParseConditional() {
    auto expr = ParseLogicalOr();
    if (expr == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Question)) {
      return expr;
    }

    const Token question = Previous();
    auto when_true = ParseExpression();
    if (when_true == nullptr) {
      return nullptr;
    }
    if (!Match(TokenKind::Colon)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P107", "missing ':' in conditional expression"));
      return nullptr;
    }
    auto when_false = ParseConditional();
    if (when_false == nullptr) {
      return nullptr;
    }

    auto node = std::make_unique<Expr>();
    node->kind = Expr::Kind::Conditional;
    node->line = question.line;
    node->column = question.column;
    node->left = std::move(expr);
    node->right = std::move(when_true);
    node->third = std::move(when_false);
    return node;
  }

  std::unique_ptr<Expr> ParseLogicalOr() {
    auto expr = ParseLogicalAnd();
    while (expr != nullptr && Match(TokenKind::OrOr)) {
      const Token op = Previous();
      auto rhs = ParseLogicalAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseLogicalAnd() {
    auto expr = ParseBitwiseOr();
    while (expr != nullptr && Match(TokenKind::AndAnd)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseOr();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseOr() {
    auto expr = ParseBitwiseXor();
    while (expr != nullptr && Match(TokenKind::Pipe)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseXor();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseXor() {
    auto expr = ParseBitwiseAnd();
    while (expr != nullptr && Match(TokenKind::Caret)) {
      const Token op = Previous();
      auto rhs = ParseBitwiseAnd();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseBitwiseAnd() {
    auto expr = ParseEquality();
    while (expr != nullptr && Match(TokenKind::Ampersand)) {
      const Token op = Previous();
      auto rhs = ParseEquality();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseEquality() {
    auto expr = ParseRelational();
    while (expr != nullptr && (Match(TokenKind::EqualEqual) || Match(TokenKind::BangEqual))) {
      const Token op = Previous();
      auto rhs = ParseRelational();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseRelational() {
    auto expr = ParseShift();
    while (expr != nullptr &&
           (Match(TokenKind::Less) || Match(TokenKind::LessEqual) || Match(TokenKind::Greater) ||
            Match(TokenKind::GreaterEqual))) {
      const Token op = Previous();
      auto rhs = ParseShift();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseShift() {
    auto expr = ParseAdditive();
    while (expr != nullptr && (Match(TokenKind::LessLess) || Match(TokenKind::GreaterGreater))) {
      const Token op = Previous();
      auto rhs = ParseAdditive();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseAdditive() {
    auto expr = ParseMultiplicative();
    while (expr != nullptr && (At(TokenKind::Plus) || At(TokenKind::Minus))) {
      const Token op = Advance();
      auto rhs = ParseMultiplicative();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseMultiplicative() {
    auto expr = ParseUnary();
    while (expr != nullptr && (At(TokenKind::Star) || At(TokenKind::Slash) || At(TokenKind::Percent))) {
      const Token op = Advance();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = op.text;
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(expr);
      node->right = std::move(rhs);
      expr = std::move(node);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParseUnary() {
    if (Match(TokenKind::Bang)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "==";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(rhs);
      node->right = std::move(zero);
      return node;
    }
    if (Match(TokenKind::Plus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "+";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(zero);
      node->right = std::move(rhs);
      return node;
    }
    if (Match(TokenKind::Minus)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto zero = std::make_unique<Expr>();
      zero->kind = Expr::Kind::Number;
      zero->number = 0;
      zero->line = op.line;
      zero->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "-";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(zero);
      node->right = std::move(rhs);
      return node;
    }
    if (Match(TokenKind::Tilde)) {
      const Token op = Previous();
      auto rhs = ParseUnary();
      if (rhs == nullptr) {
        return nullptr;
      }
      auto minus_one = std::make_unique<Expr>();
      minus_one->kind = Expr::Kind::Number;
      minus_one->number = -1;
      minus_one->line = op.line;
      minus_one->column = op.column;

      auto node = std::make_unique<Expr>();
      node->kind = Expr::Kind::Binary;
      node->op = "^";
      node->line = op.line;
      node->column = op.column;
      node->left = std::move(rhs);
      node->right = std::move(minus_one);
      return node;
    }
    return ParsePostfix();
  }

  std::unique_ptr<Expr> ParsePostfix() {
    auto expr = ParsePrimary();
    while (expr != nullptr && Match(TokenKind::LParen)) {
      const unsigned callee_line = expr->line;
      const unsigned callee_column = expr->column;
      auto call = std::make_unique<Expr>();
      call->kind = Expr::Kind::Call;
      call->line = callee_line;
      call->column = callee_column;
      if (expr->kind != Expr::Kind::Identifier) {
        diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P112", "call target must be identifier"));
        return nullptr;
      }
      call->ident = expr->ident;
      if (!At(TokenKind::RParen)) {
        while (true) {
          auto arg = ParseExpression();
          if (arg == nullptr) {
            return nullptr;
          }
          call->args.push_back(std::move(arg));
          if (!Match(TokenKind::Comma)) {
            break;
          }
        }
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after call"));
        return nullptr;
      }
      expr = std::move(call);
    }
    return expr;
  }

  std::unique_ptr<Expr> ParsePrimary() {
    if (Match(TokenKind::Number)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::Number;
      expr->line = Previous().line;
      expr->column = Previous().column;
      if (!ParseIntegerLiteralValue(Previous().text, expr->number)) {
        diagnostics_.push_back(MakeDiag(expr->line, expr->column, "O3P103",
                                        "invalid numeric literal '" + Previous().text + "'"));
        return nullptr;
      }
      return expr;
    }
    if (Match(TokenKind::KwTrue) || Match(TokenKind::KwFalse)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::BoolLiteral;
      expr->line = Previous().line;
      expr->column = Previous().column;
      expr->bool_value = Previous().kind == TokenKind::KwTrue;
      return expr;
    }
    if (Match(TokenKind::KwNil)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::NilLiteral;
      expr->line = Previous().line;
      expr->column = Previous().column;
      return expr;
    }
    if (Match(TokenKind::Identifier)) {
      auto expr = std::make_unique<Expr>();
      expr->kind = Expr::Kind::Identifier;
      expr->line = Previous().line;
      expr->column = Previous().column;
      expr->ident = Previous().text;
      return expr;
    }
    if (Match(TokenKind::LParen)) {
      auto expr = ParseExpression();
      if (expr == nullptr) {
        return nullptr;
      }
      if (!Match(TokenKind::RParen)) {
        const Token &token = Peek();
        diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P109", "missing ')' after expression"));
        return nullptr;
      }
      return expr;
    }

    if (Match(TokenKind::LBracket)) {
      return ParseMessageSendExpression();
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message = qualifier.kind == TokenKind::KwPure
                                      ? "unexpected qualifier 'pure' in expression position"
                                      : "unexpected qualifier 'extern' in expression position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message));
      return nullptr;
    }

    const Token &token = Peek();
    diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P103", "invalid expression"));
    return nullptr;
  }

  std::unique_ptr<Expr> ParseMessageSendExpression() {
    const Token open = Previous();
    auto message = std::make_unique<Expr>();
    message->kind = Expr::Kind::MessageSend;
    message->line = open.line;
    message->column = open.column;

    const std::size_t receiver_diag_count = diagnostics_.size();
    message->receiver = ParsePostfix();
    if (message->receiver == nullptr) {
      if (diagnostics_.size() == receiver_diag_count) {
        diagnostics_.push_back(MakeDiag(open.line, open.column, "O3P113",
                                        "invalid receiver expression in message send"));
      }
      return nullptr;
    }

    if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
      const Token qualifier = Advance();
      const std::string message_text =
          qualifier.kind == TokenKind::KwPure
              ? "unexpected qualifier 'pure' in message selector position"
              : "unexpected qualifier 'extern' in message selector position";
      diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
      return nullptr;
    }

    if (!At(TokenKind::Identifier)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "expected selector identifier in message send"));
      return nullptr;
    }

    const Token selector_head = Advance();
    message->selector = selector_head.text;
    if (Match(TokenKind::Colon)) {
      message->selector += ":";
      auto first_arg = ParseExpression();
      if (first_arg == nullptr) {
        return nullptr;
      }
      message->args.push_back(std::move(first_arg));

      while (true) {
        if (At(TokenKind::KwPure) || At(TokenKind::KwExtern)) {
          const Token qualifier = Advance();
          const std::string message_text =
              qualifier.kind == TokenKind::KwPure
                  ? "unexpected qualifier 'pure' in keyword selector segment position"
                  : "unexpected qualifier 'extern' in keyword selector segment position";
          diagnostics_.push_back(MakeDiag(qualifier.line, qualifier.column, "O3P100", message_text));
          return nullptr;
        }
        if (!At(TokenKind::Identifier)) {
          break;
        }
        const Token keyword = Advance();
        if (!Match(TokenKind::Colon)) {
          diagnostics_.push_back(MakeDiag(keyword.line, keyword.column, "O3P113",
                                          "missing ':' in keyword selector segment"));
          return nullptr;
        }
        message->selector += keyword.text;
        message->selector += ":";
        auto arg = ParseExpression();
        if (arg == nullptr) {
          return nullptr;
        }
        message->args.push_back(std::move(arg));
      }
    }

    if (!Match(TokenKind::RBracket)) {
      const Token &token = Peek();
      diagnostics_.push_back(MakeDiag(token.line, token.column, "O3P113",
                                      "missing ']' after message send expression"));
      return nullptr;
    }
    return message;
  }

  const std::vector<Token> &tokens_;
  std::size_t index_ = 0;
  std::vector<std::string> diagnostics_;
  bool saw_module_declaration_ = false;
  bool block_failed_ = false;
};

static bool EvalConstExpr(const Expr *expr, int &value,
                          const std::unordered_map<std::string, int> *resolved_globals = nullptr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::Number) {
    value = expr->number;
    return true;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    value = 0;
    return true;
  }
  if (expr->kind == Expr::Kind::BoolLiteral) {
    value = expr->bool_value ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Identifier) {
    if (resolved_globals == nullptr) {
      return false;
    }
    auto it = resolved_globals->find(expr->ident);
    if (it == resolved_globals->end()) {
      return false;
    }
    value = it->second;
    return true;
  }
  if (expr->kind == Expr::Kind::Conditional) {
    if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
      return false;
    }
    int cond_value = 0;
    if (!EvalConstExpr(expr->left.get(), cond_value, resolved_globals)) {
      return false;
    }
    if (cond_value != 0) {
      return EvalConstExpr(expr->right.get(), value, resolved_globals);
    }
    return EvalConstExpr(expr->third.get(), value, resolved_globals);
  }
  if (expr->kind != Expr::Kind::Binary || expr->left == nullptr || expr->right == nullptr) {
    return false;
  }
  int lhs = 0;
  int rhs = 0;
  if (!EvalConstExpr(expr->left.get(), lhs, resolved_globals) ||
      !EvalConstExpr(expr->right.get(), rhs, resolved_globals)) {
    return false;
  }
  if (expr->op == "+") {
    value = lhs + rhs;
    return true;
  }
  if (expr->op == "-") {
    value = lhs - rhs;
    return true;
  }
  if (expr->op == "*") {
    value = lhs * rhs;
    return true;
  }
  if (expr->op == "/") {
    if (rhs == 0) {
      return false;
    }
    value = lhs / rhs;
    return true;
  }
  if (expr->op == "%") {
    if (rhs == 0) {
      return false;
    }
    value = lhs % rhs;
    return true;
  }
  if (expr->op == "&") {
    value = lhs & rhs;
    return true;
  }
  if (expr->op == "|") {
    value = lhs | rhs;
    return true;
  }
  if (expr->op == "^") {
    value = lhs ^ rhs;
    return true;
  }
  if (expr->op == "<<" || expr->op == ">>") {
    if (rhs < 0 || rhs > 31) {
      return false;
    }
    value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
    return true;
  }
  if (expr->op == "==") {
    value = lhs == rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "!=") {
    value = lhs != rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "<") {
    value = lhs < rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "<=") {
    value = lhs <= rhs ? 1 : 0;
    return true;
  }
  if (expr->op == ">") {
    value = lhs > rhs ? 1 : 0;
    return true;
  }
  if (expr->op == ">=") {
    value = lhs >= rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "&&") {
    value = (lhs != 0 && rhs != 0) ? 1 : 0;
    return true;
  }
  if (expr->op == "||") {
    value = (lhs != 0 || rhs != 0) ? 1 : 0;
    return true;
  }
  return false;
}

static bool ResolveGlobalInitializerValues(const std::vector<GlobalDecl> &globals, std::vector<int> &values) {
  values.clear();
  values.reserve(globals.size());
  std::unordered_map<std::string, int> resolved_globals;
  for (const auto &global : globals) {
    int value = 0;
    if (!EvalConstExpr(global.value.get(), value, &resolved_globals)) {
      return false;
    }
    values.push_back(value);
    resolved_globals[global.name] = value;
  }
  return true;
}

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_has_invalid_type_suffix;
  ValueType return_type = ValueType::I32;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

static ValueType ScopeLookupType(const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                                 const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  return ValueType::Unknown;
}

using StaticScalarBindings = std::unordered_map<std::string, int>;

static bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                               const StaticScalarBindings *bindings = nullptr);
static bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings = nullptr);

static bool IsBoolLikeI32Literal(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    return true;
  }
  return expr->kind == Expr::Kind::Number && (expr->number == 0 || expr->number == 1);
}

static bool TryEvalStaticTruthiness(const Expr *expr, bool &value, const StaticScalarBindings *bindings = nullptr);

static bool TryEvalStaticArithmeticBinary(const std::string &op, int lhs, int rhs, int &value) {
  const auto int_min = std::numeric_limits<int>::min();
  const auto int_max = std::numeric_limits<int>::max();
  if (op == "/" || op == "%") {
    if (rhs == 0) {
      return false;
    }
    if (lhs == int_min && rhs == -1) {
      return false;
    }
    if (op == "/") {
      value = lhs / rhs;
      return true;
    }
    value = lhs % rhs;
    return true;
  }
  long long result = 0;
  if (op == "+") {
    result = static_cast<long long>(lhs) + static_cast<long long>(rhs);
  } else if (op == "-") {
    result = static_cast<long long>(lhs) - static_cast<long long>(rhs);
  } else if (op == "*") {
    result = static_cast<long long>(lhs) * static_cast<long long>(rhs);
  } else {
    return false;
  }
  if (result < static_cast<long long>(int_min) || result > static_cast<long long>(int_max)) {
    return false;
  }
  value = static_cast<int>(result);
  return true;
}

static bool TryEvalStaticBitwiseShiftBinary(const std::string &op, int lhs, int rhs, int &value) {
  if (op == "&") {
    value = lhs & rhs;
    return true;
  }
  if (op == "|") {
    value = lhs | rhs;
    return true;
  }
  if (op == "^") {
    value = lhs ^ rhs;
    return true;
  }
  if (op == "<<" || op == ">>") {
    if (rhs < 0 || rhs >= std::numeric_limits<int>::digits || lhs < 0) {
      return false;
    }
    if (op == "<<") {
      const auto shifted = static_cast<unsigned long long>(lhs) << rhs;
      if (shifted > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
        return false;
      }
      value = static_cast<int>(shifted);
      return true;
    }
    value = lhs >> rhs;
    return true;
  }
  return false;
}

static bool TryEvalStaticScalarValue(const Expr *expr, int &value, const StaticScalarBindings *bindings) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BoolLiteral) {
    value = expr->bool_value ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    value = 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Number) {
    value = expr->number;
    return true;
  }
  if (expr->kind == Expr::Kind::Identifier && bindings != nullptr) {
    auto it = bindings->find(expr->ident);
    if (it != bindings->end()) {
      value = it->second;
      return true;
    }
  }
  if (expr->kind == Expr::Kind::Conditional) {
    bool cond_truthy = false;
    if (!TryEvalStaticTruthiness(expr->left.get(), cond_truthy, bindings)) {
      return false;
    }
    const Expr *selected = cond_truthy ? expr->right.get() : expr->third.get();
    if (selected == nullptr) {
      return false;
    }
    return TryEvalStaticScalarValue(selected, value, bindings);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    return TryEvalStaticArithmeticBinary(expr->op, lhs, rhs, value);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    return TryEvalStaticBitwiseShiftBinary(expr->op, lhs, rhs, value);
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "&&" || expr->op == "||")) {
    bool lhs_truthy = false;
    if (!TryEvalStaticTruthiness(expr->left.get(), lhs_truthy, bindings)) {
      return false;
    }
    if (expr->op == "&&") {
      if (!lhs_truthy) {
        value = 0;
        return true;
      }
      bool rhs_truthy = false;
      if (!TryEvalStaticTruthiness(expr->right.get(), rhs_truthy, bindings)) {
        return false;
      }
      value = rhs_truthy ? 1 : 0;
      return true;
    }
    if (lhs_truthy) {
      value = 1;
      return true;
    }
    bool rhs_truthy = false;
    if (!TryEvalStaticTruthiness(expr->right.get(), rhs_truthy, bindings)) {
      return false;
    }
    value = rhs_truthy ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Binary && expr->left != nullptr && expr->right != nullptr &&
      (expr->op == "==" || expr->op == "!=" || expr->op == "<" || expr->op == "<=" || expr->op == ">" ||
       expr->op == ">=")) {
    int lhs = 0;
    int rhs = 0;
    if (!TryEvalStaticScalarValue(expr->left.get(), lhs, bindings) ||
        !TryEvalStaticScalarValue(expr->right.get(), rhs, bindings)) {
      return false;
    }
    bool cmp = false;
    if (expr->op == "==") {
      cmp = lhs == rhs;
    } else if (expr->op == "!=") {
      cmp = lhs != rhs;
    } else if (expr->op == "<") {
      cmp = lhs < rhs;
    } else if (expr->op == "<=") {
      cmp = lhs <= rhs;
    } else if (expr->op == ">") {
      cmp = lhs > rhs;
    } else if (expr->op == ">=") {
      cmp = lhs >= rhs;
    }
    value = cmp ? 1 : 0;
    return true;
  }
  return false;
}

static bool TryEvalStaticTruthiness(const Expr *expr, bool &value, const StaticScalarBindings *bindings) {
  int scalar = 0;
  if (!TryEvalStaticScalarValue(expr, scalar, bindings)) {
    return false;
  }
  value = scalar != 0;
  return true;
}

static bool ExprIsStaticallyFalse(const Expr *expr, const StaticScalarBindings *bindings = nullptr) {
  bool truthy = false;
  return TryEvalStaticTruthiness(expr, truthy, bindings) && !truthy;
}

static bool ExprIsStaticallyTrue(const Expr *expr, const StaticScalarBindings *bindings = nullptr) {
  bool truthy = false;
  return TryEvalStaticTruthiness(expr, truthy, bindings) && truthy;
}

static bool SupportsGenericParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsNullabilityParamTypeSuffix(const FuncParam &param) {
  return param.id_spelling || param.class_spelling || param.instancetype_spelling;
}

static bool SupportsGenericReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool SupportsNullabilityReturnTypeSuffix(const FunctionDecl &fn) {
  return fn.return_id_spelling || fn.return_class_spelling || fn.return_instancetype_spelling;
}

static bool HasInvalidParamTypeSuffix(const FuncParam &param) {
  const bool has_unsupported_generic_suffix = param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param);
  const bool has_unsupported_nullability_suffix =
      !param.nullability_suffix_tokens.empty() && !SupportsNullabilityParamTypeSuffix(param);
  return has_unsupported_generic_suffix || has_unsupported_nullability_suffix;
}

static void ValidateParameterTypeSuffixes(const FunctionDecl &fn, std::vector<std::string> &diagnostics) {
  for (const auto &param : fn.params) {
    if (param.has_generic_suffix && !SupportsGenericParamTypeSuffix(param)) {
      std::string suffix = param.generic_suffix_text;
      if (suffix.empty()) {
        suffix = "<...>";
      }
      diagnostics.push_back(MakeDiag(param.generic_line, param.generic_column, "O3S206",
                                     "type mismatch: generic parameter type suffix '" + suffix +
                                         "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                         param.name + "'"));
    }
    if (!SupportsNullabilityParamTypeSuffix(param)) {
      for (const auto &token : param.nullability_suffix_tokens) {
        diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                       "type mismatch: nullability parameter type suffix '" + token.text +
                                           "' is unsupported for non-id/Class/instancetype parameter annotation '" +
                                           param.name + "'"));
      }
    }
  }
}

static void ValidateReturnTypeSuffixes(const FunctionDecl &fn, std::vector<std::string> &diagnostics) {
  if (fn.has_return_generic_suffix && !SupportsGenericReturnTypeSuffix(fn)) {
    std::string suffix = fn.return_generic_suffix_text;
    if (suffix.empty()) {
      suffix = "<...>";
    }
    diagnostics.push_back(MakeDiag(fn.return_generic_line, fn.return_generic_column, "O3S206",
                                   "type mismatch: unsupported function return type suffix '" + suffix +
                                       "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                       "'"));
  }
  if (!SupportsNullabilityReturnTypeSuffix(fn)) {
    for (const auto &token : fn.return_nullability_suffix_tokens) {
      diagnostics.push_back(MakeDiag(token.line, token.column, "O3S206",
                                     "type mismatch: unsupported function return type suffix '" + token.text +
                                         "' for non-id/Class/instancetype return annotation in function '" + fn.name +
                                         "'"));
    }
  }
}

struct PureContractEffectInfo {
  struct SourceLoc {
    unsigned line = 1;
    unsigned column = 1;
    bool present = false;
  };

  SourceLoc global_write_site;
  SourceLoc message_send_site;
  std::unordered_map<std::string, SourceLoc> called_functions;
};

struct PureContractCause {
  std::string token;
  unsigned line = 1;
  unsigned column = 1;
  bool present = false;
  std::string detail_token;
  unsigned detail_line = 1;
  unsigned detail_column = 1;
  bool detail_present = false;
};

static bool IsEarlierPureContractSourceLoc(unsigned line, unsigned column, const PureContractEffectInfo::SourceLoc &loc) {
  if (!loc.present) {
    return true;
  }
  if (line < loc.line) {
    return true;
  }
  if (line > loc.line) {
    return false;
  }
  return column < loc.column;
}

static void RecordPureContractSourceLoc(PureContractEffectInfo::SourceLoc &loc, unsigned line, unsigned column) {
  if (!IsEarlierPureContractSourceLoc(line, column, loc)) {
    return;
  }
  loc.line = line;
  loc.column = column;
  loc.present = true;
}

static std::vector<std::string> SortedPureContractNames(const std::unordered_map<std::string, PureContractEffectInfo::SourceLoc> &names) {
  std::vector<std::string> ordered;
  ordered.reserve(names.size());
  for (const auto &entry : names) {
    ordered.push_back(entry.first);
  }
  std::sort(ordered.begin(), ordered.end());
  return ordered;
}

static PureContractCause DetermineDirectPureContractImpurityCause(const PureContractEffectInfo &info) {
  PureContractCause cause;
  if (info.global_write_site.present) {
    cause.token = "global-write";
    cause.line = info.global_write_site.line;
    cause.column = info.global_write_site.column;
    cause.present = true;
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
    return cause;
  }
  if (info.message_send_site.present) {
    cause.token = "message-send";
    cause.line = info.message_send_site.line;
    cause.column = info.message_send_site.column;
    cause.present = true;
    cause.detail_token = cause.token;
    cause.detail_line = cause.line;
    cause.detail_column = cause.column;
    cause.detail_present = true;
    return cause;
  }
  return cause;
}

static bool IsBetterPureContractCause(const PureContractCause &candidate, const PureContractCause &current) {
  if (!candidate.present) {
    return false;
  }
  if (!current.present) {
    return true;
  }
  if (candidate.token != current.token) {
    return candidate.token < current.token;
  }
  if (candidate.line != current.line) {
    return candidate.line < current.line;
  }
  return candidate.column < current.column;
}

static bool IsNameBoundInSemanticScopes(const std::vector<std::unordered_set<std::string>> &scopes,
                                        const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->find(name) != it->end()) {
      return true;
    }
  }
  return false;
}

static bool IsPureContractGlobalWriteTarget(const std::string &name,
                                            const std::vector<std::unordered_set<std::string>> &scopes,
                                            const std::unordered_set<std::string> &globals) {
  if (name.empty() || IsNameBoundInSemanticScopes(scopes, name)) {
    return false;
  }
  return globals.find(name) != globals.end();
}

static void CollectPureContractEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                          PureContractEffectInfo &info);

static void CollectPureContractEffectForClause(const ForClause &clause,
                                               std::vector<std::unordered_set<std::string>> &scopes,
                                               const std::unordered_set<std::string> &globals,
                                               PureContractEffectInfo &info) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      return;
    case ForClause::Kind::Let:
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      if (!scopes.empty() && !clause.name.empty()) {
        scopes.back().insert(clause.name);
      }
      return;
    case ForClause::Kind::Assign:
      if (IsPureContractGlobalWriteTarget(clause.name, scopes, globals)) {
        RecordPureContractSourceLoc(info.global_write_site, clause.line, clause.column);
      }
      CollectPureContractEffectExpr(clause.value.get(), scopes, info);
      return;
  }
}

static void CollectPureContractEffectStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes,
                                          const std::unordered_set<std::string> &globals, PureContractEffectInfo &info) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->let_stmt->value.get(), scopes, info);
      if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
        scopes.back().insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt == nullptr) {
        return;
      }
      if (IsPureContractGlobalWriteTarget(stmt->assign_stmt->name, scopes, globals)) {
        RecordPureContractSourceLoc(info.global_write_site, stmt->assign_stmt->line, stmt->assign_stmt->column);
      }
      CollectPureContractEffectExpr(stmt->assign_stmt->value.get(), scopes, info);
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectPureContractEffectExpr(stmt->return_stmt->value.get(), scopes, info);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectPureContractEffectExpr(stmt->expr_stmt->value.get(), scopes, info);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->if_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectPureContractEffectStmt(then_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      scopes.push_back({});
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectPureContractEffectStmt(else_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      CollectPureContractEffectExpr(stmt->do_while_stmt->condition.get(), scopes, info);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      CollectPureContractEffectForClause(stmt->for_stmt->init, scopes, globals, info);
      CollectPureContractEffectExpr(stmt->for_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      CollectPureContractEffectForClause(stmt->for_stmt->step, scopes, globals, info);
      scopes.pop_back();
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->switch_stmt->condition.get(), scopes, info);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        scopes.push_back({});
        for (const auto &case_body_stmt : case_stmt.body) {
          CollectPureContractEffectStmt(case_body_stmt.get(), scopes, globals, info);
        }
        scopes.pop_back();
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      CollectPureContractEffectExpr(stmt->while_stmt->condition.get(), scopes, info);
      scopes.push_back({});
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        CollectPureContractEffectStmt(loop_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        CollectPureContractEffectStmt(nested_stmt.get(), scopes, globals, info);
      }
      scopes.pop_back();
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

static void CollectPureContractEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                          PureContractEffectInfo &info) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
      return;
    case Expr::Kind::Binary:
      CollectPureContractEffectExpr(expr->left.get(), scopes, info);
      CollectPureContractEffectExpr(expr->right.get(), scopes, info);
      return;
    case Expr::Kind::Conditional:
      CollectPureContractEffectExpr(expr->left.get(), scopes, info);
      CollectPureContractEffectExpr(expr->right.get(), scopes, info);
      CollectPureContractEffectExpr(expr->third.get(), scopes, info);
      return;
    case Expr::Kind::Call:
      RecordPureContractSourceLoc(info.called_functions[expr->ident], expr->line, expr->column);
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
    case Expr::Kind::MessageSend:
      RecordPureContractSourceLoc(info.message_send_site, expr->line, expr->column);
      CollectPureContractEffectExpr(expr->receiver.get(), scopes, info);
      for (const auto &arg : expr->args) {
        CollectPureContractEffectExpr(arg.get(), scopes, info);
      }
      return;
  }
}

static void ValidatePureContractSemanticDiagnostics(const Objc3Program &program,
                                                    const std::unordered_map<std::string, FunctionInfo> &surface_functions,
                                                    std::vector<std::string> &diagnostics) {
  std::unordered_set<std::string> globals;
  for (const auto &global : program.globals) {
    globals.insert(global.name);
  }

  std::unordered_set<std::string> defined_functions;
  std::unordered_map<std::string, bool> pure_annotations;
  for (const auto &entry : surface_functions) {
    pure_annotations[entry.first] = entry.second.is_pure_annotation;
  }

  std::unordered_map<std::string, PureContractEffectInfo> function_effects;
  for (const auto &fn : program.functions) {
    if (fn.is_prototype) {
      continue;
    }
    defined_functions.insert(fn.name);

    PureContractEffectInfo info;
    std::vector<std::unordered_set<std::string>> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      scopes.back().insert(param.name);
    }
    for (const auto &stmt : fn.body) {
      CollectPureContractEffectStmt(stmt.get(), scopes, globals, info);
    }
    function_effects[fn.name] = std::move(info);
  }

  std::vector<std::string> ordered_functions;
  ordered_functions.reserve(function_effects.size());
  for (const auto &entry : function_effects) {
    ordered_functions.push_back(entry.first);
  }
  std::sort(ordered_functions.begin(), ordered_functions.end());

  std::unordered_set<std::string> impure_functions;
  std::unordered_map<std::string, PureContractCause> impure_causes;
  for (const std::string &name : ordered_functions) {
    const auto effect_it = function_effects.find(name);
    if (effect_it == function_effects.end()) {
      continue;
    }
    const PureContractCause direct_cause = DetermineDirectPureContractImpurityCause(effect_it->second);
    if (direct_cause.present) {
      impure_functions.insert(name);
      impure_causes[name] = direct_cause;
    }
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (const std::string &name : ordered_functions) {
      if (impure_functions.find(name) != impure_functions.end()) {
        continue;
      }
      const auto effect_it = function_effects.find(name);
      if (effect_it == function_effects.end()) {
        continue;
      }
      const std::vector<std::string> callees = SortedPureContractNames(effect_it->second.called_functions);
      PureContractCause selected_cause;
      for (const std::string &callee : callees) {
        const bool callee_defined = defined_functions.find(callee) != defined_functions.end();
        const bool callee_pure = pure_annotations.find(callee) != pure_annotations.end() && pure_annotations[callee];
        PureContractCause candidate_cause;
        const auto call_site_it = effect_it->second.called_functions.find(callee);
        if (call_site_it != effect_it->second.called_functions.end() && call_site_it->second.present) {
          candidate_cause.line = call_site_it->second.line;
          candidate_cause.column = call_site_it->second.column;
          candidate_cause.present = true;
        }
        if (!callee_defined && !callee_pure) {
          candidate_cause.token = "unannotated-extern-call:" + callee;
          if (candidate_cause.present) {
            candidate_cause.detail_token = candidate_cause.token;
            candidate_cause.detail_line = candidate_cause.line;
            candidate_cause.detail_column = candidate_cause.column;
            candidate_cause.detail_present = true;
          }
        } else if (impure_functions.find(callee) != impure_functions.end()) {
          candidate_cause.token = "impure-callee:" + callee;
          const auto callee_cause_it = impure_causes.find(callee);
          if (callee_cause_it != impure_causes.end()) {
            const PureContractCause &callee_cause = callee_cause_it->second;
            if (callee_cause.detail_present) {
              candidate_cause.detail_token = callee_cause.detail_token;
              candidate_cause.detail_line = callee_cause.detail_line;
              candidate_cause.detail_column = callee_cause.detail_column;
              candidate_cause.detail_present = true;
            } else if (callee_cause.present) {
              candidate_cause.detail_token = callee_cause.token;
              candidate_cause.detail_line = callee_cause.line;
              candidate_cause.detail_column = callee_cause.column;
              candidate_cause.detail_present = true;
            }
          }
        }

        if (IsBetterPureContractCause(candidate_cause, selected_cause)) {
          selected_cause = candidate_cause;
        }
      }
      if (!selected_cause.present) {
        continue;
      }
      if (!selected_cause.detail_present) {
        selected_cause.detail_token = selected_cause.token;
        selected_cause.detail_line = selected_cause.line;
        selected_cause.detail_column = selected_cause.column;
        selected_cause.detail_present = true;
      }
      impure_functions.insert(name);
      impure_causes[name] = selected_cause;
      changed = true;
    }
  }

  std::unordered_set<std::string> reported;
  for (const auto &fn : program.functions) {
    if (fn.is_prototype || !fn.is_pure) {
      continue;
    }
    if (impure_functions.find(fn.name) == impure_functions.end()) {
      continue;
    }
    if (!reported.insert(fn.name).second) {
      continue;
    }
    PureContractCause cause;
    const auto cause_it = impure_causes.find(fn.name);
    if (cause_it != impure_causes.end()) {
      cause = cause_it->second;
    }
    if (!cause.present) {
      cause.token = "unknown";
      cause.line = fn.line;
      cause.column = fn.column;
      cause.present = true;
      cause.detail_token = cause.token;
      cause.detail_line = cause.line;
      cause.detail_column = cause.column;
      cause.detail_present = true;
    }
    if (!cause.detail_present) {
      cause.detail_token = cause.token;
      cause.detail_line = cause.line;
      cause.detail_column = cause.column;
      cause.detail_present = true;
    }
    diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S215",
                                   "pure contract violation: function '" + fn.name +
                                       "' declared 'pure' has side effects (cause: " + cause.token +
                                       "; cause-site:" + std::to_string(cause.line) + ":" +
                                       std::to_string(cause.column) + "; detail:" + cause.detail_token + "@" +
                                       std::to_string(cause.detail_line) + ":" +
                                       std::to_string(cause.detail_column) + ")"));
  }
}

static constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
static constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
static constexpr const char *kObjc3RuntimeDispatchSymbol = "objc3_msgsend_i32";

struct Objc3LoweringContract {
  std::size_t max_message_send_args = kObjc3RuntimeDispatchDefaultArgs;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
};

struct Objc3FrontendOptions {
  Objc3LoweringContract lowering;
};

struct Objc3FrontendStageDiagnostics {
  std::vector<std::string> lexer;
  std::vector<std::string> parser;
  std::vector<std::string> semantic;
};

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  bool built = false;
};

struct Objc3FrontendPipelineResult {
  Objc3Program program;
  Objc3FrontendStageDiagnostics stage_diagnostics;
  Objc3SemanticIntegrationSurface integration_surface;
};

static bool IsMessageI32CompatibleType(ValueType type) {
  return type == ValueType::I32 || type == ValueType::Bool;
}

static ValueType ValidateMessageSendExpr(const Expr *expr,
                                         const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                                         const std::unordered_map<std::string, ValueType> &globals,
                                         const std::unordered_map<std::string, FunctionInfo> &functions,
                                         std::vector<std::string> &diagnostics,
                                         std::size_t max_message_send_args);

static ValueType ValidateExpr(const Expr *expr, const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              std::vector<std::string> &diagnostics,
                              std::size_t max_message_send_args) {
  if (expr == nullptr) {
    return ValueType::Unknown;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return ValueType::I32;
    case Expr::Kind::BoolLiteral:
      return ValueType::Bool;
    case Expr::Kind::NilLiteral:
      return ValueType::I32;
    case Expr::Kind::Identifier: {
      const ValueType local_type = ScopeLookupType(scopes, expr->ident);
      if (local_type != ValueType::Unknown) {
        return local_type;
      }
      auto global_it = globals.find(expr->ident);
      if (global_it != globals.end()) {
        return global_it->second;
      }
      if (functions.find(expr->ident) != functions.end()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: function '" + expr->ident +
                                           "' cannot be used as a value"));
        return ValueType::Function;
      }
      diagnostics.push_back(
          MakeDiag(expr->line, expr->column, "O3S202", "undefined identifier '" + expr->ident + "'"));
      return ValueType::Unknown;
    }
    case Expr::Kind::Binary: {
      const ValueType lhs =
          ValidateExpr(expr->left.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      const ValueType rhs =
          ValidateExpr(expr->right.get(), scopes, globals, functions, diagnostics, max_message_send_args);

      if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
        if (lhs != ValueType::Unknown && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::I32;
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
        if (lhs != ValueType::Unknown && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::I32;
      }

      if (expr->op == "==" || expr->op == "!=") {
        const bool bool_to_i32_literal =
            (lhs == ValueType::Bool && rhs == ValueType::I32 && IsBoolLikeI32Literal(expr->right.get())) ||
            (rhs == ValueType::Bool && lhs == ValueType::I32 && IsBoolLikeI32Literal(expr->left.get()));
        if (lhs != ValueType::Unknown && rhs != ValueType::Unknown && lhs != rhs && !bool_to_i32_literal) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: equality compares '" + std::string(TypeName(lhs)) +
                                             "' with '" + std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::Bool;
      }

      if (expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
        if (lhs != ValueType::Unknown && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::Bool;
      }

      if (expr->op == "&&" || expr->op == "||") {
        if (lhs != ValueType::Unknown && lhs != ValueType::Bool && lhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical lhs, got '" +
                                             std::string(TypeName(lhs)) + "'"));
        }
        if (rhs != ValueType::Unknown && rhs != ValueType::Bool && rhs != ValueType::I32) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical rhs, got '" +
                                             std::string(TypeName(rhs)) + "'"));
        }
        return ValueType::Bool;
      }

      return ValueType::Unknown;
    }
    case Expr::Kind::Conditional: {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return ValueType::Unknown;
      }

      const ValueType condition_type =
          ValidateExpr(expr->left.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional condition must be bool-compatible"));
      }

      const ValueType then_type =
          ValidateExpr(expr->right.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      const ValueType else_type =
          ValidateExpr(expr->third.get(), scopes, globals, functions, diagnostics, max_message_send_args);

      if (then_type == ValueType::Unknown) {
        return else_type;
      }
      if (else_type == ValueType::Unknown) {
        return then_type;
      }
      const bool then_scalar = then_type == ValueType::I32 || then_type == ValueType::Bool;
      const bool else_scalar = else_type == ValueType::I32 || else_type == ValueType::Bool;
      if (then_scalar && else_scalar) {
        if (then_type == else_type) {
          return then_type;
        }
        return ValueType::I32;
      }
      if (then_type != else_type) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional branches must be scalar-compatible"));
      }
      return then_type == else_type ? then_type : ValueType::Unknown;
    }
    case Expr::Kind::Call: {
      auto fn_it = functions.find(expr->ident);
      if (fn_it == functions.end()) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S203", "unknown function '" + expr->ident + "'"));
      } else if (fn_it->second.arity != expr->args.size()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S204",
                                       "arity mismatch for function '" + expr->ident + "'"));
      }

      for (std::size_t i = 0; i < expr->args.size(); ++i) {
        const ValueType arg_type = ValidateExpr(expr->args[i].get(), scopes, globals, functions, diagnostics,
                                                max_message_send_args);
        if (fn_it != functions.end() && i < fn_it->second.param_types.size()) {
          if (i < fn_it->second.param_has_invalid_type_suffix.size() &&
              fn_it->second.param_has_invalid_type_suffix[i]) {
            continue;
          }
          const ValueType expected = fn_it->second.param_types[i];
          const bool bool_coercion = expected == ValueType::Bool && arg_type == ValueType::I32;
          if (arg_type != ValueType::Unknown && expected != ValueType::Unknown && arg_type != expected &&
              !bool_coercion) {
            diagnostics.push_back(MakeDiag(expr->args[i]->line, expr->args[i]->column, "O3S206",
                                           "type mismatch: expected '" + std::string(TypeName(expected)) +
                                               "' argument for parameter " + std::to_string(i) + " of '" +
                                               expr->ident + "', got '" + std::string(TypeName(arg_type)) + "'"));
          }
        }
      }
      if (fn_it != functions.end()) {
        return fn_it->second.return_type;
      }
      return ValueType::Unknown;
    }
    case Expr::Kind::MessageSend: {
      return ValidateMessageSendExpr(expr, scopes, globals, functions, diagnostics, max_message_send_args);
    }
  }
  return ValueType::Unknown;
}

static ValueType ValidateMessageSendExpr(const Expr *expr,
                                         const std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                                         const std::unordered_map<std::string, ValueType> &globals,
                                         const std::unordered_map<std::string, FunctionInfo> &functions,
                                         std::vector<std::string> &diagnostics,
                                         std::size_t max_message_send_args) {
  const ValueType receiver_type =
      ValidateExpr(expr->receiver.get(), scopes, globals, functions, diagnostics, max_message_send_args);
  const std::string selector = expr->selector.empty() ? "<unknown>" : expr->selector;
  if (receiver_type != ValueType::Unknown && !IsMessageI32CompatibleType(receiver_type)) {
    const unsigned diag_line = expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column = expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(diag_line, diag_column, "O3S207",
                                   "type mismatch: message receiver for selector '" + selector +
                                       "' must be i32-compatible, got '" +
                                       std::string(TypeName(receiver_type)) + "'"));
  }

  if (expr->args.size() > max_message_send_args) {
    diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S208",
                                   "arity mismatch: message '" + selector + "' has " +
                                       std::to_string(expr->args.size()) +
                                       " argument(s); native frontend supports at most " +
                                       std::to_string(max_message_send_args)));
  }

  for (std::size_t i = 0; i < expr->args.size(); ++i) {
    const auto &arg = expr->args[i];
    const ValueType arg_type =
        ValidateExpr(arg.get(), scopes, globals, functions, diagnostics, max_message_send_args);
    if (arg_type != ValueType::Unknown && !IsMessageI32CompatibleType(arg_type)) {
      diagnostics.push_back(MakeDiag(arg->line, arg->column, "O3S209",
                                     "type mismatch: message argument " + std::to_string(i) +
                                         " for selector '" + selector +
                                         "' must be i32-compatible, got '" +
                                         std::string(TypeName(arg_type)) + "'"));
    }
  }
  return ValueType::I32;
}

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               ValueType expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               std::size_t max_message_send_args);

static void ValidateAssignmentCompatibility(const std::string &target_name, const std::string &op,
                                           const Expr *value_expr, unsigned line, unsigned column,
                                           bool found_target, ValueType target_type, ValueType value_type,
                                           std::vector<std::string> &diagnostics) {
  if (op == "=") {
    const bool target_known_scalar = target_type == ValueType::I32 || target_type == ValueType::Bool;
    const bool value_known_scalar = value_type == ValueType::I32 || value_type == ValueType::Bool;
    const bool assign_matches =
        target_type == value_type || (target_type == ValueType::I32 && value_type == ValueType::Bool) ||
        (target_type == ValueType::Bool && value_type == ValueType::I32 && IsBoolLikeI32Literal(value_expr));
    if (found_target && target_known_scalar && value_type != ValueType::Unknown && !value_known_scalar) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         std::string(TypeName(target_type)) + "', got '" +
                                         std::string(TypeName(value_type)) + "'"));
      return;
    }
    if (found_target && target_known_scalar && value_known_scalar && !assign_matches) {
      diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                     "type mismatch: assignment to '" + target_name + "' expects '" +
                                         std::string(TypeName(target_type)) + "', got '" +
                                         std::string(TypeName(value_type)) + "'"));
    }
    return;
  }

  if (!IsCompoundAssignmentOperator(op)) {
    if (op == "++" || op == "--") {
      if (found_target && target_type != ValueType::Unknown && target_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                       "type mismatch: update operator '" + op + "' target '" + target_name +
                                           "' must be 'i32', got '" + std::string(TypeName(target_type)) + "'"));
      }
      return;
    }
    diagnostics.push_back(
        MakeDiag(line, column, "O3S206", "type mismatch: unsupported assignment operator '" + op + "'"));
    return;
  }
  if (!found_target) {
    return;
  }
  if (target_type != ValueType::Unknown && target_type != ValueType::I32) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' target '" + target_name +
                                       "' must be 'i32', got '" + std::string(TypeName(target_type)) + "'"));
  }
  if (target_type == ValueType::I32 && value_type != ValueType::Unknown && value_type != ValueType::I32) {
    diagnostics.push_back(MakeDiag(line, column, "O3S206",
                                   "type mismatch: compound assignment '" + op + "' value for '" + target_name +
                                       "' must be 'i32', got '" + std::string(TypeName(value_type)) + "'"));
  }
}

static void ValidateStatement(const Stmt *stmt, std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              ValueType expected_return_type, const std::string &function_name,
                              std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                              std::size_t max_message_send_args) {
  if (stmt == nullptr) {
    return;
  }
  const auto resolve_assignment_target_type = [&](const std::string &target_name, ValueType &target_type) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      auto found = it->find(target_name);
      if (found != it->end()) {
        target_type = found->second;
        return true;
      }
    }
    auto global_it = globals.find(target_name);
    if (global_it != globals.end()) {
      target_type = global_it->second;
      return true;
    }
    return false;
  };
  const auto validate_for_clause = [&](const ForClause &clause) {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        (void)ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        return;
      case ForClause::Kind::Let: {
        if (scopes.empty()) {
          return;
        }
        const ValueType value_type =
            ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        if (scopes.back().find(clause.name) != scopes.back().end()) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S201",
                                         "duplicate declaration '" + clause.name + "'"));
        } else {
          scopes.back().emplace(clause.name, value_type);
        }
        return;
      }
      case ForClause::Kind::Assign: {
        if (scopes.empty()) {
          return;
        }
        ValueType target_type = ValueType::Unknown;
        const bool found_target = resolve_assignment_target_type(clause.name, target_type);
        if (!found_target) {
          diagnostics.push_back(MakeDiag(clause.line, clause.column, "O3S214",
                                         "invalid assignment target '" + clause.name +
                                             "': target must be a mutable symbol"));
        }
        const ValueType value_type =
            ValidateExpr(clause.value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        ValidateAssignmentCompatibility(clause.name, clause.op, clause.value.get(), clause.line, clause.column,
                                        found_target, target_type, value_type, diagnostics);
        return;
      }
    }
  };

  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      const LetStmt *let = stmt->let_stmt.get();
      if (let == nullptr || scopes.empty()) {
        return;
      }
      const ValueType value_type =
          ValidateExpr(let->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (scopes.back().find(let->name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(let->line, let->column, "O3S201",
                                       "duplicate declaration '" + let->name + "'"));
      } else {
        scopes.back().emplace(let->name, value_type);
      }
      return;
    }
    case Stmt::Kind::Assign: {
      const AssignStmt *assign = stmt->assign_stmt.get();
      if (assign == nullptr || scopes.empty()) {
        return;
      }
      ValueType target_type = ValueType::Unknown;
      const bool found_target = resolve_assignment_target_type(assign->name, target_type);
      if (!found_target) {
        diagnostics.push_back(MakeDiag(assign->line, assign->column, "O3S214",
                                       "invalid assignment target '" + assign->name +
                                           "': target must be a mutable symbol"));
      }
      const ValueType value_type =
          ValidateExpr(assign->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      ValidateAssignmentCompatibility(assign->name, assign->op, assign->value.get(), assign->line, assign->column,
                                      found_target, target_type, value_type, diagnostics);
      return;
    }
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (ret->value == nullptr) {
          if (expected_return_type != ValueType::Void) {
            diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                           "type mismatch: function '" + function_name + "' must return '" +
                                               std::string(TypeName(expected_return_type)) + "'"));
          }
          return;
        }

        if (expected_return_type == ValueType::Void) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: void function '" + function_name +
                                             "' must use 'return;'"));
          (void)ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
          return;
        }

        const ValueType return_type =
            ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        const bool return_matches =
            return_type == expected_return_type ||
            (expected_return_type == ValueType::I32 && return_type == ValueType::Bool) ||
            (expected_return_type == ValueType::Bool && return_type == ValueType::I32 &&
             IsBoolLikeI32Literal(ret->value.get()));
        if (!return_matches && return_type != ValueType::Unknown && return_type != ValueType::Function) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: return expression in function '" + function_name +
                                             "' must be '" + std::string(TypeName(expected_return_type)) +
                                             "', got '" + std::string(TypeName(return_type)) + "'"));
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        (void)ValidateExpr(stmt->expr_stmt->value.get(), scopes, globals, functions, diagnostics,
                           max_message_send_args);
      }
      return;
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      const ValueType condition_type =
          ValidateExpr(if_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(if_stmt->line, if_stmt->column, "O3S206",
                                       "type mismatch: if condition must be bool-compatible"));
      }
      scopes.push_back({});
      ValidateStatements(if_stmt->then_body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth, max_message_send_args);
      scopes.pop_back();
      scopes.push_back({});
      ValidateStatements(if_stmt->else_body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth, max_message_send_args);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
      if (do_while_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(do_while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth, max_message_send_args);
      scopes.pop_back();

      const ValueType condition_type =
          ValidateExpr(do_while_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(do_while_stmt->line, do_while_stmt->column, "O3S206",
                                       "type mismatch: do-while condition must be bool-compatible"));
      }
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
      if (for_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      validate_for_clause(for_stmt->init);
      if (for_stmt->condition != nullptr) {
        const ValueType condition_type =
            ValidateExpr(for_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
        if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
            condition_type != ValueType::I32) {
          diagnostics.push_back(MakeDiag(for_stmt->line, for_stmt->column, "O3S206",
                                         "type mismatch: for condition must be bool-compatible"));
        }
      }
      validate_for_clause(for_stmt->step);
      scopes.push_back({});
      ValidateStatements(for_stmt->body, scopes, globals, functions, expected_return_type, function_name, diagnostics,
                         loop_depth + 1, switch_depth, max_message_send_args);
      scopes.pop_back();
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }
      const ValueType condition_type =
          ValidateExpr(switch_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(switch_stmt->line, switch_stmt->column, "O3S206",
                                       "type mismatch: switch condition must be i32-compatible"));
      }

      std::unordered_set<int> seen_case_values;
      bool seen_default = false;
      for (const auto &case_stmt : switch_stmt->cases) {
        if (case_stmt.is_default) {
          if (seen_default) {
            diagnostics.push_back(MakeDiag(case_stmt.line, case_stmt.column, "O3S206",
                                           "type mismatch: duplicate default label in switch"));
          }
          seen_default = true;
        } else {
          if (!seen_case_values.insert(case_stmt.value).second) {
            diagnostics.push_back(MakeDiag(case_stmt.value_line, case_stmt.value_column, "O3S206",
                                           "type mismatch: duplicate case label '" +
                                               std::to_string(case_stmt.value) + "' in switch"));
          }
        }
        scopes.push_back({});
        ValidateStatements(case_stmt.body, scopes, globals, functions, expected_return_type, function_name,
                           diagnostics, loop_depth, switch_depth + 1, max_message_send_args);
        scopes.pop_back();
      }
      return;
    }
    case Stmt::Kind::While: {
      const WhileStmt *while_stmt = stmt->while_stmt.get();
      if (while_stmt == nullptr) {
        return;
      }
      const ValueType condition_type =
          ValidateExpr(while_stmt->condition.get(), scopes, globals, functions, diagnostics, max_message_send_args);
      if (condition_type != ValueType::Unknown && condition_type != ValueType::Bool &&
          condition_type != ValueType::I32) {
        diagnostics.push_back(MakeDiag(while_stmt->line, while_stmt->column, "O3S206",
                                       "type mismatch: while condition must be bool-compatible"));
      }
      scopes.push_back({});
      ValidateStatements(while_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth + 1, switch_depth, max_message_send_args);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Block: {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr) {
        return;
      }
      scopes.push_back({});
      ValidateStatements(block_stmt->body, scopes, globals, functions, expected_return_type, function_name,
                         diagnostics, loop_depth, switch_depth, max_message_send_args);
      scopes.pop_back();
      return;
    }
    case Stmt::Kind::Break:
      if (loop_depth <= 0 && switch_depth <= 0) {
        diagnostics.push_back(MakeDiag(stmt->line, stmt->column, "O3S212", "loop-control misuse: 'break' outside loop"));
      }
      return;
    case Stmt::Kind::Continue:
      if (loop_depth <= 0) {
        diagnostics.push_back(
            MakeDiag(stmt->line, stmt->column, "O3S213", "loop-control misuse: 'continue' outside loop"));
      }
      return;
    case Stmt::Kind::Empty:
      return;
  }
}

static void ValidateStatements(const std::vector<std::unique_ptr<Stmt>> &statements,
                               std::vector<std::unordered_map<std::string, ValueType>> &scopes,
                               const std::unordered_map<std::string, ValueType> &globals,
                               const std::unordered_map<std::string, FunctionInfo> &functions,
                               ValueType expected_return_type, const std::string &function_name,
                               std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                               std::size_t max_message_send_args) {
  for (const auto &stmt : statements) {
    ValidateStatement(stmt.get(), scopes, globals, functions, expected_return_type, function_name, diagnostics,
                      loop_depth, switch_depth, max_message_send_args);
  }
}

static bool StatementReturnsOrFallsThroughToNextCase(const Stmt *stmt, const StaticScalarBindings *bindings = nullptr);

static bool BlockReturnsOrFallsThroughToNextCase(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                 const StaticScalarBindings *bindings = nullptr) {
  for (const auto &stmt : statements) {
    if (StatementAlwaysReturns(stmt.get(), bindings)) {
      return true;
    }
    if (!StatementReturnsOrFallsThroughToNextCase(stmt.get(), bindings)) {
      return false;
    }
  }
  return true;
}

static bool StatementReturnsOrFallsThroughToNextCase(const Stmt *stmt, const StaticScalarBindings *bindings) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
    case Stmt::Kind::Assign:
    case Stmt::Kind::Expr:
    case Stmt::Kind::Empty:
      return true;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      return BlockReturnsOrFallsThroughToNextCase(stmt->block_stmt->body, bindings);
    case Stmt::Kind::If: {
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      const IfStmt *if_stmt = stmt->if_stmt.get();
      const bool then_ok = BlockReturnsOrFallsThroughToNextCase(if_stmt->then_body, bindings);
      const bool else_ok =
          if_stmt->else_body.empty() ? true : BlockReturnsOrFallsThroughToNextCase(if_stmt->else_body, bindings);
      if (ExprIsStaticallyTrue(if_stmt->condition.get(), bindings)) {
        return then_ok;
      }
      if (ExprIsStaticallyFalse(if_stmt->condition.get(), bindings)) {
        return else_ok;
      }
      return then_ok && else_ok;
    }
    case Stmt::Kind::Switch:
      // Nested switches that do not already guarantee return may still complete and
      // continue with deterministic fallthrough into subsequent outer case-body statements.
      return true;
    case Stmt::Kind::Return:
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      if (!ExprIsStaticallyFalse(stmt->do_while_stmt->condition.get(), bindings)) {
        return false;
      }
      return BlockReturnsOrFallsThroughToNextCase(stmt->do_while_stmt->body, bindings);
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr || stmt->for_stmt->condition == nullptr) {
        return false;
      }
      return ExprIsStaticallyFalse(stmt->for_stmt->condition.get(), bindings);
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      return ExprIsStaticallyFalse(stmt->while_stmt->condition.get(), bindings);
  }
  return false;
}

static bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings) {
  if (stmt == nullptr) {
    return false;
  }
  if (stmt->kind == Stmt::Kind::Switch && stmt->switch_stmt != nullptr) {
    const auto &cases = stmt->switch_stmt->cases;
    if (cases.empty()) {
      return false;
    }

    bool has_default = false;
    std::vector<bool> arm_guarantees(cases.size(), false);
    bool next_arm_guarantees_return = false;

    for (std::size_t offset = 0; offset < cases.size(); ++offset) {
      const std::size_t i = cases.size() - 1 - offset;
      const auto &case_stmt = cases[i];
      has_default = has_default || case_stmt.is_default;

      const bool body_guarantees_return = BlockAlwaysReturns(case_stmt.body, bindings);
      if (body_guarantees_return) {
        arm_guarantees[i] = true;
      } else if (BlockReturnsOrFallsThroughToNextCase(case_stmt.body, bindings)) {
        // Case bodies that either return or fall through chain deterministically to the next case arm.
        arm_guarantees[i] = next_arm_guarantees_return;
      } else {
        arm_guarantees[i] = false;
      }
      next_arm_guarantees_return = arm_guarantees[i];
    }

    int static_switch_value = 0;
    if (TryEvalStaticScalarValue(stmt->switch_stmt->condition.get(), static_switch_value, bindings)) {
      std::size_t default_index = cases.size();
      std::size_t selected_index = cases.size();
      for (std::size_t i = 0; i < cases.size(); ++i) {
        const auto &case_stmt = cases[i];
        if (case_stmt.is_default) {
          if (default_index == cases.size()) {
            default_index = i;
          }
          continue;
        }
        if (static_switch_value == case_stmt.value) {
          selected_index = i;
          break;
        }
      }
      if (selected_index == cases.size()) {
        selected_index = default_index;
      }
      if (selected_index == cases.size()) {
        return false;
      }
      return arm_guarantees[selected_index];
    }

    if (!has_default) {
      return false;
    }
    for (bool arm_guarantees_return : arm_guarantees) {
      if (!arm_guarantees_return) {
        return false;
      }
    }
    return true;
  }
  if (stmt->kind == Stmt::Kind::Return) {
    return true;
  }
  if (stmt->kind == Stmt::Kind::Block && stmt->block_stmt != nullptr) {
    return BlockAlwaysReturns(stmt->block_stmt->body, bindings);
  }
  if (stmt->kind == Stmt::Kind::If && stmt->if_stmt != nullptr) {
    const IfStmt *if_stmt = stmt->if_stmt.get();
    if (ExprIsStaticallyTrue(if_stmt->condition.get(), bindings)) {
      if (if_stmt->then_body.empty()) {
        return false;
      }
      return BlockAlwaysReturns(if_stmt->then_body, bindings);
    }
    if (ExprIsStaticallyFalse(if_stmt->condition.get(), bindings)) {
      if (if_stmt->else_body.empty()) {
        return false;
      }
      return BlockAlwaysReturns(if_stmt->else_body, bindings);
    }
    if (if_stmt->then_body.empty() || if_stmt->else_body.empty()) {
      return false;
    }
    return BlockAlwaysReturns(if_stmt->then_body, bindings) && BlockAlwaysReturns(if_stmt->else_body, bindings);
  }
  if (stmt->kind == Stmt::Kind::While && stmt->while_stmt != nullptr) {
    if (!ExprIsStaticallyTrue(stmt->while_stmt->condition.get(), bindings)) {
      return false;
    }
    return BlockAlwaysReturns(stmt->while_stmt->body, bindings);
  }
  if (stmt->kind == Stmt::Kind::For && stmt->for_stmt != nullptr) {
    const bool guaranteed_entry = (stmt->for_stmt->condition == nullptr) ||
                                  ExprIsStaticallyTrue(stmt->for_stmt->condition.get(), bindings);
    if (!guaranteed_entry) {
      return false;
    }
    return BlockAlwaysReturns(stmt->for_stmt->body, bindings);
  }
  if (stmt->kind == Stmt::Kind::DoWhile && stmt->do_while_stmt != nullptr) {
    return BlockAlwaysReturns(stmt->do_while_stmt->body, bindings);
  }
  return false;
}

static bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                               const StaticScalarBindings *bindings) {
  for (const auto &stmt : statements) {
    if (StatementAlwaysReturns(stmt.get(), bindings)) {
      return true;
    }
  }
  return false;
}

static void CollectAssignedIdentifiers(const std::vector<std::unique_ptr<Stmt>> &statements,
                                       std::unordered_set<std::string> &assigned);

static void CollectAssignedIdentifiersFromStmt(const Stmt *stmt, std::unordered_set<std::string> &assigned) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        assigned.insert(stmt->assign_stmt->name);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->block_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->if_stmt->then_body, assigned);
        CollectAssignedIdentifiers(stmt->if_stmt->else_body, assigned);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->do_while_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        if (stmt->for_stmt->init.kind == ForClause::Kind::Assign) {
          assigned.insert(stmt->for_stmt->init.name);
        }
        if (stmt->for_stmt->step.kind == ForClause::Kind::Assign) {
          assigned.insert(stmt->for_stmt->step.name);
        }
        CollectAssignedIdentifiers(stmt->for_stmt->body, assigned);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectAssignedIdentifiers(case_stmt.body, assigned);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectAssignedIdentifiers(stmt->while_stmt->body, assigned);
      }
      return;
    default:
      return;
  }
}

static void CollectAssignedIdentifiers(const std::vector<std::unique_ptr<Stmt>> &statements,
                                       std::unordered_set<std::string> &assigned) {
  for (const auto &stmt : statements) {
    CollectAssignedIdentifiersFromStmt(stmt.get(), assigned);
  }
}

static void CollectNonTopLevelLetNames(const std::vector<std::unique_ptr<Stmt>> &statements, bool is_top_level,
                                       std::unordered_set<std::string> &names);

static void CollectNonTopLevelLetNamesFromStmt(const Stmt *stmt, bool is_top_level,
                                               std::unordered_set<std::string> &names) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (!is_top_level && stmt->let_stmt != nullptr) {
        names.insert(stmt->let_stmt->name);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->block_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->if_stmt->then_body, false, names);
        CollectNonTopLevelLetNames(stmt->if_stmt->else_body, false, names);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->do_while_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        if (stmt->for_stmt->init.kind == ForClause::Kind::Let) {
          names.insert(stmt->for_stmt->init.name);
        }
        CollectNonTopLevelLetNames(stmt->for_stmt->body, false, names);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectNonTopLevelLetNames(case_stmt.body, false, names);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectNonTopLevelLetNames(stmt->while_stmt->body, false, names);
      }
      return;
    default:
      return;
  }
}

static void CollectNonTopLevelLetNames(const std::vector<std::unique_ptr<Stmt>> &statements, bool is_top_level,
                                       std::unordered_set<std::string> &names) {
  for (const auto &stmt : statements) {
    CollectNonTopLevelLetNamesFromStmt(stmt.get(), is_top_level, names);
  }
}

static void CollectSwitchConditionIdentifierNames(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                  std::unordered_set<std::string> &names);

static void CollectSwitchConditionIdentifierNamesFromStmt(const Stmt *stmt, std::unordered_set<std::string> &names) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        const Expr *condition = stmt->switch_stmt->condition.get();
        if (condition != nullptr && condition->kind == Expr::Kind::Identifier && !condition->ident.empty()) {
          names.insert(condition->ident);
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          CollectSwitchConditionIdentifierNames(case_stmt.body, names);
        }
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->block_stmt->body, names);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->if_stmt->then_body, names);
        CollectSwitchConditionIdentifierNames(stmt->if_stmt->else_body, names);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->do_while_stmt->body, names);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->for_stmt->body, names);
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectSwitchConditionIdentifierNames(stmt->while_stmt->body, names);
      }
      return;
    default:
      return;
  }
}

static void CollectSwitchConditionIdentifierNames(const std::vector<std::unique_ptr<Stmt>> &statements,
                                                  std::unordered_set<std::string> &names) {
  for (const auto &stmt : statements) {
    CollectSwitchConditionIdentifierNamesFromStmt(stmt.get(), names);
  }
}

static StaticScalarBindings CollectFunctionStaticScalarBindings(const FunctionDecl &fn,
                                                               const StaticScalarBindings *global_bindings = nullptr) {
  std::unordered_set<std::string> assigned;
  CollectAssignedIdentifiers(fn.body, assigned);

  std::unordered_set<std::string> non_top_level_lets;
  CollectNonTopLevelLetNames(fn.body, true, non_top_level_lets);

  std::unordered_set<std::string> switch_condition_identifiers;
  CollectSwitchConditionIdentifierNames(fn.body, switch_condition_identifiers);

  StaticScalarBindings bindings;
  for (const auto &stmt : fn.body) {
    if (stmt == nullptr || stmt->kind != Stmt::Kind::Let || stmt->let_stmt == nullptr || stmt->let_stmt->value == nullptr) {
      continue;
    }
    const std::string &name = stmt->let_stmt->name;
    if (assigned.find(name) != assigned.end() || non_top_level_lets.find(name) != non_top_level_lets.end() ||
        switch_condition_identifiers.find(name) != switch_condition_identifiers.end()) {
      continue;
    }
    int value = 0;
    if (TryEvalStaticScalarValue(stmt->let_stmt->value.get(), value, &bindings)) {
      bindings[name] = value;
    }
  }

  if (global_bindings != nullptr) {
    for (const auto &[name, value] : *global_bindings) {
      if (bindings.find(name) != bindings.end()) {
        continue;
      }
      if (assigned.find(name) != assigned.end() || non_top_level_lets.find(name) != non_top_level_lets.end() ||
          switch_condition_identifiers.find(name) != switch_condition_identifiers.end()) {
        continue;
      }
      bindings[name] = value;
    }
  }
  return bindings;
}

static Objc3SemanticIntegrationSurface BuildSemanticIntegrationSurface(const Objc3Program &program,
                                                                       std::vector<std::string> &diagnostics) {
  Objc3SemanticIntegrationSurface surface;
  std::unordered_map<std::string, int> resolved_global_values;

  for (const auto &global : program.globals) {
    const bool duplicate_global = surface.globals.find(global.name) != surface.globals.end();
    if (duplicate_global) {
      diagnostics.push_back(MakeDiag(global.line, global.column, "O3S200", "duplicate global '" + global.name + "'"));
    } else {
      surface.globals.emplace(global.name, ValueType::I32);
    }
    int value = 0;
    if (!EvalConstExpr(global.value.get(), value, &resolved_global_values)) {
      diagnostics.push_back(
          MakeDiag(global.line, global.column, "O3S210", "global initializer must be constant expression"));
    } else if (!duplicate_global) {
      resolved_global_values.emplace(global.name, value);
    }
  }

  for (const auto &fn : program.functions) {
    if (surface.globals.find(fn.name) != surface.globals.end()) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      continue;
    }

    auto it = surface.functions.find(fn.name);
    if (it == surface.functions.end()) {
      FunctionInfo info;
      info.arity = fn.params.size();
      info.param_types.reserve(fn.params.size());
      info.param_has_invalid_type_suffix.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        info.param_types.push_back(param.type);
        info.param_has_invalid_type_suffix.push_back(HasInvalidParamTypeSuffix(param));
      }
      info.return_type = fn.return_type;
      info.has_definition = !fn.is_prototype;
      info.is_pure_annotation = fn.is_pure;
      surface.functions.emplace(fn.name, std::move(info));
      continue;
    }

    FunctionInfo &existing = it->second;
    bool compatible = existing.arity == fn.params.size() && existing.return_type == fn.return_type;
    if (compatible) {
      for (std::size_t i = 0; i < fn.params.size(); ++i) {
        if (existing.param_types[i] != fn.params[i].type) {
          compatible = false;
          break;
        }
      }
    }
    if (!compatible) {
      diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S206",
                                     "type mismatch: incompatible function signature for '" + fn.name + "'"));
      continue;
    }

    for (std::size_t i = 0; i < fn.params.size() && i < existing.param_has_invalid_type_suffix.size(); ++i) {
      existing.param_has_invalid_type_suffix[i] =
          existing.param_has_invalid_type_suffix[i] || HasInvalidParamTypeSuffix(fn.params[i]);
    }
    existing.is_pure_annotation = existing.is_pure_annotation || fn.is_pure;

    if (!fn.is_prototype) {
      if (existing.has_definition) {
        diagnostics.push_back(MakeDiag(fn.line, fn.column, "O3S200", "duplicate function '" + fn.name + "'"));
      } else {
        existing.has_definition = true;
      }
    }
  }

  surface.built = true;
  return surface;
}

static void ValidateSemanticBodies(const Objc3Program &program, const Objc3SemanticIntegrationSurface &surface,
                                   const Objc3FrontendOptions &options, std::vector<std::string> &diagnostics) {
  StaticScalarBindings global_static_bindings;
  std::unordered_set<std::string> assigned_identifier_names;
  for (const auto &fn : program.functions) {
    CollectAssignedIdentifiers(fn.body, assigned_identifier_names);
  }
  std::vector<int> global_initializer_values;
  if (ResolveGlobalInitializerValues(program.globals, global_initializer_values)) {
    const std::size_t count = std::min(program.globals.size(), global_initializer_values.size());
    for (std::size_t i = 0; i < count; ++i) {
      const std::string &name = program.globals[i].name;
      if (assigned_identifier_names.find(name) != assigned_identifier_names.end()) {
        continue;
      }
      global_static_bindings[name] = global_initializer_values[i];
    }
  }

  for (const auto &fn : program.functions) {
    ValidateReturnTypeSuffixes(fn, diagnostics);
    ValidateParameterTypeSuffixes(fn, diagnostics);

    std::vector<std::unordered_map<std::string, ValueType>> scopes;
    scopes.push_back({});
    for (const auto &param : fn.params) {
      if (scopes.back().find(param.name) != scopes.back().end()) {
        diagnostics.push_back(MakeDiag(param.line, param.column, "O3S201", "duplicate parameter '" + param.name + "'"));
      } else {
        scopes.back().emplace(param.name, param.type);
      }
    }

    if (!fn.is_prototype) {
      const StaticScalarBindings static_scalar_bindings = CollectFunctionStaticScalarBindings(fn, &global_static_bindings);
      ValidateStatements(fn.body, scopes, surface.globals, surface.functions, fn.return_type, fn.name, diagnostics,
                         0, 0, options.lowering.max_message_send_args);
      if (fn.return_type != ValueType::Void && !BlockAlwaysReturns(fn.body, &static_scalar_bindings)) {
        diagnostics.push_back(
            MakeDiag(fn.line, fn.column, "O3S205", "missing return path in function '" + fn.name + "'"));
      }
    }
  }
}

static Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                            const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3Lexer lexer(source);
  std::vector<Token> tokens = lexer.Run(result.stage_diagnostics.lexer);

  Objc3Parser parser(tokens);
  result.program = parser.Parse();
  result.stage_diagnostics.parser = parser.TakeDiagnostics();

  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    result.integration_surface = BuildSemanticIntegrationSurface(result.program, result.stage_diagnostics.semantic);
    ValidateSemanticBodies(result.program, result.integration_surface, options, result.stage_diagnostics.semantic);
    ValidatePureContractSemanticDiagnostics(result.program, result.integration_surface.functions,
                                            result.stage_diagnostics.semantic);
  }

  result.program.diagnostics.reserve(result.stage_diagnostics.lexer.size() + result.stage_diagnostics.parser.size() +
                                     result.stage_diagnostics.semantic.size());
  result.program.diagnostics.insert(result.program.diagnostics.end(), result.stage_diagnostics.lexer.begin(),
                                    result.stage_diagnostics.lexer.end());
  result.program.diagnostics.insert(result.program.diagnostics.end(), result.stage_diagnostics.parser.begin(),
                                    result.stage_diagnostics.parser.end());
  result.program.diagnostics.insert(result.program.diagnostics.end(), result.stage_diagnostics.semantic.begin(),
                                    result.stage_diagnostics.semantic.end());
  NormalizeDiagnostics(result.program.diagnostics);
  return result;
}

class Objc3IREmitter {
 public:
  Objc3IREmitter(const Objc3Program &program, const Objc3LoweringContract &lowering_contract)
      : program_(program), lowering_contract_(lowering_contract) {
    for (const auto &global : program_.globals) {
      globals_.insert(global.name);
    }
    for (const auto &fn : program_.functions) {
      function_arity_[fn.name] = fn.params.size();
      if (fn.is_pure) {
        declared_pure_functions_.insert(fn.name);
      }
      if (!fn.is_prototype && defined_functions_.insert(fn.name).second) {
        function_definitions_.push_back(&fn);
      }
    }
    function_signatures_ = BuildLoweredFunctionSignatures(program_);
    CollectSelectorLiterals();
    CollectMutableGlobalSymbols();
    CollectFunctionEffects();
  }

  bool Emit(std::string &ir, std::string &error) {
    runtime_dispatch_call_emitted_ = false;
    std::ostringstream body;

    std::vector<int> resolved_global_values;
    if (!ResolveGlobalInitializerValues(program_.globals, resolved_global_values) ||
        resolved_global_values.size() != program_.globals.size()) {
      error = "global initializer failed const evaluation";
      return false;
    }
    global_const_values_.clear();
    global_nil_proven_symbols_.clear();
    for (std::size_t i = 0; i < program_.globals.size(); ++i) {
      if (mutable_global_symbols_.find(program_.globals[i].name) == mutable_global_symbols_.end()) {
        global_const_values_[program_.globals[i].name] = resolved_global_values[i];
      }
      body << "@" << program_.globals[i].name << " = global i32 " << resolved_global_values[i] << ", align 4\n";
    }
    for (const auto &global : program_.globals) {
      if (mutable_global_symbols_.find(global.name) != mutable_global_symbols_.end()) {
        continue;
      }
      if (IsCompileTimeGlobalNilExpr(global.value.get())) {
        global_nil_proven_symbols_.insert(global.name);
      }
    }
    if (!program_.globals.empty()) {
      body << "\n";
    }

    EmitSelectorConstants(body);

    EmitPrototypeDeclarations(body);

    for (const FunctionDecl *fn : function_definitions_) {
      EmitFunction(*fn, body);
      body << "\n";
    }

    EmitEntryPoint(body);

    std::ostringstream out;
    out << "; objc3c native frontend IR\n";
    out << "source_filename = \"" << program_.module_name << ".objc3\"\n\n";
    if (runtime_dispatch_call_emitted_) {
      out << "declare i32 @" << lowering_contract_.runtime_dispatch_symbol << "(i32, ptr";
      for (std::size_t i = 0; i < lowering_contract_.max_message_send_args; ++i) {
        out << ", i32";
      }
      out << ")\n\n";
    }
    out << body.str();
    ir = out.str();
    return true;
  }

 private:
  struct LoweredFunctionSignature {
    ValueType return_type = ValueType::I32;
    std::vector<ValueType> param_types;
  };

  struct FunctionEffectInfo {
    bool has_global_write = false;
    bool has_message_send = false;
    std::unordered_set<std::string> called_functions;
  };

  struct LoweredMessageSend {
    std::string receiver = "0";
    bool receiver_is_compile_time_zero = false;
    bool receiver_is_compile_time_nonzero = false;
    std::vector<std::string> args;
    std::string selector;
  };

  struct ControlLabels {
    std::string continue_label;
    std::string break_label;
    bool continue_allowed = false;
  };

  struct FunctionContext {
    std::vector<std::string> entry_lines;
    std::vector<std::string> code_lines;
    std::vector<std::unordered_map<std::string, std::string>> scopes;
    std::vector<ControlLabels> control_stack;
    std::unordered_set<std::string> nil_bound_ptrs;
    std::unordered_set<std::string> nonzero_bound_ptrs;
    std::unordered_map<std::string, int> const_value_ptrs;
    ValueType return_type = ValueType::I32;
    int temp_counter = 0;
    int label_counter = 0;
    bool terminated = false;
    bool global_proofs_invalidated = false;
  };

  static const char *LLVMScalarType(ValueType type) {
    if (type == ValueType::Bool) {
      return "i1";
    }
    if (type == ValueType::Void) {
      return "void";
    }
    return "i32";
  }

  static std::map<std::string, LoweredFunctionSignature> BuildLoweredFunctionSignatures(const Objc3Program &program) {
    std::map<std::string, LoweredFunctionSignature> signatures;
    for (const auto &fn : program.functions) {
      LoweredFunctionSignature signature;
      signature.return_type = fn.return_type;
      signature.param_types.reserve(fn.params.size());
      for (const auto &param : fn.params) {
        signature.param_types.push_back(param.type);
      }
      auto existing = signatures.find(fn.name);
      if (existing == signatures.end()) {
        signatures.emplace(fn.name, std::move(signature));
      }
    }
    return signatures;
  }

  static std::string EscapeCStringLiteral(const std::string &text) {
    std::ostringstream out;
    for (unsigned char c : text) {
      if (c == '\\' || c == '"') {
        out << '\\' << static_cast<char>(c);
        continue;
      }
      if (c >= 32 && c <= 126) {
        out << static_cast<char>(c);
        continue;
      }
      std::ostringstream byte;
      byte << std::hex << std::uppercase << static_cast<int>(c);
      std::string value = byte.str();
      if (value.size() < 2) {
        value = "0" + value;
      }
      out << "\\" << value;
    }
    return out.str();
  }

  void RegisterSelectorLiteral(const std::string &selector) {
    if (selector.empty() || selector_globals_.find(selector) != selector_globals_.end()) {
      return;
    }
    selector_globals_.emplace(selector, "");
  }

  void AssignSelectorGlobalNames() {
    std::size_t index = 0;
    for (auto &entry : selector_globals_) {
      entry.second = "@.objc3.sel." + std::to_string(index++);
    }
  }

  void CollectSelectorExpr(const Expr *expr) {
    if (expr == nullptr) {
      return;
    }
    switch (expr->kind) {
      case Expr::Kind::MessageSend:
        RegisterSelectorLiteral(expr->selector);
        CollectSelectorExpr(expr->receiver.get());
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      case Expr::Kind::Binary:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        return;
      case Expr::Kind::Conditional:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        CollectSelectorExpr(expr->third.get());
        return;
      case Expr::Kind::Call:
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      default:
        return;
    }
  }

  void CollectSelectorStmt(const Stmt *stmt) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr) {
          CollectSelectorExpr(stmt->let_stmt->value.get());
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          CollectSelectorExpr(stmt->assign_stmt->value.get());
        }
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectSelectorExpr(stmt->return_stmt->value.get());
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectSelectorExpr(stmt->expr_stmt->value.get());
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->if_stmt->condition.get());
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectSelectorStmt(then_stmt.get());
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectSelectorStmt(else_stmt.get());
        }
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        CollectSelectorExpr(stmt->do_while_stmt->condition.get());
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->for_stmt->init.value.get());
        CollectSelectorExpr(stmt->for_stmt->condition.get());
        CollectSelectorExpr(stmt->for_stmt->step.value.get());
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->switch_stmt->condition.get());
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectSelectorStmt(case_body_stmt.get());
          }
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->while_stmt->condition.get());
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Block:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectSelectorStmt(nested_stmt.get());
        }
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectSelectorLiterals() {
    for (const auto &global : program_.globals) {
      CollectSelectorExpr(global.value.get());
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        CollectSelectorStmt(stmt.get());
      }
    }
    AssignSelectorGlobalNames();
  }

  static bool IsNameBoundInScopes(const std::vector<std::unordered_set<std::string>> &scopes,
                                  const std::string &name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      if (it->find(name) != it->end()) {
        return true;
      }
    }
    return false;
  }

  void NotePotentialGlobalMutation(const std::string &name,
                                   const std::vector<std::unordered_set<std::string>> &scopes) {
    if (name.empty() || IsNameBoundInScopes(scopes, name)) {
      return;
    }
    if (globals_.find(name) != globals_.end()) {
      mutable_global_symbols_.insert(name);
    }
  }

  void CollectMutableGlobalSymbolsForClause(const ForClause &clause, std::vector<std::unordered_set<std::string>> &scopes) {
    switch (clause.kind) {
      case ForClause::Kind::None:
      case ForClause::Kind::Expr:
        return;
      case ForClause::Kind::Let:
        if (!scopes.empty() && !clause.name.empty()) {
          scopes.back().insert(clause.name);
        }
        return;
      case ForClause::Kind::Assign:
        NotePotentialGlobalMutation(clause.name, scopes);
        return;
    }
  }

  void CollectMutableGlobalSymbolsStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr && !stmt->let_stmt->name.empty() && !scopes.empty()) {
          scopes.back().insert(stmt->let_stmt->name);
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          NotePotentialGlobalMutation(stmt->assign_stmt->name, scopes);
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectMutableGlobalSymbolsStmt(then_stmt.get(), scopes);
        }
        scopes.pop_back();
        scopes.push_back({});
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectMutableGlobalSymbolsStmt(else_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        CollectMutableGlobalSymbolsForClause(stmt->for_stmt->init, scopes);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        CollectMutableGlobalSymbolsForClause(stmt->for_stmt->step, scopes);
        scopes.pop_back();
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectMutableGlobalSymbolsStmt(case_body_stmt.get(), scopes);
          }
          scopes.pop_back();
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectMutableGlobalSymbolsStmt(loop_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Block:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectMutableGlobalSymbolsStmt(nested_stmt.get(), scopes);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Return:
      case Stmt::Kind::Expr:
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectMutableGlobalSymbols() {
    mutable_global_symbols_.clear();
    for (const FunctionDecl *fn : function_definitions_) {
      if (fn == nullptr) {
        continue;
      }
      std::vector<std::unordered_set<std::string>> scopes;
      scopes.push_back({});
      for (const auto &param : fn->params) {
        scopes.back().insert(param.name);
      }
      for (const auto &stmt : fn->body) {
        CollectMutableGlobalSymbolsStmt(stmt.get(), scopes);
      }
    }
  }

  bool IsGlobalSymbolWriteTarget(const std::string &name,
                                 const std::vector<std::unordered_set<std::string>> &scopes) const {
    if (name.empty() || IsNameBoundInScopes(scopes, name)) {
      return false;
    }
    return globals_.find(name) != globals_.end();
  }

  void CollectFunctionEffectExpr(const Expr *expr, std::vector<std::unordered_set<std::string>> &scopes,
                                 FunctionEffectInfo &info) const {
    if (expr == nullptr) {
      return;
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
      case Expr::Kind::BoolLiteral:
      case Expr::Kind::NilLiteral:
      case Expr::Kind::Identifier:
        return;
      case Expr::Kind::Binary:
        CollectFunctionEffectExpr(expr->left.get(), scopes, info);
        CollectFunctionEffectExpr(expr->right.get(), scopes, info);
        return;
      case Expr::Kind::Conditional:
        CollectFunctionEffectExpr(expr->left.get(), scopes, info);
        CollectFunctionEffectExpr(expr->right.get(), scopes, info);
        CollectFunctionEffectExpr(expr->third.get(), scopes, info);
        return;
      case Expr::Kind::Call:
        info.called_functions.insert(expr->ident);
        for (const auto &arg : expr->args) {
          CollectFunctionEffectExpr(arg.get(), scopes, info);
        }
        return;
      case Expr::Kind::MessageSend:
        info.has_message_send = true;
        CollectFunctionEffectExpr(expr->receiver.get(), scopes, info);
        for (const auto &arg : expr->args) {
          CollectFunctionEffectExpr(arg.get(), scopes, info);
        }
        return;
    }
  }

  void CollectFunctionEffectForClause(const ForClause &clause, std::vector<std::unordered_set<std::string>> &scopes,
                                      FunctionEffectInfo &info) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        return;
      case ForClause::Kind::Let:
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        if (!scopes.empty() && !clause.name.empty()) {
          scopes.back().insert(clause.name);
        }
        return;
      case ForClause::Kind::Assign:
        if (IsGlobalSymbolWriteTarget(clause.name, scopes)) {
          info.has_global_write = true;
        }
        CollectFunctionEffectExpr(clause.value.get(), scopes, info);
        return;
    }
  }

  void CollectFunctionEffectStmt(const Stmt *stmt, std::vector<std::unordered_set<std::string>> &scopes,
                                 FunctionEffectInfo &info) const {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->let_stmt->value.get(), scopes, info);
        if (!scopes.empty() && !stmt->let_stmt->name.empty()) {
          scopes.back().insert(stmt->let_stmt->name);
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt == nullptr) {
          return;
        }
        if (IsGlobalSymbolWriteTarget(stmt->assign_stmt->name, scopes)) {
          info.has_global_write = true;
        }
        CollectFunctionEffectExpr(stmt->assign_stmt->value.get(), scopes, info);
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectFunctionEffectExpr(stmt->return_stmt->value.get(), scopes, info);
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectFunctionEffectExpr(stmt->expr_stmt->value.get(), scopes, info);
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->if_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectFunctionEffectStmt(then_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        scopes.push_back({});
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectFunctionEffectStmt(else_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        CollectFunctionEffectExpr(stmt->do_while_stmt->condition.get(), scopes, info);
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        CollectFunctionEffectForClause(stmt->for_stmt->init, scopes, info);
        CollectFunctionEffectExpr(stmt->for_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        CollectFunctionEffectForClause(stmt->for_stmt->step, scopes, info);
        scopes.pop_back();
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->switch_stmt->condition.get(), scopes, info);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          scopes.push_back({});
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectFunctionEffectStmt(case_body_stmt.get(), scopes, info);
          }
          scopes.pop_back();
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectFunctionEffectExpr(stmt->while_stmt->condition.get(), scopes, info);
        scopes.push_back({});
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectFunctionEffectStmt(loop_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Block:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        scopes.push_back({});
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectFunctionEffectStmt(nested_stmt.get(), scopes, info);
        }
        scopes.pop_back();
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectFunctionEffects() {
    function_effects_.clear();
    impure_functions_.clear();

    for (const FunctionDecl *fn : function_definitions_) {
      if (fn == nullptr) {
        continue;
      }
      FunctionEffectInfo info;
      std::vector<std::unordered_set<std::string>> scopes;
      scopes.push_back({});
      for (const auto &param : fn->params) {
        scopes.back().insert(param.name);
      }
      for (const auto &stmt : fn->body) {
        CollectFunctionEffectStmt(stmt.get(), scopes, info);
      }
      function_effects_[fn->name] = std::move(info);
    }

    for (const auto &entry : function_effects_) {
      const FunctionEffectInfo &info = entry.second;
      if (info.has_global_write || info.has_message_send) {
        impure_functions_.insert(entry.first);
      }
    }

    bool changed = true;
    while (changed) {
      changed = false;
      for (const auto &entry : function_effects_) {
        const std::string &name = entry.first;
        if (impure_functions_.find(name) != impure_functions_.end()) {
          continue;
        }
        for (const std::string &callee : entry.second.called_functions) {
          const bool callee_defined = defined_functions_.find(callee) != defined_functions_.end();
          const bool callee_declared_pure = declared_pure_functions_.find(callee) != declared_pure_functions_.end();
          if ((!callee_defined && !callee_declared_pure) ||
              impure_functions_.find(callee) != impure_functions_.end()) {
            impure_functions_.insert(name);
            changed = true;
            break;
          }
        }
      }
    }
  }

  bool FunctionMayHaveGlobalSideEffects(const std::string &name) const {
    if (name.empty()) {
      return true;
    }
    if (defined_functions_.find(name) == defined_functions_.end()) {
      return declared_pure_functions_.find(name) == declared_pure_functions_.end();
    }
    return impure_functions_.find(name) != impure_functions_.end();
  }

  void EmitSelectorConstants(std::ostringstream &out) const {
    for (const auto &entry : selector_globals_) {
      const std::string &selector = entry.first;
      const std::string &global_name = entry.second;
      const std::size_t storage_len = selector.size() + 1;
      out << global_name << " = private unnamed_addr constant [" << storage_len << " x i8] c\""
          << EscapeCStringLiteral(selector) << "\\00\", align 1\n";
    }
    if (!selector_globals_.empty()) {
      out << "\n";
    }
  }

  std::string NewTemp(FunctionContext &ctx) const { return "%t" + std::to_string(ctx.temp_counter++); }

  std::string NewLabel(FunctionContext &ctx, const std::string &prefix) const {
    return prefix + std::to_string(ctx.label_counter++);
  }

  std::string LookupVarPtr(const FunctionContext &ctx, const std::string &name) const {
    for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    if (globals_.find(name) != globals_.end()) {
      return "@" + name;
    }
    return "";
  }

  std::string CoerceI32ToBoolI1(const std::string &i32_value, FunctionContext &ctx) const {
    const std::string bool_i1 = NewTemp(ctx);
    ctx.code_lines.push_back("  " + bool_i1 + " = icmp ne i32 " + i32_value + ", 0");
    return bool_i1;
  }

  std::string CoerceValueToI32(const std::string &value, ValueType value_type, FunctionContext &ctx) const {
    if (value_type != ValueType::Bool) {
      return value;
    }
    const std::string widened = NewTemp(ctx);
    ctx.code_lines.push_back("  " + widened + " = zext i1 " + value + " to i32");
    return widened;
  }

  const LoweredFunctionSignature *LookupFunctionSignature(const std::string &name) const {
    auto signature_it = function_signatures_.find(name);
    if (signature_it == function_signatures_.end()) {
      return nullptr;
    }
    return &signature_it->second;
  }

  void AppendLoweredCallArg(std::vector<std::string> &args, const std::string &arg_i32, ValueType expected_type,
                            FunctionContext &ctx) const {
    if (expected_type == ValueType::Bool) {
      const std::string arg_i1 = CoerceI32ToBoolI1(arg_i32, ctx);
      args.push_back("i1 " + arg_i1);
      return;
    }
    args.push_back("i32 " + arg_i32);
  }

  void EmitTypedReturn(const std::string &i32_value, FunctionContext &ctx) const {
    if (ctx.return_type == ValueType::Void) {
      ctx.code_lines.push_back("  ret void");
      return;
    }
    if (ctx.return_type == ValueType::Bool) {
      const std::string bool_i1 = CoerceI32ToBoolI1(i32_value, ctx);
      ctx.code_lines.push_back("  ret i1 " + bool_i1);
      return;
    }
    ctx.code_lines.push_back("  ret i32 " + i32_value);
  }

  void EmitTypedParamStore(const FuncParam &param, std::size_t index, const std::string &ptr, FunctionContext &ctx) const {
    if (param.type == ValueType::Bool) {
      const std::string widened = "%arg" + std::to_string(index) + ".zext." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + widened + " = zext i1 %arg" + std::to_string(index) + " to i32");
      ctx.entry_lines.push_back("  store i32 " + widened + ", ptr " + ptr + ", align 4");
      return;
    }
    ctx.entry_lines.push_back("  store i32 %arg" + std::to_string(index) + ", ptr " + ptr + ", align 4");
  }

  void EmitAssignmentStore(const std::string &ptr, const std::string &op, const Expr *value_expr,
                           FunctionContext &ctx) const {
    if (ptr.empty()) {
      return;
    }
    int assigned_const_value = 0;
    const bool has_assigned_const_value =
        op == "=" && value_expr != nullptr && TryGetCompileTimeI32ExprInContext(value_expr, ctx, assigned_const_value);
    const bool has_assigned_nil_value = op == "=" && value_expr != nullptr && IsCompileTimeNilReceiverExprInContext(value_expr, ctx);
    // Any explicit write invalidates compile-time nil binding for this storage slot.
    ctx.nil_bound_ptrs.erase(ptr);
    // Any explicit write invalidates compile-time known non-zero binding for this storage slot.
    ctx.nonzero_bound_ptrs.erase(ptr);
    // Any explicit write invalidates tracked compile-time constant value for this storage slot.
    ctx.const_value_ptrs.erase(ptr);
    if (op == "++" || op == "--") {
      const std::string lhs = NewTemp(ctx);
      ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr + ", align 4");
      const std::string out = NewTemp(ctx);
      const std::string opcode = op == "++" ? "add" : "sub";
      ctx.code_lines.push_back("  " + out + " = " + opcode + " i32 " + lhs + ", 1");
      ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr + ", align 4");
      return;
    }
    if (op == "=") {
      if (value_expr == nullptr) {
        return;
      }
      const std::string value = EmitExpr(value_expr, ctx);
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
      if (has_assigned_nil_value && ptr.rfind("@", 0) != 0) {
        ctx.nil_bound_ptrs.insert(ptr);
      }
      if (has_assigned_const_value) {
        ctx.const_value_ptrs[ptr] = assigned_const_value;
        if (assigned_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
      }
      return;
    }

    if (value_expr == nullptr) {
      return;
    }
    std::string binary_opcode;
    if (!TryGetCompoundAssignmentBinaryOpcode(op, binary_opcode)) {
      const std::string value = EmitExpr(value_expr, ctx);
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
      return;
    }

    const std::string lhs = NewTemp(ctx);
    ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr + ", align 4");
    const std::string rhs = EmitExpr(value_expr, ctx);
    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + out + " = " + binary_opcode + " i32 " + lhs + ", " + rhs);
    ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr + ", align 4");
  }

  void EmitForClause(const ForClause &clause, FunctionContext &ctx) const {
    switch (clause.kind) {
      case ForClause::Kind::None:
        return;
      case ForClause::Kind::Expr:
        if (clause.value != nullptr) {
          (void)EmitExpr(clause.value.get(), ctx);
        }
        return;
      case ForClause::Kind::Assign: {
        const std::string ptr = LookupVarPtr(ctx, clause.name);
        EmitAssignmentStore(ptr, clause.op, clause.value.get(), ctx);
        return;
      }
      case ForClause::Kind::Let: {
        if (ctx.scopes.empty() || clause.value == nullptr) {
          return;
        }
        const std::string value = EmitExpr(clause.value.get(), ctx);
        const std::string ptr = "%" + clause.name + ".addr." + std::to_string(ctx.temp_counter++);
        int clause_const_value = 0;
        const bool has_clause_const_value = TryGetCompileTimeI32ExprInContext(clause.value.get(), ctx, clause_const_value);
        const bool has_clause_nil_value = IsCompileTimeNilReceiverExprInContext(clause.value.get(), ctx);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.scopes.back()[clause.name] = ptr;
        if (has_clause_nil_value) {
          ctx.nil_bound_ptrs.insert(ptr);
        }
        if (has_clause_const_value) {
          ctx.const_value_ptrs[ptr] = clause_const_value;
        }
        if (has_clause_const_value && clause_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
        return;
      }
    }
  }

  bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeNilReceiverExprInContext(expr->right.get(), ctx);
      }
      return IsCompileTimeNilReceiverExprInContext(expr->third.get(), ctx);
    }
    if (expr->kind != Expr::Kind::Identifier) {
      return false;
    }
    const std::string ptr = LookupVarPtr(ctx, expr->ident);
    if (ptr.empty()) {
      return false;
    }
    if (ctx.nil_bound_ptrs.find(ptr) != ctx.nil_bound_ptrs.end()) {
      return true;
    }
    if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    return false;
  }

  bool IsCompileTimeGlobalNilExpr(const Expr *expr) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      const FunctionContext global_eval_ctx;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), global_eval_ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeGlobalNilExpr(expr->right.get());
      }
      return IsCompileTimeGlobalNilExpr(expr->third.get());
    }
    return false;
  }

  bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::Number) {
      value = expr->number;
      return true;
    }
    if (expr->kind == Expr::Kind::BoolLiteral) {
      value = expr->bool_value ? 1 : 0;
      return true;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      value = 0;
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      const std::string ptr = LookupVarPtr(ctx, expr->ident);
      if (ptr.empty()) {
        return false;
      }
      auto value_it = ctx.const_value_ptrs.find(ptr);
      if (value_it != ctx.const_value_ptrs.end()) {
        value = value_it->second;
        return true;
      }
      if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
        auto global_it = global_const_values_.find(expr->ident);
        if (global_it != global_const_values_.end()) {
          value = global_it->second;
          return true;
        }
      }
      return false;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
      }
      return TryGetCompileTimeI32ExprInContext(expr->third.get(), ctx, value);
    }
    if (expr->kind != Expr::Kind::Binary || expr->left == nullptr || expr->right == nullptr) {
      return false;
    }
    if (expr->op == "&&" || expr->op == "||") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (expr->op == "&&") {
        if (lhs == 0) {
          value = 0;
          return true;
        }
        int rhs = 0;
        if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
          return false;
        }
        value = rhs != 0 ? 1 : 0;
        return true;
      }
      if (lhs != 0) {
        value = 1;
        return true;
      }
      int rhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
        return false;
      }
      value = rhs != 0 ? 1 : 0;
      return true;
    }
    int lhs = 0;
    int rhs = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs) ||
        !TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
      return false;
    }
    if (expr->op == "+") {
      value = lhs + rhs;
      return true;
    }
    if (expr->op == "-") {
      value = lhs - rhs;
      return true;
    }
    if (expr->op == "*") {
      value = lhs * rhs;
      return true;
    }
    if (expr->op == "/") {
      if (rhs == 0) {
        return false;
      }
      value = lhs / rhs;
      return true;
    }
    if (expr->op == "%") {
      if (rhs == 0) {
        return false;
      }
      value = lhs % rhs;
      return true;
    }
    if (expr->op == "&") {
      value = lhs & rhs;
      return true;
    }
    if (expr->op == "|") {
      value = lhs | rhs;
      return true;
    }
    if (expr->op == "^") {
      value = lhs ^ rhs;
      return true;
    }
    if (expr->op == "<<" || expr->op == ">>") {
      if (rhs < 0 || rhs > 31) {
        return false;
      }
      value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
      return true;
    }
    if (expr->op == "==") {
      value = lhs == rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "!=") {
      value = lhs != rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<") {
      value = lhs < rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<=") {
      value = lhs <= rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">") {
      value = lhs > rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">=") {
      value = lhs >= rhs ? 1 : 0;
      return true;
    }
    return false;
  }

  bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    int const_value = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr, ctx, const_value)) {
      return false;
    }
    return const_value != 0;
  }

  LoweredMessageSend LowerMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    LoweredMessageSend lowered;
    lowered.args.assign(lowering_contract_.max_message_send_args, "0");
    if (expr == nullptr) {
      return lowered;
    }

    lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);
    lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);
    lowered.receiver = EmitExpr(expr->receiver.get(), ctx);
    lowered.selector = expr->selector;
    for (std::size_t i = 0; i < expr->args.size() && i < lowered.args.size(); ++i) {
      lowered.args[i] = EmitExpr(expr->args[i].get(), ctx);
    }
    return lowered;
  }

  std::string EmitRuntimeDispatch(const LoweredMessageSend &lowered, FunctionContext &ctx) const {
    if (lowered.receiver_is_compile_time_zero) {
      return "0";
    }

    auto selector_it = selector_globals_.find(lowered.selector);
    if (selector_it == selector_globals_.end()) {
      return "0";
    }

    const std::size_t selector_len = lowered.selector.size() + 1;
    const std::string selector_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + selector_ptr + " = getelementptr inbounds [" + std::to_string(selector_len) +
                             " x i8], ptr " + selector_it->second + ", i32 0, i32 0");

    const auto emit_dispatch_call = [&](const std::string &dispatch_value) {
      std::ostringstream call;
      call << "  " << dispatch_value << " = call i32 @" << lowering_contract_.runtime_dispatch_symbol << "(i32 "
           << lowered.receiver << ", ptr " << selector_ptr;
      for (const std::string &arg : lowered.args) {
        call << ", i32 " << arg;
      }
      call << ")";
      runtime_dispatch_call_emitted_ = true;
      ctx.code_lines.push_back(call.str());
    };

    if (lowered.receiver_is_compile_time_nonzero) {
      const std::string dispatch_value = NewTemp(ctx);
      emit_dispatch_call(dispatch_value);
      InvalidateGlobalProofState(ctx);
      return dispatch_value;
    }

    const std::string is_nil = NewTemp(ctx);
    const std::string nil_label = NewLabel(ctx, "msg_nil_");
    const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");
    const std::string merge_label = NewLabel(ctx, "msg_merge_");
    const std::string dispatch_value = NewTemp(ctx);
    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + is_nil + " = icmp eq i32 " + lowered.receiver + ", 0");
    ctx.code_lines.push_back("  br i1 " + is_nil + ", label %" + nil_label + ", label %" + dispatch_label);
    ctx.code_lines.push_back(nil_label + ":");
    ctx.code_lines.push_back("  br label %" + merge_label);
    ctx.code_lines.push_back(dispatch_label + ":");
    emit_dispatch_call(dispatch_value);
    ctx.code_lines.push_back("  br label %" + merge_label);
    ctx.code_lines.push_back(merge_label + ":");
    ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +
                             dispatch_label + "]");
    InvalidateGlobalProofState(ctx);
    return out;
  }

  std::string EmitMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    const LoweredMessageSend lowered = LowerMessageSendExpr(expr, ctx);
    return EmitRuntimeDispatch(lowered, ctx);
  }

  std::string EmitExpr(const Expr *expr, FunctionContext &ctx) const {
    if (expr == nullptr) {
      return "0";
    }
    switch (expr->kind) {
      case Expr::Kind::Number:
        return std::to_string(expr->number);
      case Expr::Kind::BoolLiteral:
        return expr->bool_value ? "1" : "0";
      case Expr::Kind::NilLiteral:
        return "0";
      case Expr::Kind::Identifier: {
        const std::string ptr = LookupVarPtr(ctx, expr->ident);
        if (!ptr.empty()) {
          const std::string tmp = NewTemp(ctx);
          ctx.code_lines.push_back("  " + tmp + " = load i32, ptr " + ptr + ", align 4");
          return tmp;
        }
        if (globals_.find(expr->ident) != globals_.end()) {
          const std::string tmp = NewTemp(ctx);
          ctx.code_lines.push_back("  " + tmp + " = load i32, ptr @" + expr->ident + ", align 4");
          return tmp;
        }
        return "0";
      }
      case Expr::Kind::Binary: {
        if (expr->op == "&&" || expr->op == "||") {
          const std::string lhs = EmitExpr(expr->left.get(), ctx);
          const std::string lhs_i1 = NewTemp(ctx);
          const std::string rhs_label = NewLabel(ctx, expr->op == "&&" ? "and_rhs_" : "or_rhs_");
          const std::string rhs_done_label = NewLabel(ctx, expr->op == "&&" ? "and_rhs_done_" : "or_rhs_done_");
          const std::string short_label = NewLabel(ctx, expr->op == "&&" ? "and_short_" : "or_short_");
          const std::string merge_label = NewLabel(ctx, expr->op == "&&" ? "and_merge_" : "or_merge_");
          const std::string rhs_i1 = NewTemp(ctx);
          const std::string logical_i1 = NewTemp(ctx);
          const std::string out_i32 = NewTemp(ctx);
          const std::string short_value = expr->op == "&&" ? "0" : "1";

          ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs + ", 0");
          if (expr->op == "&&") {
            ctx.code_lines.push_back(
                "  br i1 " + lhs_i1 + ", label %" + rhs_label + ", label %" + short_label);
          } else {
            ctx.code_lines.push_back(
                "  br i1 " + lhs_i1 + ", label %" + short_label + ", label %" + rhs_label);
          }

          ctx.code_lines.push_back(rhs_label + ":");
          const std::string rhs = EmitExpr(expr->right.get(), ctx);
          ctx.code_lines.push_back("  br label %" + rhs_done_label);
          ctx.code_lines.push_back(rhs_done_label + ":");
          ctx.code_lines.push_back("  " + rhs_i1 + " = icmp ne i32 " + rhs + ", 0");
          ctx.code_lines.push_back("  br label %" + merge_label);

          ctx.code_lines.push_back(short_label + ":");
          ctx.code_lines.push_back("  br label %" + merge_label);

          ctx.code_lines.push_back(merge_label + ":");
          ctx.code_lines.push_back("  " + logical_i1 + " = phi i1 [" + short_value + ", %" + short_label +
                                   "], [" + rhs_i1 + ", %" + rhs_done_label + "]");
          ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + logical_i1 + " to i32");
          return out_i32;
        }

        const std::string lhs = EmitExpr(expr->left.get(), ctx);
        const std::string rhs = EmitExpr(expr->right.get(), ctx);
        if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
          const std::string tmp = NewTemp(ctx);
          std::string op = "add";
          if (expr->op == "+") {
            op = "add";
          } else if (expr->op == "-") {
            op = "sub";
          } else if (expr->op == "*") {
            op = "mul";
          } else if (expr->op == "/") {
            op = "sdiv";
          } else if (expr->op == "%") {
            op = "srem";
          }
          ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs + ", " + rhs);
          return tmp;
        }

        if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
          const std::string tmp = NewTemp(ctx);
          std::string op = "and";
          if (expr->op == "&") {
            op = "and";
          } else if (expr->op == "|") {
            op = "or";
          } else if (expr->op == "^") {
            op = "xor";
          } else if (expr->op == "<<") {
            op = "shl";
          } else if (expr->op == ">>") {
            op = "ashr";
          }
          ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs + ", " + rhs);
          return tmp;
        }

        std::string pred;
        if (expr->op == "==") {
          pred = "eq";
        } else if (expr->op == "!=") {
          pred = "ne";
        } else if (expr->op == "<") {
          pred = "slt";
        } else if (expr->op == "<=") {
          pred = "sle";
        } else if (expr->op == ">") {
          pred = "sgt";
        } else if (expr->op == ">=") {
          pred = "sge";
        } else {
          return "0";
        }
        const std::string cmp_i1 = NewTemp(ctx);
        const std::string out_i32 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cmp_i1 + " = icmp " + pred + " i32 " + lhs + ", " + rhs);
        ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + cmp_i1 + " to i32");
        return out_i32;
      }
      case Expr::Kind::Conditional: {
        const std::string cond_value = EmitExpr(expr->left.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string true_label = NewLabel(ctx, "cond_true_");
        const std::string false_label = NewLabel(ctx, "cond_false_");
        const std::string merge_label = NewLabel(ctx, "cond_merge_");
        const std::string result_ptr = "%cond.addr." + std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + result_ptr + " = alloca i32, align 4");
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond_value + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + true_label + ", label %" + false_label);

        ctx.code_lines.push_back(true_label + ":");
        const std::string true_value = EmitExpr(expr->right.get(), ctx);
        ctx.code_lines.push_back("  store i32 " + true_value + ", ptr " + result_ptr + ", align 4");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(false_label + ":");
        const std::string false_value = EmitExpr(expr->third.get(), ctx);
        ctx.code_lines.push_back("  store i32 " + false_value + ", ptr " + result_ptr + ", align 4");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(merge_label + ":");
        const std::string out_value = NewTemp(ctx);
        ctx.code_lines.push_back("  " + out_value + " = load i32, ptr " + result_ptr + ", align 4");
        return out_value;
      }
      case Expr::Kind::Call: {
        const LoweredFunctionSignature *signature = LookupFunctionSignature(expr->ident);
        std::vector<std::string> args;
        args.reserve(expr->args.size());
        for (std::size_t i = 0; i < expr->args.size(); ++i) {
          const std::string arg_i32 = EmitExpr(expr->args[i].get(), ctx);
          const ValueType expected_type =
              signature != nullptr && i < signature->param_types.size() ? signature->param_types[i] : ValueType::I32;
          AppendLoweredCallArg(args, arg_i32, expected_type, ctx);
        }
        std::ostringstream arglist;
        for (std::size_t i = 0; i < args.size(); ++i) {
          if (i != 0) {
            arglist << ", ";
          }
          arglist << args[i];
        }
        const ValueType return_type = signature != nullptr ? signature->return_type : ValueType::I32;
        const std::string llvm_return_type = LLVMScalarType(return_type);
        const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);
        if (return_type == ValueType::Void) {
          ctx.code_lines.push_back("  call " + llvm_return_type + " @" + expr->ident + "(" + arglist.str() + ")");
          if (call_may_have_global_side_effects) {
            InvalidateGlobalProofState(ctx);
          }
          return "0";
        }
        const std::string tmp = NewTemp(ctx);
        ctx.code_lines.push_back("  " + tmp + " = call " + llvm_return_type + " @" + expr->ident + "(" +
                                 arglist.str() + ")");
        const std::string out = CoerceValueToI32(tmp, return_type, ctx);
        if (call_may_have_global_side_effects) {
          InvalidateGlobalProofState(ctx);
        }
        return out;
      }
      case Expr::Kind::MessageSend: {
        return EmitMessageSendExpr(expr, ctx);
      }
    }
    return "0";
  }

  void EmitStatement(const Stmt *stmt, FunctionContext &ctx) const {
    if (stmt == nullptr || ctx.terminated) {
      return;
    }

    switch (stmt->kind) {
      case Stmt::Kind::Let: {
        const LetStmt *let = stmt->let_stmt.get();
        if (let == nullptr || ctx.scopes.empty()) {
          return;
        }
        // Evaluate the initializer against the currently visible scope first so
        // shadowing declarations can read the previous binding deterministically.
        const std::string value = EmitExpr(let->value.get(), ctx);
        int let_const_value = 0;
        const bool has_let_const_value = TryGetCompileTimeI32ExprInContext(let->value.get(), ctx, let_const_value);
        const bool has_let_nil_value = IsCompileTimeNilReceiverExprInContext(let->value.get(), ctx);
        const std::string ptr = "%" + let->name + ".addr." + std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.scopes.back()[let->name] = ptr;
        if (has_let_nil_value) {
          ctx.nil_bound_ptrs.insert(ptr);
        }
        if (has_let_const_value) {
          ctx.const_value_ptrs[ptr] = let_const_value;
        }
        if (has_let_const_value && let_const_value != 0) {
          ctx.nonzero_bound_ptrs.insert(ptr);
        }
        ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr + ", align 4");
        return;
      }
      case Stmt::Kind::Return: {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (ret == nullptr) {
          return;
        }
        if (ret->value == nullptr) {
          EmitTypedReturn("0", ctx);
        } else {
          const std::string value = EmitExpr(ret->value.get(), ctx);
          EmitTypedReturn(value, ctx);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Assign: {
        const AssignStmt *assign = stmt->assign_stmt.get();
        if (assign == nullptr) {
          return;
        }
        const std::string ptr = LookupVarPtr(ctx, assign->name);
        EmitAssignmentStore(ptr, assign->op, assign->value.get(), ctx);
        return;
      }
      case Stmt::Kind::Break: {
        if (ctx.control_stack.empty()) {
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          ctx.code_lines.push_back("  br label %" + ctx.control_stack.back().break_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Continue: {
        std::string continue_label;
        for (auto it = ctx.control_stack.rbegin(); it != ctx.control_stack.rend(); ++it) {
          if (it->continue_allowed) {
            continue_label = it->continue_label;
            break;
          }
        }
        if (continue_label.empty()) {
          ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
        } else {
          ctx.code_lines.push_back("  br label %" + continue_label);
        }
        ctx.terminated = true;
        return;
      }
      case Stmt::Kind::Empty:
        return;
      case Stmt::Kind::Block: {
        const BlockStmt *block_stmt = stmt->block_stmt.get();
        if (block_stmt == nullptr) {
          return;
        }
        ctx.scopes.push_back({});
        for (const auto &nested_stmt : block_stmt->body) {
          EmitStatement(nested_stmt.get(), ctx);
        }
        ctx.scopes.pop_back();
        return;
      }
      case Stmt::Kind::Expr: {
        const ExprStmt *expr_stmt = stmt->expr_stmt.get();
        if (expr_stmt != nullptr) {
          (void)EmitExpr(expr_stmt->value.get(), ctx);
        }
        return;
      }
      case Stmt::Kind::While: {
        const WhileStmt *while_stmt = stmt->while_stmt.get();
        if (while_stmt == nullptr) {
          return;
        }

        const std::string cond_label = NewLabel(ctx, "while_cond_");
        const std::string body_label = NewLabel(ctx, "while_body_");
        const std::string end_label = NewLabel(ctx, "while_end_");
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(cond_label + ":");
        const std::string cond = EmitExpr(while_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);

        ctx.code_lines.push_back(body_label + ":");
        ctx.scopes.push_back({});
        ctx.control_stack.push_back({cond_label, end_label, true});
        ctx.terminated = false;
        for (const auto &s : while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        ctx.scopes.pop_back();
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + cond_label);
        }
        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::DoWhile: {
        const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
        if (do_while_stmt == nullptr) {
          return;
        }

        const std::string body_label = NewLabel(ctx, "do_body_");
        const std::string cond_label = NewLabel(ctx, "do_cond_");
        const std::string end_label = NewLabel(ctx, "do_end_");
        ctx.code_lines.push_back("  br label %" + body_label);

        ctx.code_lines.push_back(body_label + ":");
        ctx.scopes.push_back({});
        ctx.control_stack.push_back({cond_label, end_label, true});
        ctx.terminated = false;
        for (const auto &s : do_while_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        ctx.scopes.pop_back();
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + cond_label);
        }

        ctx.code_lines.push_back(cond_label + ":");
        const std::string cond = EmitExpr(do_while_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::For: {
        const ForStmt *for_stmt = stmt->for_stmt.get();
        if (for_stmt == nullptr) {
          return;
        }

        ctx.scopes.push_back({});
        EmitForClause(for_stmt->init, ctx);

        const std::string cond_label = NewLabel(ctx, "for_cond_");
        const std::string body_label = NewLabel(ctx, "for_body_");
        const std::string step_label = NewLabel(ctx, "for_step_");
        const std::string end_label = NewLabel(ctx, "for_end_");

        ctx.code_lines.push_back("  br label %" + cond_label);
        ctx.code_lines.push_back(cond_label + ":");
        if (for_stmt->condition == nullptr) {
          ctx.code_lines.push_back("  br label %" + body_label);
        } else {
          const std::string cond = EmitExpr(for_stmt->condition.get(), ctx);
          const std::string cond_i1 = NewTemp(ctx);
          ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
          ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + body_label + ", label %" + end_label);
        }

        ctx.code_lines.push_back(body_label + ":");
        ctx.scopes.push_back({});
        ctx.control_stack.push_back({step_label, end_label, true});
        ctx.terminated = false;
        for (const auto &s : for_stmt->body) {
          EmitStatement(s.get(), ctx);
        }
        const bool body_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        ctx.scopes.pop_back();
        if (!body_terminated) {
          ctx.code_lines.push_back("  br label %" + step_label);
        }

        ctx.code_lines.push_back(step_label + ":");
        EmitForClause(for_stmt->step, ctx);
        ctx.code_lines.push_back("  br label %" + cond_label);

        ctx.code_lines.push_back(end_label + ":");
        ctx.scopes.pop_back();
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::Switch: {
        const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
        if (switch_stmt == nullptr) {
          return;
        }

        const std::string condition_value = EmitExpr(switch_stmt->condition.get(), ctx);
        const std::string end_label = NewLabel(ctx, "switch_end_");

        std::vector<std::string> arm_labels;
        arm_labels.reserve(switch_stmt->cases.size());
        std::vector<std::size_t> case_clause_indices;
        case_clause_indices.reserve(switch_stmt->cases.size());
        std::size_t default_index = switch_stmt->cases.size();

        for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
          const SwitchCase &case_stmt = switch_stmt->cases[i];
          if (case_stmt.is_default) {
            arm_labels.push_back(NewLabel(ctx, "switch_default_"));
            if (default_index == switch_stmt->cases.size()) {
              default_index = i;
            }
          } else {
            arm_labels.push_back(NewLabel(ctx, "switch_case_"));
            case_clause_indices.push_back(i);
          }
        }

        const std::string default_label =
            default_index < switch_stmt->cases.size() ? arm_labels[default_index] : end_label;

        if (!case_clause_indices.empty()) {
          std::vector<std::string> test_labels;
          test_labels.reserve(case_clause_indices.size());
          for (std::size_t i = 0; i < case_clause_indices.size(); ++i) {
            test_labels.push_back(NewLabel(ctx, "switch_test_"));
          }

          ctx.code_lines.push_back("  br label %" + test_labels[0]);
          for (std::size_t test_index = 0; test_index < case_clause_indices.size(); ++test_index) {
            const std::size_t case_index = case_clause_indices[test_index];
            const std::string next_label =
                (test_index + 1 < case_clause_indices.size()) ? test_labels[test_index + 1] : default_label;

            ctx.code_lines.push_back(test_labels[test_index] + ":");
            const std::string cmp = NewTemp(ctx);
            ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " + condition_value + ", " +
                                     std::to_string(switch_stmt->cases[case_index].value));
            ctx.code_lines.push_back("  br i1 " + cmp + ", label %" + arm_labels[case_index] + ", label %" +
                                     next_label);
          }
        } else {
          ctx.code_lines.push_back("  br label %" + default_label);
        }

        for (std::size_t arm_index = 0; arm_index < switch_stmt->cases.size(); ++arm_index) {
          const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
          ctx.code_lines.push_back(arm_labels[arm_index] + ":");
          ctx.scopes.push_back({});
          ctx.control_stack.push_back({"", end_label, false});
          ctx.terminated = false;
          for (const auto &case_body_stmt : case_stmt.body) {
            EmitStatement(case_body_stmt.get(), ctx);
          }
          const bool arm_terminated = ctx.terminated;
          ctx.control_stack.pop_back();
          ctx.scopes.pop_back();

          if (!arm_terminated) {
            if (arm_index + 1 < switch_stmt->cases.size()) {
              ctx.code_lines.push_back("  br label %" + arm_labels[arm_index + 1]);
            } else {
              ctx.code_lines.push_back("  br label %" + end_label);
            }
          }
        }

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }
      case Stmt::Kind::If: {
        const IfStmt *if_stmt = stmt->if_stmt.get();
        if (if_stmt == nullptr) {
          return;
        }

        const std::string cond = EmitExpr(if_stmt->condition.get(), ctx);
        const std::string cond_i1 = NewTemp(ctx);
        const std::string then_label = NewLabel(ctx, "if_then_");
        const std::string else_label = NewLabel(ctx, "if_else_");
        const std::string merge_label = NewLabel(ctx, "if_end_");

        ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond + ", 0");
        ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" + then_label + ", label %" + else_label);

        ctx.code_lines.push_back(then_label + ":");
        ctx.scopes.push_back({});
        ctx.terminated = false;
        for (const auto &s : if_stmt->then_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool then_terminated = ctx.terminated;
        ctx.scopes.pop_back();
        if (!then_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        ctx.code_lines.push_back(else_label + ":");
        ctx.scopes.push_back({});
        ctx.terminated = false;
        for (const auto &s : if_stmt->else_body) {
          EmitStatement(s.get(), ctx);
        }
        const bool else_terminated = ctx.terminated;
        ctx.scopes.pop_back();
        if (!else_terminated) {
          ctx.code_lines.push_back("  br label %" + merge_label);
        }

        if (then_terminated && else_terminated) {
          ctx.terminated = true;
        } else {
          ctx.code_lines.push_back(merge_label + ":");
          ctx.terminated = false;
        }
        return;
      }
    }
  }

  void InvalidateGlobalProofState(FunctionContext &ctx) const {
    ctx.global_proofs_invalidated = true;
    for (auto it = ctx.nil_bound_ptrs.begin(); it != ctx.nil_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nil_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.nonzero_bound_ptrs.begin(); it != ctx.nonzero_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nonzero_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.const_value_ptrs.begin(); it != ctx.const_value_ptrs.end();) {
      if (it->first.rfind("@", 0) == 0) {
        it = ctx.const_value_ptrs.erase(it);
      } else {
        ++it;
      }
    }
  }

  void EmitPrototypeDeclarations(std::ostringstream &out) const {
    bool emitted = false;
    for (const auto &entry : function_signatures_) {
      if (defined_functions_.find(entry.first) != defined_functions_.end()) {
        continue;
      }
      const LoweredFunctionSignature &signature = entry.second;
      std::ostringstream params;
      for (std::size_t i = 0; i < signature.param_types.size(); ++i) {
        if (i != 0) {
          params << ", ";
        }
        params << LLVMScalarType(signature.param_types[i]);
      }
      out << "declare " << LLVMScalarType(signature.return_type) << " @" << entry.first << "(" << params.str()
          << ")\n";
      emitted = true;
    }
    if (emitted) {
      out << "\n";
    }
  }

  void EmitFunction(const FunctionDecl &fn, std::ostringstream &out) const {
    std::ostringstream signature;
    for (std::size_t i = 0; i < fn.params.size(); ++i) {
      if (i != 0) {
        signature << ", ";
      }
      signature << LLVMScalarType(fn.params[i].type) << " %arg" << i;
    }

    out << "define " << LLVMScalarType(fn.return_type) << " @" << fn.name << "(" << signature.str() << ") {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = fn.return_type;
    ctx.scopes.push_back({});

    for (std::size_t i = 0; i < fn.params.size(); ++i) {
      const auto &param = fn.params[i];
      const std::string ptr = "%" + param.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      EmitTypedParamStore(param, i, ptr, ctx);
      ctx.scopes.back()[param.name] = ptr;
    }

    for (const auto &stmt : fn.body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      if (fn.return_type == ValueType::Void) {
        ctx.code_lines.push_back("  ret void");
      } else {
        ctx.code_lines.push_back("  ret " + std::string(LLVMScalarType(fn.return_type)) + " 0");
      }
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }

    out << "}\n";
  }

  void EmitEntryPoint(std::ostringstream &out) const {
    out << "define i32 @objc3c_entry() {\n";
    out << "entry:\n";

    auto main_it = function_arity_.find("main");
    if (main_it != function_arity_.end() && main_it->second == 0) {
      ValueType main_return_type = ValueType::I32;
      const LoweredFunctionSignature *main_signature = LookupFunctionSignature("main");
      if (main_signature != nullptr) {
        main_return_type = main_signature->return_type;
      }
      if (main_return_type == ValueType::Void) {
        out << "  call void @main()\n";
        out << "  ret i32 0\n";
      } else {
        out << "  %call_main = call " << LLVMScalarType(main_return_type) << " @main()\n";
        if (main_return_type == ValueType::Bool) {
          out << "  %call_main_i32 = zext i1 %call_main to i32\n";
          out << "  ret i32 %call_main_i32\n";
        } else {
          out << "  ret i32 %call_main\n";
        }
      }
      out << "}\n";
      return;
    }

    std::string previous = "0";
    for (std::size_t i = 0; i < program_.globals.size(); ++i) {
      const auto &global = program_.globals[i];
      const std::string load_name = "%entry_load_" + std::to_string(i);
      const std::string sum_name = "%entry_sum_" + std::to_string(i);
      out << "  " << load_name << " = load i32, ptr @" << global.name << ", align 4\n";
      out << "  " << sum_name << " = add i32 " << previous << ", " << load_name << "\n";
      previous = sum_name;
    }
    out << "  ret i32 " << previous << "\n";
    out << "}\n";
  }

  const Objc3Program &program_;
  Objc3LoweringContract lowering_contract_;
  std::unordered_set<std::string> globals_;
  std::unordered_set<std::string> mutable_global_symbols_;
  std::unordered_map<std::string, int> global_const_values_;
  std::unordered_set<std::string> global_nil_proven_symbols_;
  std::unordered_set<std::string> defined_functions_;
  std::unordered_set<std::string> declared_pure_functions_;
  std::vector<const FunctionDecl *> function_definitions_;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects_;
  std::unordered_set<std::string> impure_functions_;
  std::unordered_map<std::string, std::size_t> function_arity_;
  std::map<std::string, LoweredFunctionSignature> function_signatures_;
  std::map<std::string, std::string> selector_globals_;
  mutable bool runtime_dispatch_call_emitted_ = false;
};

static CXChildVisitResult VisitSymbol(CXCursor cursor, CXCursor, CXClientData client_data) {
  auto *ctx = static_cast<SymbolContext *>(client_data);
  CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind == CXCursor_FunctionDecl || kind == CXCursor_VarDecl || kind == CXCursor_ObjCInterfaceDecl ||
      kind == CXCursor_ObjCInstanceMethodDecl || kind == CXCursor_ObjCClassMethodDecl) {
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned line = 0;
    unsigned column = 0;
    unsigned offset = 0;
    clang_getFileLocation(location, &file, &line, &column, &offset);
    (void)offset;

    SymbolRow row{
        ToString(clang_getCursorKindSpelling(kind)),
        ToString(clang_getCursorSpelling(cursor)),
        line,
        column,
    };
    ctx->rows.push_back(std::move(row));
  }

  return CXChildVisit_Recurse;
}

static std::string FormatDiagnostic(CXDiagnostic diagnostic) {
  CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
  std::string severity_text;
  switch (severity) {
    case CXDiagnostic_Ignored:
      severity_text = "ignored";
      break;
    case CXDiagnostic_Note:
      severity_text = "note";
      break;
    case CXDiagnostic_Warning:
      severity_text = "warning";
      break;
    case CXDiagnostic_Error:
      severity_text = "error";
      break;
    case CXDiagnostic_Fatal:
      severity_text = "fatal";
      break;
    default:
      severity_text = "unknown";
      break;
  }

  CXSourceLocation location = clang_getDiagnosticLocation(diagnostic);
  CXFile file;
  unsigned line = 0;
  unsigned column = 0;
  unsigned offset = 0;
  clang_getFileLocation(location, &file, &line, &column, &offset);
  (void)offset;

  std::ostringstream oss;
  oss << severity_text << ":" << line << ":" << column << ": "
      << ToString(clang_getDiagnosticSpelling(diagnostic));
  return oss.str();
}

static void WriteText(const fs::path &path, const std::string &contents) {
  fs::create_directories(path.parent_path());
  std::ofstream out(path, std::ios::binary);
  out << contents;
}

static std::string ReadText(const fs::path &path) {
  std::ifstream in(path, std::ios::binary);
  std::ostringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

static std::string JoinLines(const std::vector<std::string> &lines) {
  std::ostringstream out;
  for (const auto &line : lines) {
    out << line << "\n";
  }
  return out.str();
}

static std::string EscapeJsonString(const std::string &value) {
  std::ostringstream out;
  for (unsigned char c : value) {
    switch (c) {
      case '"':
        out << "\\\"";
        break;
      case '\\':
        out << "\\\\";
        break;
      case '\b':
        out << "\\b";
        break;
      case '\f':
        out << "\\f";
        break;
      case '\n':
        out << "\\n";
        break;
      case '\r':
        out << "\\r";
        break;
      case '\t':
        out << "\\t";
        break;
      default:
        if (c < 0x20) {
          std::ostringstream code;
          code << std::hex << std::uppercase << static_cast<int>(c);
          std::string hex = code.str();
          while (hex.size() < 4) {
            hex = "0" + hex;
          }
          out << "\\u" << hex;
        } else {
          out << static_cast<char>(c);
        }
        break;
    }
  }
  return out.str();
}

static void WriteDiagnosticsTextArtifact(const fs::path &out_dir, const std::string &emit_prefix,
                                         const std::vector<std::string> &diagnostics) {
  WriteText(out_dir / (emit_prefix + ".diagnostics.txt"), JoinLines(diagnostics));
}

static void WriteDiagnosticsJsonArtifact(const fs::path &out_dir, const std::string &emit_prefix,
                                         const std::vector<std::string> &diagnostics) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"schema_version\": \"1.0.0\",\n";
  out << "  \"diagnostics\": [\n";
  for (std::size_t i = 0; i < diagnostics.size(); ++i) {
    const DiagSortKey key = ParseDiagSortKey(diagnostics[i]);
    const unsigned line = key.line == std::numeric_limits<unsigned>::max() ? 0U : key.line;
    const unsigned column = key.column == std::numeric_limits<unsigned>::max() ? 0U : key.column;
    out << "    {\"severity\":\"" << EscapeJsonString(ToLower(key.severity)) << "\",\"line\":" << line
        << ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(key.code) << "\",\"message\":\""
        << EscapeJsonString(key.message) << "\",\"raw\":\"" << EscapeJsonString(diagnostics[i]) << "\"}";
    if (i + 1 != diagnostics.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "  ]\n";
  out << "}\n";
  WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());
}

static void WriteDiagnosticsArtifacts(const fs::path &out_dir, const std::string &emit_prefix,
                                      const std::vector<std::string> &diagnostics) {
  WriteDiagnosticsTextArtifact(out_dir, emit_prefix, diagnostics);
  WriteDiagnosticsJsonArtifact(out_dir, emit_prefix, diagnostics);
}

static int RunProcess(const std::string &executable, const std::vector<std::string> &args) {
  std::vector<const char *> argv;
  argv.reserve(args.size() + 2);
  argv.push_back(executable.c_str());
  for (const auto &arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  const int status = _spawnvp(_P_WAIT, executable.c_str(), argv.data());
  if (status == -1) {
    return 127;
  }
  return status;
}

static int RunObjectiveCCompile(const fs::path &clang_path, const fs::path &input, const fs::path &object_out) {
  const std::string clang_exe = clang_path.string();
  const int syntax_status =
      RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-fsyntax-only", input.string()});
  if (syntax_status != 0) {
    return syntax_status;
  }

  return RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-c", input.string(), "-o",
                                object_out.string(), "-fno-color-diagnostics"});
}

static int RunIRCompile(const fs::path &clang_path, const fs::path &ir_path, const fs::path &object_out) {
  const std::string clang_exe = clang_path.string();
  return RunProcess(clang_exe, {"-x", "ir", "-c", ir_path.string(), "-o", object_out.string(),
                                "-fno-color-diagnostics"});
}

static bool EmitObjc3IR(const Objc3Program &program, const Objc3LoweringContract &lowering_contract,
                        const fs::path &output_ir, std::string &error) {
  Objc3IREmitter emitter(program, lowering_contract);
  std::string ir;
  if (!emitter.Emit(ir, error)) {
    return false;
  }
  WriteText(output_ir, ir);
  return true;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "usage: objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] "
                 "[--objc3-max-message-args <0-"
              << kObjc3RuntimeDispatchMaxArgs << ">] [--objc3-runtime-dispatch-symbol <symbol>]\n";
    return 2;
  }

  fs::path input = argv[1];
  fs::path out_dir = fs::path("artifacts") / "compilation" / "objc3c-native";
  std::string emit_prefix = "module";
  fs::path clang_path = fs::path("clang");
  Objc3FrontendOptions frontend_options;

  for (int i = 2; i < argc; ++i) {
    std::string flag = argv[i];
    if (flag == "--out-dir" && i + 1 < argc) {
      out_dir = argv[++i];
    } else if (flag == "--emit-prefix" && i + 1 < argc) {
      emit_prefix = argv[++i];
    } else if (flag == "--clang" && i + 1 < argc) {
      clang_path = argv[++i];
    } else if (flag == "--objc3-max-message-args" && i + 1 < argc) {
      const std::string value = argv[++i];
      errno = 0;
      char *end = nullptr;
      const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
      if (value.empty() || end == value.c_str() || *end != '\0' || errno == ERANGE ||
          parsed > kObjc3RuntimeDispatchMaxArgs) {
        std::cerr << "invalid --objc3-max-message-args (expected integer 0-"
                  << kObjc3RuntimeDispatchMaxArgs << "): " << value << "\n";
        return 2;
      }
      frontend_options.lowering.max_message_send_args = static_cast<std::size_t>(parsed);
    } else if (flag == "--objc3-runtime-dispatch-symbol" && i + 1 < argc) {
      const std::string symbol = argv[++i];
      if (!IsValidRuntimeDispatchSymbol(symbol)) {
        std::cerr << "invalid --objc3-runtime-dispatch-symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " << symbol
                  << "\n";
        return 2;
      }
      frontend_options.lowering.runtime_dispatch_symbol = symbol;
    } else {
      std::cerr << "unknown arg: " << flag << "\n";
      return 2;
    }
  }

  if (!fs::exists(input)) {
    std::cerr << "input file not found: " << input.string() << "\n";
    return 2;
  }
  if (clang_path.has_root_path() && !fs::exists(clang_path)) {
    std::cerr << "clang executable not found: " << clang_path.string() << "\n";
    return 2;
  }

  const std::string extension = ToLower(input.extension().string());
  if (extension == ".objc3") {
    const std::string source = ReadText(input);
    Objc3FrontendPipelineResult frontend_pipeline = RunObjc3FrontendPipeline(source, frontend_options);
    Objc3Program &program = frontend_pipeline.program;
    WriteDiagnosticsArtifacts(out_dir, emit_prefix, program.diagnostics);
    if (!program.diagnostics.empty()) {
      return 1;
    }

    std::vector<const FunctionDecl *> manifest_functions;
    manifest_functions.reserve(program.functions.size());
    std::unordered_set<std::string> manifest_function_names;
    for (const auto &fn : program.functions) {
      if (manifest_function_names.insert(fn.name).second) {
        manifest_functions.push_back(&fn);
      }
    }

    std::size_t scalar_return_i32 = 0;
    std::size_t scalar_return_bool = 0;
    std::size_t scalar_return_void = 0;
    std::size_t scalar_param_i32 = 0;
    std::size_t scalar_param_bool = 0;
    for (const auto &entry : frontend_pipeline.integration_surface.functions) {
      const FunctionInfo &signature = entry.second;
      if (signature.return_type == ValueType::Bool) {
        ++scalar_return_bool;
      } else if (signature.return_type == ValueType::Void) {
        ++scalar_return_void;
      } else {
        ++scalar_return_i32;
      }
      for (const ValueType param_type : signature.param_types) {
        if (param_type == ValueType::Bool) {
          ++scalar_param_bool;
        } else {
          ++scalar_param_i32;
        }
      }
    }

    std::ostringstream manifest;
    manifest << "{\n";
    manifest << "  \"source\": \"" << input.generic_string() << "\",\n";
    manifest << "  \"module\": \"" << program.module_name << "\",\n";
    manifest << "  \"frontend\": {\n";
    manifest << "    \"max_message_send_args\":" << frontend_options.lowering.max_message_send_args << ",\n";
    manifest << "    \"pipeline\": {\n";
    manifest << "      \"semantic_skipped\": " << (frontend_pipeline.integration_surface.built ? "false" : "true")
             << ",\n";
    manifest << "      \"stages\": {\n";
    manifest << "        \"lexer\": {\"diagnostics\":" << frontend_pipeline.stage_diagnostics.lexer.size()
             << "},\n";
    manifest << "        \"parser\": {\"diagnostics\":" << frontend_pipeline.stage_diagnostics.parser.size()
             << "},\n";
    manifest << "        \"semantic\": {\"diagnostics\":" << frontend_pipeline.stage_diagnostics.semantic.size()
             << "}\n";
    manifest << "      },\n";
    manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()
             << ",\"declared_functions\":" << manifest_functions.size()
             << ",\"resolved_global_symbols\":" << frontend_pipeline.integration_surface.globals.size()
             << ",\"resolved_function_symbols\":" << frontend_pipeline.integration_surface.functions.size()
             << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
             << ",\"scalar_return_bool\":" << scalar_return_bool
             << ",\"scalar_return_void\":" << scalar_return_void
             << ",\"scalar_param_i32\":" << scalar_param_i32
             << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
    manifest << "    }\n";
    manifest << "  },\n";
    manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << frontend_options.lowering.runtime_dispatch_symbol
             << "\",\"runtime_dispatch_arg_slots\":" << frontend_options.lowering.max_message_send_args
             << ",\"selector_global_ordering\":\"lexicographic\"},\n";
    std::vector<int> resolved_global_values;
    if (!ResolveGlobalInitializerValues(program.globals, resolved_global_values) ||
        resolved_global_values.size() != program.globals.size()) {
      const std::vector<std::string> ir_diags = {
          MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")};
      WriteDiagnosticsArtifacts(out_dir, emit_prefix, ir_diags);
      return 1;
    }

    manifest << "  \"globals\": [\n";
    for (std::size_t i = 0; i < program.globals.size(); ++i) {
      manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]
               << ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column
               << "}";
      if (i + 1 != program.globals.size()) {
        manifest << ",";
      }
      manifest << "\n";
    }
    manifest << "  ],\n";
    manifest << "  \"functions\": [\n";
    for (std::size_t i = 0; i < manifest_functions.size(); ++i) {
      const auto &fn = *manifest_functions[i];
      manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size()
               << ",\"param_types\":[";
      for (std::size_t p = 0; p < fn.params.size(); ++p) {
        manifest << "\"" << TypeName(fn.params[p].type) << "\"";
        if (p + 1 != fn.params.size()) {
          manifest << ",";
        }
      }
      manifest << "]"
               << ",\"return\":\"" << TypeName(fn.return_type) << "\""
               << ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";
      if (i + 1 != manifest_functions.size()) {
        manifest << ",";
      }
      manifest << "\n";
    }
    manifest << "  ]\n";
    manifest << "}\n";
    WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest.str());

    const fs::path ir_out = out_dir / (emit_prefix + ".ll");
    std::string ir_error;
    if (!EmitObjc3IR(program, frontend_options.lowering, ir_out, ir_error)) {
      const std::vector<std::string> ir_diags = {
          MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
      WriteDiagnosticsArtifacts(out_dir, emit_prefix, ir_diags);
      return 1;
    }

    const fs::path object_out = out_dir / (emit_prefix + ".obj");
    const int compile_status = RunIRCompile(clang_path, ir_out, object_out);
    return compile_status == 0 ? 0 : 3;
  }

  const std::vector<const char *> parse_args = {"-x", "objective-c", "-std=gnu11"};
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit tu = clang_parseTranslationUnit(index, input.string().c_str(), parse_args.data(),
                                                    static_cast<int>(parse_args.size()), nullptr, 0,
                                                    CXTranslationUnit_None);

  std::vector<std::string> diagnostics;
  bool has_errors = false;
  if (tu != nullptr) {
    const unsigned count = clang_getNumDiagnostics(tu);
    diagnostics.reserve(count);
    for (unsigned i = 0; i < count; ++i) {
      CXDiagnostic diagnostic = clang_getDiagnostic(tu, i);
      diagnostics.push_back(FormatDiagnostic(diagnostic));
      CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
      if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal) {
        has_errors = true;
      }
      clang_disposeDiagnostic(diagnostic);
    }
  } else {
    diagnostics.push_back("fatal:0:0: unable to parse translation unit");
    has_errors = true;
  }

  NormalizeDiagnostics(diagnostics);
  WriteDiagnosticsArtifacts(out_dir, emit_prefix, diagnostics);

  if (has_errors || tu == nullptr) {
    if (tu != nullptr) {
      clang_disposeTranslationUnit(tu);
    }
    clang_disposeIndex(index);
    return 1;
  }

  SymbolContext context;
  clang_visitChildren(clang_getTranslationUnitCursor(tu), VisitSymbol, &context);
  std::sort(context.rows.begin(), context.rows.end(), [](const SymbolRow &a, const SymbolRow &b) {
    if (a.line != b.line) {
      return a.line < b.line;
    }
    if (a.column != b.column) {
      return a.column < b.column;
    }
    if (a.kind != b.kind) {
      return a.kind < b.kind;
    }
    return a.name < b.name;
  });

  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input.generic_string() << "\",\n";
  manifest << "  \"symbols\": [\n";
  for (std::size_t i = 0; i < context.rows.size(); ++i) {
    const SymbolRow &row = context.rows[i];
    manifest << "    {\"kind\":\"" << row.kind << "\",\"name\":\"" << row.name << "\",\"line\":"
             << row.line << ",\"column\":" << row.column << "}";
    if (i + 1 != context.rows.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ]\n";
  manifest << "}\n";
  WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest.str());

  const fs::path object_out = out_dir / (emit_prefix + ".obj");
  const int compile_status = RunObjectiveCCompile(clang_path, input, object_out);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);

  return compile_status == 0 ? 0 : 3;
}
