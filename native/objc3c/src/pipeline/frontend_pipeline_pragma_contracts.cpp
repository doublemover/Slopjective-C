#include "pipeline/frontend_pipeline_pragma_contracts.h"

void CopyLanguageVersionPragmaContract(
    const Objc3LexerLanguageVersionPragmaContract &source,
    Objc3FrontendLanguageVersionPragmaContract &target) {
  target.seen = source.seen;
  target.directive_count = source.directive_count;
  target.duplicate = source.duplicate;
  target.non_leading = source.non_leading;
  target.first_line = source.first_line;
  target.first_column = source.first_column;
  target.last_line = source.last_line;
  target.last_column = source.last_column;
}

void CopyNamedIdentifierPragmaContract(
    const Objc3LexerNamedIdentifierPragmaContract &source,
    Objc3FrontendNamedIdentifierPragmaContract &target) {
  target.seen = source.seen;
  target.directive_count = source.directive_count;
  target.duplicate = source.duplicate;
  target.non_leading = source.non_leading;
  target.first_line = source.first_line;
  target.first_column = source.first_column;
  target.last_line = source.last_line;
  target.last_column = source.last_column;
  target.identifier = source.identifier;
}

void CopyBootstrapRegistrationSourceContract(
    const Objc3LexerBootstrapRegistrationSourceContract &source,
    Objc3FrontendBootstrapRegistrationSourcePragmaContract &target) {
  CopyNamedIdentifierPragmaContract(source.registration_descriptor,
                                    target.registration_descriptor);
  CopyNamedIdentifierPragmaContract(source.image_root, target.image_root);
}
