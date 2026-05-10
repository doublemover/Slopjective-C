$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_contract_helpers.psm1") -Force -DisableNameChecking

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native/typed_abi/sema-pass-manager-diagnostics-bus-contract"
$defaultRunId = "typed_abi-sema-type-system-default"
$configuredRunId = $env:OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID

$runId = Resolve-ValidatedRunId -ConfiguredRunId $configuredRunId -DefaultRunId $defaultRunId
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$runDirRel = "tmp/artifacts/compilation/objc3c-native/typed_abi/sema-pass-manager-diagnostics-bus-contract/$runId"
$summaryRel = "$runDirRel/summary.json"

$buildScriptPath = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
$nativeExePath = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExePath } else { $configuredNativeExe }
$nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)

$semaHeaderPath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_semantic_passes.h"
$semaContractHeaderPath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_contract.h"
$semaPassManagerContractHeaderPath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_pass_manager_contract.h"
$semaPassManagerHeaderPath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_pass_manager.h"
$semaPassManagerSourcePath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
$semaDiagnosticsBusHeaderPath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_diagnostics_bus.h"
$semaDiagnosticsBusSourcePath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
$semaSourcePath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_semantic_passes.cpp"
$semaPureContractSourcePath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_pure_contract.cpp"
$semaStaticAnalysisHeaderPath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_static_analysis.h"
$semaStaticAnalysisSourcePath = Join-Path $repoRoot "native/objc3c/src/sema/objc3_static_analysis.cpp"
$parseDiagnosticsBusHeaderPath = Join-Path $repoRoot "native/objc3c/src/parse/objc3_diagnostics_bus.h"
$pipelineSourcePath = Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
$frontendTypesPath = Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_types.h"
$cmakePath = Join-Path $repoRoot "native/objc3c/CMakeLists.txt"

$positiveFixturePath = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive/typed_i32_bool.objc3"
$negativeFixturePaths = @(
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_type_mismatch.objc3"),
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_pure_definition_impure_message_send.objc3")
)

$checks = New-Object 'System.Collections.Generic.List[object]'
$caseResults = New-Object 'System.Collections.Generic.List[object]'
$hadFatalError = $false
$fatalErrorMessage = ""

Set-SemaPassManagerDiagnosticsBusContractContext -Checks $checks -RepoRoot $repoRoot

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

