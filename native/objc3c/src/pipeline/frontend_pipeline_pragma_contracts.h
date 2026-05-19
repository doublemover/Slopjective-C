#pragma once

#include "lex/objc3_lexer.h"
#include "pipeline/objc3_frontend_types.h"

void CopyLanguageVersionPragmaContract(
    const Objc3LexerLanguageVersionPragmaContract &source,
    Objc3FrontendLanguageVersionPragmaContract &target);
void CopyNamedIdentifierPragmaContract(
    const Objc3LexerNamedIdentifierPragmaContract &source,
    Objc3FrontendNamedIdentifierPragmaContract &target);
void CopyBootstrapRegistrationSourceContract(
    const Objc3LexerBootstrapRegistrationSourceContract &source,
    Objc3FrontendBootstrapRegistrationSourcePragmaContract &target);
