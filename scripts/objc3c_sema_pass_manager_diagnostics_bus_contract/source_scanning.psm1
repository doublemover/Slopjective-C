function Assert-SemaPassManagerDiagnosticsBusSourceFiles {
  param([Parameter(Mandatory = $true)][object]$Config)

  $sources = $Config.sources
  Assert-FileExists -Path $sources.sema_header -Id "source.sema_header.exists" -Description "sema header"
  Assert-FileExists -Path $sources.sema_contract_header -Id "source.sema_contract_header.exists" -Description "sema contract header"
  Assert-FileExists -Path $sources.sema_pass_manager_contract_header -Id "source.sema_pass_manager_contract_header.exists" -Description "sema pass-manager contract header"
  Assert-FileExists -Path $sources.sema_pass_manager_header -Id "source.sema_pass_manager_header.exists" -Description "sema pass-manager header"
  Assert-FileExists -Path $sources.sema_pass_manager_source -Id "source.sema_pass_manager_source.exists" -Description "sema pass-manager source"
  Assert-FileExists -Path $sources.sema_diagnostics_bus_header -Id "source.sema_diagnostics_bus_header.exists" -Description "sema diagnostics-bus header"
  Assert-FileExists -Path $sources.sema_diagnostics_bus_source -Id "source.sema_diagnostics_bus_source.exists" -Description "sema diagnostics-bus source"
  Assert-FileExists -Path $sources.sema_source -Id "source.sema_source.exists" -Description "sema source"
  Assert-FileExists -Path $sources.sema_pure_contract_source -Id "source.sema_pure_contract_source.exists" -Description "sema pure-contract source"
  Assert-FileExists -Path $sources.sema_static_analysis_header -Id "source.sema_static_analysis_header.exists" -Description "sema static analysis header"
  Assert-FileExists -Path $sources.sema_static_analysis_source -Id "source.sema_static_analysis_source.exists" -Description "sema static analysis source"
  Assert-FileExists -Path $sources.parse_diagnostics_bus_header -Id "source.parse_diagnostics_bus_header.exists" -Description "parse diagnostics-bus header"
  Assert-FileExists -Path $sources.pipeline_source -Id "source.pipeline_source.exists" -Description "pipeline source"
  Assert-FileExists -Path $sources.frontend_types -Id "source.frontend_types.exists" -Description "frontend types"
  Assert-FileExists -Path $sources.cmake -Id "source.cmake.exists" -Description "native CMake file"
  Assert-FileExists -Path $Config.fixtures.positive -Id "fixture.positive.exists" -Description "positive sema fixture"
  foreach ($negativeFixturePath in $Config.fixtures.negative) {
    Assert-FileExists `
      -Path $negativeFixturePath `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)) `
      -Description "negative sema fixture"
  }
}

function Read-SemaPassManagerDiagnosticsBusSourceSnapshot {
  param([Parameter(Mandatory = $true)][object]$Config)

  $sources = $Config.sources
  return [pscustomobject]@{
    sema_header = Read-NormalizedText -Path $sources.sema_header
    sema_contract_header = Read-NormalizedText -Path $sources.sema_contract_header
    sema_pass_manager_contract_header = Read-NormalizedText -Path $sources.sema_pass_manager_contract_header
    sema_pass_manager_header = Read-NormalizedText -Path $sources.sema_pass_manager_header
    sema_pass_manager_source = Read-NormalizedText -Path $sources.sema_pass_manager_source
    sema_diagnostics_bus_header = Read-NormalizedText -Path $sources.sema_diagnostics_bus_header
    parse_diagnostics_bus_header = Read-NormalizedText -Path $sources.parse_diagnostics_bus_header
    sema_source = Read-NormalizedText -Path $sources.sema_source
    sema_pure_contract_source = Read-NormalizedText -Path $sources.sema_pure_contract_source
    pipeline_source = Read-NormalizedText -Path $sources.pipeline_source
    frontend_types = Read-NormalizedText -Path $sources.frontend_types
    cmake = Read-NormalizedText -Path $sources.cmake
  }
}

function Assert-SemaPassManagerDiagnosticsBusStaticContracts {
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

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.frontend_types -RequiredTokens @(
      '#include "parse/objc3_diagnostics_bus.h"',
      '#include "sema/objc3_sema_contract.h"',
      "Objc3FrontendDiagnosticsBus stage_diagnostics;",
      "Objc3SemanticIntegrationSurface integration_surface;"
    )) `
    -Id "contract.frontend_types.diagnostics_bus" `
    -FailureMessage "frontend types missing stage diagnostics-bus contract fields for semantic pass extraction" `
    -PassMessage "frontend types define semantic diagnostics-bus contract fields"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.parse_diagnostics_bus_header -RequiredTokens @(
      "struct Objc3FrontendDiagnosticsBus",
      "std::vector<std::string> lexer;",
      "std::vector<std::string> parser;",
      "std::vector<std::string> semantic;",
      "inline void TransportObjc3DiagnosticsToParsedProgram(const Objc3FrontendDiagnosticsBus &bus, Objc3ParsedProgram &program)"
    )) `
    -Id "contract.parse_diagnostics_bus.surface" `
    -FailureMessage "parse diagnostics-bus header missing frontend diagnostics transport surface" `
    -PassMessage "parse diagnostics-bus header exposes frontend diagnostics transport surface"

  $pipelinePassManagerWiring = (
    $Snapshot.pipeline_source.IndexOf('#include "sema/objc3_sema_pass_manager.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("Objc3SemanticValidationOptions semantic_options;", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("semantic_options.max_message_send_args = options.lowering.max_message_send_args;", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("Objc3SemaPassManagerInput sema_input;", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("sema_input.program = &result.program;", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("sema_input.validation_options = semantic_options;", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);", [System.StringComparison]::Ordinal) -ge 0 -and
    $Snapshot.pipeline_source.IndexOf("result.integration_surface = std::move(sema_result.integration_surface);", [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $pipelinePassManagerWiring `
    -Id "contract.pipeline.sema_pass_manager_wiring" `
    -FailureMessage "pipeline missing expected sema pass-manager extraction wiring into semantic diagnostics bus" `
    -PassMessage "pipeline wires extracted sema pass manager into semantic diagnostics bus"

  Assert-Contract `
    -Condition ($Snapshot.pipeline_source.IndexOf("TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "contract.pipeline.semantic_diagnostics_merge" `
    -FailureMessage "pipeline missing semantic diagnostics-bus merge into program diagnostics stream" `
    -PassMessage "pipeline merges semantic diagnostics-bus into program diagnostics stream"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $Snapshot.cmake -RequiredTokens @(
      "add_library(objc3c_sema STATIC",
      "src/sema/objc3_sema_pass_manager.cpp",
      "src/sema/objc3_semantic_passes.cpp",
      "src/sema/objc3_static_analysis.cpp",
      "src/sema/objc3_pure_contract.cpp",
      "target_link_libraries(objc3c_pipeline PUBLIC",
      "objc3c_sema"
    )) `
    -Id "contract.cmake.sema_target_registered" `
    -FailureMessage "CMake missing sema target registration/linkage for extracted pass-manager and diagnostics-bus surfaces" `
    -PassMessage "CMake registers sema target/linkage for extracted pass-manager and diagnostics-bus surfaces"
}
