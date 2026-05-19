function Assert-SemaPassManagerDiagnosticsBusHeaderContracts {
  param([Parameter(Mandatory = $true)][object]$Snapshot)

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_header -RequiredTokens @(
      '#include "sema/objc3_sema_contract.h"',
      "BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
      "ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,",
      "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,"
    )) `
    -Id "contract.sema_header.surface" `
    -FailureMessage "sema header missing expected pass-manager extraction and diagnostics-bus contract surface" `
    -PassMessage "sema header exposes pass-manager extraction and diagnostics-bus contract surface"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_contract_header -RequiredTokens @(
      '#include "parse/objc3_parser_contract.h"',
      "struct FunctionInfo",
      "struct Objc3SemanticIntegrationSurface",
      "struct Objc3SemanticValidationOptions",
      "bool built = false;",
      "std::size_t max_message_send_args = 4;",
      "bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values);"
    )) `
    -Id "contract.sema_contract_header.surface" `
    -FailureMessage "sema contract header missing semantic integration surface/value contract definitions" `
    -PassMessage "sema contract header exposes semantic integration surface/value contracts"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_pass_manager_contract_header -RequiredTokens @(
      '#include "sema/objc3_sema_contract.h"',
      "enum class Objc3SemaPassId",
      "BuildIntegrationSurface = 0",
      "ValidateBodies = 1",
      "ValidatePureContract = 2",
      "inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder",
      "struct Objc3SemaDiagnosticsBus",
      "void Publish(const std::string &diagnostic) const",
      "void PublishBatch(const std::vector<std::string> &batch) const",
      "std::size_t Count() const",
      "struct Objc3SemaPassManagerInput",
      "Objc3SemaDiagnosticsBus diagnostics_bus;",
      "struct Objc3SemaPassManagerResult",
      "std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};",
      "bool executed = false;"
    )) `
    -Id "contract.sema_pass_manager_contract_header.surface" `
    -FailureMessage "sema pass-manager contract header missing pass-order or diagnostics-bus contract surface" `
    -PassMessage "sema pass-manager contract header exposes pass-order and diagnostics-bus contract surface"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_pass_manager_header -RequiredTokens @(
      '#include "sema/objc3_sema_pass_manager_contract.h"',
      "Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input);"
    )) `
    -Id "contract.sema_pass_manager_header.surface" `
    -FailureMessage "sema pass-manager header missing RunObjc3SemaPassManager entrypoint surface" `
    -PassMessage "sema pass-manager header exposes RunObjc3SemaPassManager entrypoint"
}
