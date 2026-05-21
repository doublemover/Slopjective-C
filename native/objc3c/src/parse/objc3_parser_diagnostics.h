#pragma once

#include <string>
#include <string_view>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

struct Objc3ParserDiagnosticFixIt {
  unsigned start_line = 1;
  unsigned start_column = 1;
  unsigned end_line = 1;
  unsigned end_column = 1;
  std::string label;
  std::string replacement;
  bool machine_applicable = true;
};

inline constexpr std::string_view kObjc3ParserDiagnosticOwnerContractId =
    kObjc3DiagnosticOwnerContractId;
inline constexpr std::string_view kObjc3ParserDiagnosticStageOwnerName =
    ::kObjc3ParserDiagnosticStageOwner;
inline constexpr std::string_view kObjc3ParserDiagnosticFixitOwnerName =
    ::kObjc3ParserDiagnosticFixitOwner;
inline constexpr std::string_view kObjc3ParserDiagnosticRecoveryOwnerName =
    ::kObjc3ParserDiagnosticRecoveryOwner;

bool IsObjc3ParserOwnedDiagnosticCode(const char *code);
std::string BuildObjc3ParserDiagnostic(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message);
std::string BuildObjc3ParserDiagnosticWithFixIt(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message,
    const Objc3ParserDiagnosticFixIt &fixit);
std::string BuildObjc3ParserDiagnosticWithFixItAndRecovery(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message,
    const Objc3ParserDiagnosticFixIt &fixit,
    std::string_view strategy,
    std::string_view boundary);
std::string BuildObjc3ParserDiagnosticWithRecovery(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message,
    std::string_view strategy,
    std::string_view boundary);
std::string BuildObjc3MissingSemicolonDiagnostic(
    const Objc3LexToken &token,
    const std::string &context);
std::string BuildObjc3MissingSemicolonDiagnostic(
    const Objc3LexToken &token,
    const Objc3LexToken &insertion_anchor,
    const std::string &context);
std::string BuildObjc3InvalidDeclarationIdentifierDiagnostic(
    const Objc3LexToken &token);
std::string BuildObjc3RemovedOptionalTemplateAliasDiagnostic(
    const Objc3LexToken &token);
std::string BuildObjc3UnsupportedTopLevelDiagnostic(
    const Objc3LexToken &token);

}  // namespace objc3c::parse
