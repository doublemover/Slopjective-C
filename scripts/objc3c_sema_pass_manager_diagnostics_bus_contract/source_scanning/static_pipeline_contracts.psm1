function Assert-SemaPassManagerDiagnosticsBusPipelineContracts {
  param([Parameter(Mandatory = $true)][object]$Snapshot)

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
