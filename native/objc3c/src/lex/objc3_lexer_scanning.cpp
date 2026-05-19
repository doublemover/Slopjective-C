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