Push-Location $repoRoot
try {
  Assert-FileExists -Path $semaHeaderPath -Id "source.sema_header.exists" -Description "sema header"
  Assert-FileExists -Path $semaContractHeaderPath -Id "source.sema_contract_header.exists" -Description "sema contract header"
  Assert-FileExists -Path $semaPassManagerContractHeaderPath -Id "source.sema_pass_manager_contract_header.exists" -Description "sema pass-manager contract header"
  Assert-FileExists -Path $semaPassManagerHeaderPath -Id "source.sema_pass_manager_header.exists" -Description "sema pass-manager header"
  Assert-FileExists -Path $semaPassManagerSourcePath -Id "source.sema_pass_manager_source.exists" -Description "sema pass-manager source"
  Assert-FileExists -Path $semaDiagnosticsBusHeaderPath -Id "source.sema_diagnostics_bus_header.exists" -Description "sema diagnostics-bus header"
  Assert-FileExists -Path $semaDiagnosticsBusSourcePath -Id "source.sema_diagnostics_bus_source.exists" -Description "sema diagnostics-bus source"
  Assert-FileExists -Path $semaSourcePath -Id "source.sema_source.exists" -Description "sema source"
  Assert-FileExists -Path $semaPureContractSourcePath -Id "source.sema_pure_contract_source.exists" -Description "sema pure-contract source"
  Assert-FileExists -Path $semaStaticAnalysisHeaderPath -Id "source.sema_static_analysis_header.exists" -Description "sema static analysis header"
  Assert-FileExists -Path $semaStaticAnalysisSourcePath -Id "source.sema_static_analysis_source.exists" -Description "sema static analysis source"
  Assert-FileExists -Path $parseDiagnosticsBusHeaderPath -Id "source.parse_diagnostics_bus_header.exists" -Description "parse diagnostics-bus header"
  Assert-FileExists -Path $pipelineSourcePath -Id "source.pipeline_source.exists" -Description "pipeline source"
  Assert-FileExists -Path $frontendTypesPath -Id "source.frontend_types.exists" -Description "frontend types"
  Assert-FileExists -Path $cmakePath -Id "source.cmake.exists" -Description "native CMake file"
  Assert-FileExists -Path $positiveFixturePath -Id "fixture.positive.exists" -Description "positive sema fixture"
  foreach ($negativeFixturePath in $negativeFixturePaths) {
    Assert-FileExists `
      -Path $negativeFixturePath `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)) `
      -Description "negative sema fixture"
  }

  $semaHeaderText = Read-NormalizedText -Path $semaHeaderPath
  $semaContractHeaderText = Read-NormalizedText -Path $semaContractHeaderPath
  $semaPassManagerContractHeaderText = Read-NormalizedText -Path $semaPassManagerContractHeaderPath
  $semaPassManagerHeaderText = Read-NormalizedText -Path $semaPassManagerHeaderPath
  $semaPassManagerSourceText = Read-NormalizedText -Path $semaPassManagerSourcePath
  $semaDiagnosticsBusHeaderText = Read-NormalizedText -Path $semaDiagnosticsBusHeaderPath
  $parseDiagnosticsBusHeaderText = Read-NormalizedText -Path $parseDiagnosticsBusHeaderPath
  $semaSourceText = Read-NormalizedText -Path $semaSourcePath
  $semaPureContractSourceText = Read-NormalizedText -Path $semaPureContractSourcePath
  $pipelineSourceText = Read-NormalizedText -Path $pipelineSourcePath
  $frontendTypesText = Read-NormalizedText -Path $frontendTypesPath
  $cmakeText = Read-NormalizedText -Path $cmakePath

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $semaHeaderText -RequiredTokens @(
      '#include "sema/objc3_sema_contract.h"',
      "BuildSemanticIntegrationSurface(const Objc3ParsedProgram &program,",
      "ValidateSemanticBodies(const Objc3ParsedProgram &program, const Objc3SemanticIntegrationSurface &surface,",
      "ValidatePureContractSemanticDiagnostics(const Objc3ParsedProgram &program,"
    )) `
    -Id "contract.sema_header.surface" `
    -FailureMessage "sema header missing expected pass-manager extraction and diagnostics-bus contract surface" `
    -PassMessage "sema header exposes pass-manager extraction and diagnostics-bus contract surface"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $semaContractHeaderText -RequiredTokens @(
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
    -Condition (Assert-TokensPresent -Text $semaPassManagerContractHeaderText -RequiredTokens @(
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
    -Condition (Assert-TokensPresent -Text $semaPassManagerHeaderText -RequiredTokens @(
      '#include "sema/objc3_sema_pass_manager_contract.h"',
      "Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input);"
    )) `
    -Id "contract.sema_pass_manager_header.surface" `
    -FailureMessage "sema pass-manager header missing RunObjc3SemaPassManager entrypoint surface" `
    -PassMessage "sema pass-manager header exposes RunObjc3SemaPassManager entrypoint"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $semaPassManagerSourceText -RequiredTokens @(
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
    -Condition (Assert-TokensPresent -Text $semaDiagnosticsBusHeaderText -RequiredTokens @(
      '#include "sema/objc3_sema_pass_manager_contract.h"',
      "using Objc3SemanticDiagnosticsBus = Objc3SemaDiagnosticsBus;"
    )) `
    -Id "contract.sema_diagnostics_bus_header.alias" `
    -FailureMessage "sema diagnostics-bus header missing compatibility alias surface" `
    -PassMessage "sema diagnostics-bus header keeps compatibility alias to pass-manager diagnostics bus"

  $semaSourceHasEntrypoints = (
    $semaSourceText.IndexOf('#include "sema/objc3_semantic_passes.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $semaSourceText.IndexOf('#include "sema/objc3_static_analysis.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($semaSourceText, 'Objc3SemanticIntegrationSurface\s+BuildSemanticIntegrationSurface\s*\(') -and
    [regex]::IsMatch($semaSourceText, 'void\s+ValidateSemanticBodies\s*\(') -and
    $semaSourceText.IndexOf("O3S200", [System.StringComparison]::Ordinal) -ge 0 -and
    $semaSourceText.IndexOf("O3S205", [System.StringComparison]::Ordinal) -ge 0 -and
    $semaSourceText.IndexOf("O3S206", [System.StringComparison]::Ordinal) -ge 0 -and
    $semaSourceText.IndexOf("O3S214", [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $semaSourceHasEntrypoints `
    -Id "contract.sema_source.entrypoints" `
    -FailureMessage "sema source missing expected pass-manager entrypoint markers" `
    -PassMessage "sema source contains expected pass-manager entrypoint markers"

  Assert-Contract `
    -Condition ($semaSourceText.IndexOf("ValidatePureContractSemanticDiagnostics(", [System.StringComparison]::Ordinal) -lt 0) `
    -Id "contract.sema_source.no_pure_contract_impl" `
    -FailureMessage "sema source contains pure-contract pass implementation marker; expected extracted implementation in objc3_pure_contract.cpp" `
    -PassMessage "pure-contract pass implementation remains extracted from sema source"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $semaPureContractSourceText -RequiredTokens @(
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
    -Condition (Assert-TokensPresent -Text $frontendTypesText -RequiredTokens @(
      '#include "parse/objc3_diagnostics_bus.h"',
      '#include "sema/objc3_sema_contract.h"',
      "Objc3FrontendDiagnosticsBus stage_diagnostics;",
      "Objc3SemanticIntegrationSurface integration_surface;"
    )) `
    -Id "contract.frontend_types.diagnostics_bus" `
    -FailureMessage "frontend types missing stage diagnostics-bus contract fields for semantic pass extraction" `
    -PassMessage "frontend types define semantic diagnostics-bus contract fields"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $parseDiagnosticsBusHeaderText -RequiredTokens @(
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
    $pipelineSourceText.IndexOf('#include "sema/objc3_sema_pass_manager.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("Objc3SemanticValidationOptions semantic_options;", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("semantic_options.max_message_send_args = options.lowering.max_message_send_args;", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("Objc3SemaPassManagerInput sema_input;", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("sema_input.program = &result.program;", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("sema_input.validation_options = semantic_options;", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("result.integration_surface = std::move(sema_result.integration_surface);", [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $pipelinePassManagerWiring `
    -Id "contract.pipeline.sema_pass_manager_wiring" `
    -FailureMessage "pipeline missing expected sema pass-manager extraction wiring into semantic diagnostics bus" `
    -PassMessage "pipeline wires extracted sema pass manager into semantic diagnostics bus"

  Assert-Contract `
    -Condition ($pipelineSourceText.IndexOf("TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "contract.pipeline.semantic_diagnostics_merge" `
    -FailureMessage "pipeline missing semantic diagnostics-bus merge into program diagnostics stream" `
    -PassMessage "pipeline merges semantic diagnostics-bus into program diagnostics stream"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $cmakeText -RequiredTokens @(
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

  Invoke-SemaPassManagerDiagnosticsBusRuntimeCases `
    -RepoRoot $repoRoot `
    -RunDir $runDir `
    -BuildScriptPath $buildScriptPath `
    -NativeExePath $nativeExePath `
    -NativeExeExplicit $nativeExeExplicit `
    -PositiveFixturePath $positiveFixturePath `
    -NegativeFixturePaths $negativeFixturePaths `
    -CaseResults $caseResults
}
catch {
  $hadFatalError = $true
  $fatalErrorMessage = $_.Exception.Message
  Write-Output ("error: {0}" -f $fatalErrorMessage)
}
finally {
  Pop-Location
}

$checkArray = $checks.ToArray()
$caseResultArray = $caseResults.ToArray()
$total = $checkArray.Count
$passed = @($checkArray | Where-Object { $_.passed }).Count
$failed = $total - $passed
$status = if (-not $hadFatalError -and $total -gt 0 -and $failed -eq 0) { "PASS" } else { "FAIL" }

$nativeExeSummary = "$nativeExePath"
if (Test-Path -LiteralPath $nativeExePath -PathType Leaf) {
  try {
    $nativeExeSummary = Get-RepoRelativePath -Path $nativeExePath -Root $repoRoot
  }
  catch {
    $nativeExeSummary = "$nativeExePath"
  }
}

$summary = @{
  contract = "objc3c-sema-pass-manager-diagnostics-bus-contract-v1"
  run_id = $runId
  run_dir = $runDirRel
  summary_path = $summaryRel
  native_executable = $nativeExeSummary
  status = $status
  total = $total
  passed = $passed
  failed = $failed
  fatal_error = $fatalErrorMessage
  checks = $checkArray
  cases = $caseResultArray
}
$summary | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output ("summary_path: {0}" -f $summaryRel)
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
