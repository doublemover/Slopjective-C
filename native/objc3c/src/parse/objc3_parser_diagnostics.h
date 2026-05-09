#pragma once

#include <string>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

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
std::string BuildObjc3MissingSemicolonDiagnostic(
    const Objc3LexToken &token,
    const std::string &context);
std::string BuildObjc3InvalidDeclarationIdentifierDiagnostic(
    const Objc3LexToken &token);
std::string BuildObjc3RemovedOptionalTemplateAliasDiagnostic(
    const Objc3LexToken &token);
std::string BuildObjc3UnsupportedTopLevelDiagnostic(
    const Objc3LexToken &token);

}  // namespace objc3c::parse
