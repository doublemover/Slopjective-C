#include "token/objc3_token_keyword_data.h"

namespace {

constexpr Objc3KeywordTokenEntry kIdentifierKeywords[] = {
    {"module", Objc3LexTokenKind::KwModule},
    {"let", Objc3LexTokenKind::KwLet},
    {"var", Objc3LexTokenKind::KwVar},
    {"fn", Objc3LexTokenKind::KwFn},
    {"async", Objc3LexTokenKind::KwAsync},
    {"pure", Objc3LexTokenKind::KwPure},
    {"extern", Objc3LexTokenKind::KwExtern},
    {"return", Objc3LexTokenKind::KwReturn},
    {"if", Objc3LexTokenKind::KwIf},
    {"else", Objc3LexTokenKind::KwElse},
    {"guard", Objc3LexTokenKind::KwGuard},
    {"defer", Objc3LexTokenKind::KwDefer},
    {"do", Objc3LexTokenKind::KwDo},
    {"await", Objc3LexTokenKind::KwAwait},
    {"try", Objc3LexTokenKind::KwTry},
    {"throw", Objc3LexTokenKind::KwThrow},
    {"catch", Objc3LexTokenKind::KwCatch},
    {"for", Objc3LexTokenKind::KwFor},
    {"switch", Objc3LexTokenKind::KwSwitch},
    {"match", Objc3LexTokenKind::KwMatch},
    {"case", Objc3LexTokenKind::KwCase},
    {"default", Objc3LexTokenKind::KwDefault},
    {"while", Objc3LexTokenKind::KwWhile},
    {"break", Objc3LexTokenKind::KwBreak},
    {"continue", Objc3LexTokenKind::KwContinue},
    {"i32", Objc3LexTokenKind::KwI32},
    {"bool", Objc3LexTokenKind::KwBool},
    {"BOOL", Objc3LexTokenKind::KwBOOL},
    {"NSInteger", Objc3LexTokenKind::KwNSInteger},
    {"NSUInteger", Objc3LexTokenKind::KwNSUInteger},
    {"void", Objc3LexTokenKind::KwVoid},
    {"id", Objc3LexTokenKind::KwId},
    {"Class", Objc3LexTokenKind::KwClass},
    {"SEL", Objc3LexTokenKind::KwSEL},
    {"Protocol", Objc3LexTokenKind::KwProtocol},
    {"instancetype", Objc3LexTokenKind::KwInstancetype},
    {"true", Objc3LexTokenKind::KwTrue},
    {"false", Objc3LexTokenKind::KwFalse},
    {"nil", Objc3LexTokenKind::KwNil},
};

constexpr Objc3KeywordTokenEntry kAtDirectiveKeywords[] = {
    {"interface", Objc3LexTokenKind::KwAtInterface},
    {"implementation", Objc3LexTokenKind::KwAtImplementation},
    {"protocol", Objc3LexTokenKind::KwAtProtocol},
    {"required", Objc3LexTokenKind::KwAtRequired},
    {"optional", Objc3LexTokenKind::KwAtOptional},
    {"property", Objc3LexTokenKind::KwAtProperty},
    {"keypath", Objc3LexTokenKind::KwAtKeypath},
    {"import", Objc3LexTokenKind::KwAtImport},
    {"reify_generics", Objc3LexTokenKind::KwAtReifyGenerics},
    {"cleanup", Objc3LexTokenKind::KwAtCleanup},
    {"resource", Objc3LexTokenKind::KwAtResource},
    {"end", Objc3LexTokenKind::KwAtEnd},
    {"autoreleasepool", Objc3LexTokenKind::KwAtAutoreleasePool},
};

}  // namespace

const Objc3KeywordTokenEntry *Objc3IdentifierKeywordTokenEntries(
    std::size_t &count) {
  count = sizeof(kIdentifierKeywords) / sizeof(kIdentifierKeywords[0]);
  return kIdentifierKeywords;
}

const Objc3KeywordTokenEntry *Objc3AtDirectiveKeywordTokenEntries(
    std::size_t &count) {
  count = sizeof(kAtDirectiveKeywords) / sizeof(kAtDirectiveKeywords[0]);
  return kAtDirectiveKeywords;
}
