#include "lex/objc3_lexer.h"

Objc3Lexer::Objc3Lexer(const std::string &source, const Objc3LexerOptions &options)
    : source_(source), options_(options) {}

const Objc3LexerCanonicalLiteralRejectionCounts
    &Objc3Lexer::CanonicalLiteralRejectionCounts() const {
  return canonical_literal_rejection_counts_;
}

const Objc3LexerLanguageVersionPragmaContract &Objc3Lexer::LanguageVersionPragmaContract() const {
  return language_version_pragma_contract_;
}

const Objc3LexerBootstrapRegistrationSourceContract &
Objc3Lexer::BootstrapRegistrationSourceContract() const {
  return bootstrap_registration_source_contract_;
}
