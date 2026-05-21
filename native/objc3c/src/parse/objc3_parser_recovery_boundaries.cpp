#include "parse/objc3_parser_recovery_boundaries.h"

namespace objc3c::parse {

bool Objc3ParserRecoveryBoundaryIsHardCutoverOwned() {
  return Objc3DiagnosticStageIsHardCutover(Objc3FrontendDiagnosticStage::kParser) &&
         Objc3DiagnosticOwnerIsExplicit(kObjc3ParserRecoveryBoundaryOwner);
}

bool Objc3ParserRecoveryBoundaryCountsAsSuccess() {
  const Objc3DiagnosticStageOwnerContract *contract =
      FindObjc3DiagnosticStageOwnerContract(Objc3FrontendDiagnosticStage::kParser);
  return contract == nullptr || contract->recovery_counts_as_success;
}

bool IsObjc3TopLevelRecoveryBoundaryToken(Objc3LexTokenKind kind) {
  switch (kind) {
  case Objc3LexTokenKind::KwModule:
  case Objc3LexTokenKind::KwLet:
  case Objc3LexTokenKind::KwVar:
  case Objc3LexTokenKind::KwFn:
  case Objc3LexTokenKind::KwPure:
  case Objc3LexTokenKind::KwExtern:
  case Objc3LexTokenKind::KwAsync:
  case Objc3LexTokenKind::KwAtInterface:
  case Objc3LexTokenKind::KwAtImplementation:
  case Objc3LexTokenKind::KwAtProtocol:
  case Objc3LexTokenKind::KwAtProperty:
  case Objc3LexTokenKind::KwAtEnd:
    return true;
  default:
    return false;
  }
}

bool IsObjc3StatementRecoveryBoundaryToken(Objc3LexTokenKind kind) {
  switch (kind) {
  case Objc3LexTokenKind::KwLet:
  case Objc3LexTokenKind::KwVar:
  case Objc3LexTokenKind::KwReturn:
  case Objc3LexTokenKind::KwIf:
  case Objc3LexTokenKind::KwGuard:
  case Objc3LexTokenKind::KwDefer:
  case Objc3LexTokenKind::KwDo:
  case Objc3LexTokenKind::KwMatch:
  case Objc3LexTokenKind::KwFor:
  case Objc3LexTokenKind::KwSwitch:
  case Objc3LexTokenKind::KwWhile:
  case Objc3LexTokenKind::KwBreak:
  case Objc3LexTokenKind::KwContinue:
  case Objc3LexTokenKind::KwCase:
  case Objc3LexTokenKind::KwDefault:
  case Objc3LexTokenKind::KwAtAutoreleasePool:
  case Objc3LexTokenKind::RBrace:
    return true;
  default:
    return false;
  }
}

}  // namespace objc3c::parse
