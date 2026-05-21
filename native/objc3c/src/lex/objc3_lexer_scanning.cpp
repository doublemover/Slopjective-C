#include "lex/objc3_lexer.h"

#include <cctype>

#include "diag/objc3_diag_utils.h"
#include "lex/objc3_lexer_char_class.h"
#include "support/objc3_ascii_predicates.h"

namespace {

using objc3c::support::IsBinaryDigit;
using objc3c::support::IsDigitSeparator;
using objc3c::support::IsHexDigit;
using objc3c::support::IsOctalDigit;

}  // namespace

void Objc3Lexer::SkipTrivia(std::vector<std::string> &diagnostics) {
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
          diagnostics.push_back(MakeDiag(line_, column_, "O3L003", "nested block comments are unsupported"));
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
        diagnostics.push_back(MakeDiag(comment_line, comment_column, "O3L002", "unterminated block comment"));
        index_ = source_.size();
        return;
      }
      continue;
    }
    break;
  }
}

std::string Objc3Lexer::ConsumeIdentifier() {
  const std::size_t begin = index_;
  Advance();
  while (index_ < source_.size() && IsObjc3IdentifierBody(source_[index_])) {
    Advance();
  }
  return source_.substr(begin, index_ - begin);
}

std::string Objc3Lexer::ConsumeNumber() {
  const std::size_t begin = index_;
  if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
      (source_[index_ + 1] == 'b' || source_[index_ + 1] == 'B')) {
    Advance();
    Advance();
    while (index_ < source_.size() && (IsBinaryDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }
  if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
      (source_[index_ + 1] == 'o' || source_[index_ + 1] == 'O')) {
    Advance();
    Advance();
    while (index_ < source_.size() && (IsOctalDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }
  if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
      (source_[index_ + 1] == 'x' || source_[index_ + 1] == 'X')) {
    Advance();
    Advance();
    while (index_ < source_.size() && (IsHexDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }
  while (index_ < source_.size() &&
         (std::isdigit(static_cast<unsigned char>(source_[index_])) != 0 || IsDigitSeparator(source_[index_]))) {
    Advance();
  }
  return source_.substr(begin, index_ - begin);
}

bool Objc3Lexer::ConsumeStringLiteral(
    std::string &token_text, std::vector<std::string> &diagnostics) {
  token_text.clear();
  const unsigned token_line = line_;
  const unsigned token_column = column_;
  Advance();

  std::string value;
  bool terminated = false;
  bool invalid = false;
  while (index_ < source_.size()) {
    const char current = source_[index_];
    if (current == '"') {
      Advance();
      terminated = true;
      break;
    }
    if (current == '\n') {
      break;
    }
    if (current == '$' && index_ + 1u < source_.size() &&
        source_[index_ + 1u] == '{') {
      diagnostics.push_back(MakeDiag(
          line_, column_, "O3L011",
          "string interpolation is unsupported; build text values with explicit objc3.text runtime helpers"));
      invalid = true;
      Advance();
      continue;
    }
    if (current == '\\') {
      const unsigned escape_line = line_;
      const unsigned escape_column = column_;
      Advance();
      if (index_ >= source_.size() || source_[index_] == '\n') {
        diagnostics.push_back(MakeDiag(
            token_line, token_column, "O3L005",
            "unterminated string literal"));
        return false;
      }
      const char escaped = source_[index_];
      switch (escaped) {
        case 'n':
          value.push_back('\n');
          break;
        case 'r':
          value.push_back('\r');
          break;
        case 't':
          value.push_back('\t');
          break;
        case '"':
          value.push_back('"');
          break;
        case '\\':
          value.push_back('\\');
          break;
        default:
          diagnostics.push_back(MakeDiag(
              escape_line, escape_column, "O3L010",
              std::string("invalid string escape '\\") + escaped +
                  "'; supported escapes are \\n, \\r, \\t, \\\", and \\\\"));
          invalid = true;
          break;
      }
      Advance();
      continue;
    }
    value.push_back(current);
    Advance();
  }

  if (!terminated) {
    diagnostics.push_back(MakeDiag(
        token_line, token_column, "O3L005", "unterminated string literal"));
    return false;
  }
  int unit_count = 0;
  if (!TryCountObjc3Utf8Scalars(value, unit_count)) {
    diagnostics.push_back(MakeDiag(
        token_line, token_column, "O3L012",
        "malformed UTF-8 in string literal"));
    return false;
  }
  if (invalid) {
    return false;
  }
  token_text = EscapeObjc3StringTokenText(value);
  return true;
}

void Objc3Lexer::Advance() {
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

bool Objc3Lexer::MatchChar(char expected) {
  if (index_ >= source_.size() || source_[index_] != expected) {
    return false;
  }
  Advance();
  return true;
}
