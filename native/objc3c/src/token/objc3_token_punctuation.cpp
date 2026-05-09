#include "token/objc3_token_punctuation.h"

namespace {

bool HasCharAt(const std::string &source, std::size_t index, char expected) {
  return index < source.size() && source[index] == expected;
}

Objc3PunctuationTokenClassification Token(
    Objc3LexTokenKind kind,
    const char *text,
    std::size_t width) {
  return Objc3PunctuationTokenClassification{true, kind, text, width, false};
}

Objc3PunctuationTokenClassification StrayBlockCommentTerminator() {
  Objc3PunctuationTokenClassification result;
  result.recognized = true;
  result.width = 2u;
  result.stray_block_comment_terminator = true;
  return result;
}

}  // namespace

Objc3PunctuationTokenClassification ClassifyObjc3PunctuationToken(
    const std::string &source,
    std::size_t index) {
  if (index >= source.size()) {
    return Objc3PunctuationTokenClassification{};
  }

  switch (source[index]) {
  case '(':
    return Token(Objc3LexTokenKind::LParen, "(", 1u);
  case ')':
    return Token(Objc3LexTokenKind::RParen, ")", 1u);
  case '[':
    return Token(Objc3LexTokenKind::LBracket, "[", 1u);
  case ']':
    return Token(Objc3LexTokenKind::RBracket, "]", 1u);
  case '{':
    return Token(Objc3LexTokenKind::LBrace, "{", 1u);
  case '}':
    return Token(Objc3LexTokenKind::RBrace, "}", 1u);
  case ',':
    return Token(Objc3LexTokenKind::Comma, ",", 1u);
  case ':':
    return Token(Objc3LexTokenKind::Colon, ":", 1u);
  case '.':
    return Token(Objc3LexTokenKind::Dot, ".", 1u);
  case ';':
    return Token(Objc3LexTokenKind::Semicolon, ";", 1u);
  case '=':
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::EqualEqual, "==", 2u);
    }
    return Token(Objc3LexTokenKind::Equal, "=", 1u);
  case '!':
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::BangEqual, "!=", 2u);
    }
    return Token(Objc3LexTokenKind::Bang, "!", 1u);
  case '<':
    if (HasCharAt(source, index + 1u, '<')) {
      if (HasCharAt(source, index + 2u, '=')) {
        return Token(Objc3LexTokenKind::LessLessEqual, "<<=", 3u);
      }
      return Token(Objc3LexTokenKind::LessLess, "<<", 2u);
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::LessEqual, "<=", 2u);
    }
    return Token(Objc3LexTokenKind::Less, "<", 1u);
  case '>':
    if (HasCharAt(source, index + 1u, '>')) {
      if (HasCharAt(source, index + 2u, '=')) {
        return Token(Objc3LexTokenKind::GreaterGreaterEqual, ">>=", 3u);
      }
      return Token(Objc3LexTokenKind::GreaterGreater, ">>", 2u);
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::GreaterEqual, ">=", 2u);
    }
    return Token(Objc3LexTokenKind::Greater, ">", 1u);
  case '&':
    if (HasCharAt(source, index + 1u, '&')) {
      return Token(Objc3LexTokenKind::AndAnd, "&&", 2u);
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::AmpersandEqual, "&=", 2u);
    }
    return Token(Objc3LexTokenKind::Ampersand, "&", 1u);
  case '|':
    if (HasCharAt(source, index + 1u, '|')) {
      return Token(Objc3LexTokenKind::OrOr, "||", 2u);
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::PipeEqual, "|=", 2u);
    }
    return Token(Objc3LexTokenKind::Pipe, "|", 1u);
  case '^':
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::CaretEqual, "^=", 2u);
    }
    return Token(Objc3LexTokenKind::Caret, "^", 1u);
  case '?':
    if (HasCharAt(source, index + 1u, '?')) {
      return Token(Objc3LexTokenKind::QuestionQuestion, "??", 2u);
    }
    if (HasCharAt(source, index + 1u, '.')) {
      return Token(Objc3LexTokenKind::QuestionDot, "?.", 2u);
    }
    return Token(Objc3LexTokenKind::Question, "?", 1u);
  case '~':
    return Token(Objc3LexTokenKind::Tilde, "~", 1u);
  case '+':
    if (HasCharAt(source, index + 1u, '+')) {
      return Token(Objc3LexTokenKind::PlusPlus, "++", 2u);
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::PlusEqual, "+=", 2u);
    }
    return Token(Objc3LexTokenKind::Plus, "+", 1u);
  case '-':
    if (HasCharAt(source, index + 1u, '-')) {
      return Token(Objc3LexTokenKind::MinusMinus, "--", 2u);
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::MinusEqual, "-=", 2u);
    }
    return Token(Objc3LexTokenKind::Minus, "-", 1u);
  case '*':
    if (HasCharAt(source, index + 1u, '/')) {
      return StrayBlockCommentTerminator();
    }
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::StarEqual, "*=", 2u);
    }
    return Token(Objc3LexTokenKind::Star, "*", 1u);
  case '/':
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::SlashEqual, "/=", 2u);
    }
    return Token(Objc3LexTokenKind::Slash, "/", 1u);
  case '%':
    if (HasCharAt(source, index + 1u, '=')) {
      return Token(Objc3LexTokenKind::PercentEqual, "%=", 2u);
    }
    return Token(Objc3LexTokenKind::Percent, "%", 1u);
  default:
    return Objc3PunctuationTokenClassification{};
  }
}
