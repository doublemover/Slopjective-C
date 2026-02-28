#include "lex/objc3_lexer.h"

#include <cctype>
#include <sstream>
#include <string>

namespace {

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

bool IsIdentStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsIdentBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsHexDigit(char c) {
  return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

bool IsBinaryDigit(char c) {
  return c == '0' || c == '1';
}

bool IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

bool IsDigitSeparator(char c) {
  return c == '_';
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

}  // namespace

Objc3Lexer::Objc3Lexer(const std::string &source) : source_(source) {}

std::vector<Objc3LexToken> Objc3Lexer::Run(std::vector<std::string> &diagnostics) {
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
          diagnostics.push_back(MakeDiag(token_line, token_column, "O3L004", "stray block comment terminator"));
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
        diagnostics.push_back(
            MakeDiag(token_line, token_column, "O3L001", std::string("unexpected character '") + c + "'"));
        break;
    }
  }
  return tokens;
}

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
  while (index_ < source_.size() && IsIdentBody(source_[index_])) {
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
