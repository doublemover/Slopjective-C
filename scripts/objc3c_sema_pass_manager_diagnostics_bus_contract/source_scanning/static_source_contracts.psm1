function Assert-SemaPassManagerDiagnosticsBusSourceContracts {
  param([Parameter(Mandatory = $true)][object]$Snapshot)

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_pass_manager_source -RequiredTokens @(
      '#include "sema/objc3_sema_pass_manager.h"',
      '#include "sema/objc3_semantic_passes.h"',
      "Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input)",
      "for (const Objc3SemaPassId pass : kObjc3SemaPassOrder)",
      "BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);",
      "ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);",
      "ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);",
      "input.diagnostics_bus.PublishBatch(pass_diagnostics);",
      "result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();"
    )) `
    -Id "contract.sema_pass_manager_source.surface" `
    -FailureMessage "sema pass-manager source missing pass-execution or diagnostics-bus publish markers" `
    -PassMessage "sema pass-manager source executes pass order and publishes diagnostics through bus"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_diagnostics_bus_header -RequiredTokens @(
      '#include "sema/objc3_sema_pass_manager_contract.h"',
      "using Objc3SemanticDiagnosticsBus = Objc3SemaDiagnosticsBus;"
    )) `
    -Id "contract.sema_diagnostics_bus_header.alias" `
    -FailureMessage "sema diagnostics-bus header missing compatibility alias surface" `
    -PassMessage "sema diagnostics-bus header keeps compatibility alias to pass-manager diagnostics bus"

  $semaSourceHasEntrypoints = (
    $Snapshot.sema_source.IndexOf('#include "sema/objc3_semantic_passes.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.sema_source.IndexOf('#include "sema/objc3_static_analysis.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($Snapshot.sema_source, 'Objc3SemanticIntegrationSurface\s+BuildSemanticIntegrationSurface\s*\(') -and
    [regex]::IsMatch($Snapshot.sema_source, 'void\s+ValidateSemanticBodies\s*\(') -and
    $Snapshot.sema_source.IndexOf("O3S200", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.sema_source.IndexOf("O3S205", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.sema_source.IndexOf("O3S206", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.sema_source.IndexOf("O3S214", [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $semaSourceHasEntrypoints `
    -Id "contract.sema_source.entrypoints" `
    -FailureMessage "sema source missing expected pass-manager entrypoint markers" `
    -PassMessage "sema source contains expected pass-manager entrypoint markers"

  Assert-Contract `
    -Condition ($Snapshot.sema_source.IndexOf("ValidatePureContractSemanticDiagnostics(", [System.StringComparison]::Ordinal) -lt 0) `
    -Id "contract.sema_source.no_pure_contract_impl" `
    -FailureMessage "sema source contains pure-contract pass implementation marker; expected extracted implementation in objc3_pure_contract.cpp" `
    -PassMessage "pure-contract pass implementation remains extracted from sema source"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.sema_pure_contract_source -RequiredTokens @(
      '#include "sema/objc3_semantic_passes.h"',
      "struct PureContractEffectInfo",
      "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,",
      "std::vector<std::string> &diagnostics)",
      "O3S215"
    )) `
    -Id "contract.sema_pure_contract_source.surface" `
    -FailureMessage "sema pure-contract source missing expected extracted diagnostics pass surface" `
    -PassMessage "sema pure-contract source exposes extracted diagnostics pass surface"
}
