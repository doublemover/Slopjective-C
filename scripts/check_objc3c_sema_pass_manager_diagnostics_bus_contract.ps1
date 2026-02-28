$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/sema-pass-manager-diagnostics-bus-contract"
$configuredRunId = $env:OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID

function Resolve-ValidatedRunId {
  param([Parameter()][string]$ConfiguredRunId)

  if ([string]::IsNullOrWhiteSpace($ConfiguredRunId)) {
    return Get-Date -Format "yyyyMMdd_HHmmss_fff"
  }

  $candidate = $ConfiguredRunId.Trim()
  if ($candidate.Length -gt 80) {
    throw "sema extraction FAIL: configured run id exceeds 80 characters"
  }
  if ($candidate -notmatch '^[A-Za-z0-9_-]+$') {
    throw "sema extraction FAIL: configured run id must match ^[A-Za-z0-9_-]+$"
  }
  return $candidate
}

$runId = Resolve-ValidatedRunId -ConfiguredRunId $configuredRunId
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$runDirRel = "tmp/artifacts/objc3c-native/sema-pass-manager-diagnostics-bus-contract/$runId"
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

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = [System.IO.Path]::GetFullPath("$Path")
  $fullRoot = [System.IO.Path]::GetFullPath("$Root")
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Read-NormalizedText {
  param([Parameter(Mandatory = $true)][string]$Path)

  $text = Get-Content -LiteralPath $Path -Raw
  $text = $text -replace "`r`n", "`n"
  $text = $text -replace "`r", "`n"
  return $text
}

function Get-FileSha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Add-Check {
  param(
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][string]$Detail,
    [Parameter()][object]$Evidence = $null
  )

  $checks.Add([pscustomobject]@{
      id = $Id
      passed = $Passed
      detail = $Detail
      evidence = $Evidence
    }) | Out-Null
}

function Assert-Contract {
  param(
    [Parameter(Mandatory = $true)][bool]$Condition,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage,
    [Parameter()][object]$Evidence = $null
  )

  if (-not $Condition) {
    Add-Check -Id $Id -Passed $false -Detail $FailureMessage -Evidence $Evidence
    throw "sema extraction contract FAIL: $FailureMessage"
  }
  Add-Check -Id $Id -Passed $true -Detail $PassMessage -Evidence $Evidence
}

function Assert-FileExists {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$Description
  )

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $repoRoot
  Assert-Contract `
    -Condition $exists `
    -Id $Id `
    -FailureMessage "missing $Description at $relative" `
    -PassMessage "found $Description at $relative" `
    -Evidence @{ path = $relative }
}

function Assert-TokensPresent {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$RequiredTokens
  )

  foreach ($token in $RequiredTokens) {
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      return $false
    }
  }
  return $true
}

function Get-ExpectedSemaCodesFromFixture {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $text = Read-NormalizedText -Path $FixturePath
  $match = [regex]::Match($text, '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$')
  if (-not $match.Success) {
    throw "sema extraction FAIL: missing expected diagnostic header in $FixturePath"
  }
  $codes = [regex]::Matches($match.Groups[1].Value, 'O3[A-Z]\d{3}') | ForEach-Object { $_.Value.ToUpperInvariant() }
  $normalized = @($codes | Sort-Object -Unique)
  if ($normalized.Count -eq 0) {
    throw "sema extraction FAIL: expected diagnostic header has no parseable codes in $FixturePath"
  }
  $nonSemaCodes = @($normalized | Where-Object { $_ -notmatch '^O3S\d{3}$' })
  if ($nonSemaCodes.Count -gt 0) {
    throw "sema extraction FAIL: expected diagnostics for sema fixture must be O3S* only in $FixturePath (found: $($nonSemaCodes -join ','))"
  }
  return $normalized
}

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

  if (-not $nativeExeExplicit -and !(Test-Path -LiteralPath $nativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $repoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $repoRoot) }
  }

  Assert-FileExists -Path $nativeExePath -Id "runtime.native_executable.exists" -Description "native executable"

  $positiveCaseDir = Join-Path $runDir "positive_smoke"
  $positiveRun1Dir = Join-Path $positiveCaseDir "run1"
  $positiveRun2Dir = Join-Path $positiveCaseDir "run2"
  New-Item -ItemType Directory -Force -Path $positiveRun1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $positiveRun2Dir | Out-Null
  $positiveRun1Log = Join-Path $positiveCaseDir "run1.log"
  $positiveRun2Log = Join-Path $positiveCaseDir "run2.log"
  $positiveArgsRun1 = @($positiveFixturePath, "--out-dir", $positiveRun1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveArgsRun2 = @($positiveFixturePath, "--out-dir", $positiveRun2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveRun1Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $positiveArgsRun1 -LogPath $positiveRun1Log
  $positiveRun2Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $positiveArgsRun2 -LogPath $positiveRun2Log

  Assert-Contract `
    -Condition ($positiveRun1Exit -eq 0 -and $positiveRun2Exit -eq 0) `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage ("positive sema fixture compile exits must be zero (run1={0} run2={1})" -f $positiveRun1Exit, $positiveRun2Exit) `
    -PassMessage "positive sema fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_log = Get-RepoRelativePath -Path $positiveRun1Log -Root $repoRoot
      run2_log = Get-RepoRelativePath -Path $positiveRun2Log -Root $repoRoot
    }

  $positiveArtifacts = @("module.manifest.json", "module.diagnostics.txt", "module.diagnostics.json", "module.ll", "module.obj", "module.object-backend.txt")
  $positiveDigests = @{}
  foreach ($artifact in $positiveArtifacts) {
    $artifactRun1 = Join-Path $positiveRun1Dir $artifact
    $artifactRun2 = Join-Path $positiveRun2Dir $artifact
    $existsRun1 = Test-Path -LiteralPath $artifactRun1 -PathType Leaf
    $existsRun2 = Test-Path -LiteralPath $artifactRun2 -PathType Leaf
    Assert-Contract `
      -Condition ($existsRun1 -and $existsRun2) `
      -Id ("runtime.positive.artifact.exists.{0}" -f $artifact) `
      -FailureMessage ("positive sema fixture missing artifact across replay: {0}" -f $artifact) `
      -PassMessage ("positive sema artifact present across replay: {0}" -f $artifact)

    if ($artifact -eq "module.obj") {
      $objRun1Bytes = (Get-Item -LiteralPath $artifactRun1).Length
      $objRun2Bytes = (Get-Item -LiteralPath $artifactRun2).Length
      Assert-Contract `
        -Condition ($objRun1Bytes -gt 0 -and $objRun2Bytes -gt 0) `
        -Id "runtime.positive.artifact.nonempty.module.obj" `
        -FailureMessage "positive sema module.obj is empty in one or more runs" `
        -PassMessage "positive sema module.obj is non-empty across replay" `
        -Evidence @{ run1_bytes = $objRun1Bytes; run2_bytes = $objRun2Bytes }
    }

    $hashRun1 = Get-FileSha256Hex -Path $artifactRun1
    $hashRun2 = Get-FileSha256Hex -Path $artifactRun2
    $positiveDigests[$artifact] = [ordered]@{
      run1_sha256 = $hashRun1
      run2_sha256 = $hashRun2
      deterministic = ($hashRun1 -eq $hashRun2)
    }
    Assert-Contract `
      -Condition ($hashRun1 -eq $hashRun2) `
      -Id ("runtime.positive.artifact.deterministic_sha256.{0}" -f $artifact) `
      -FailureMessage ("positive sema artifact hash drift detected for {0}" -f $artifact) `
      -PassMessage ("positive sema artifact hash stable for {0}" -f $artifact) `
      -Evidence @{ run1_sha256 = $hashRun1; run2_sha256 = $hashRun2 }
  }

  $positiveDiagnosticsText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($positiveDiagnosticsText)) `
    -Id "runtime.positive.diagnostics.empty" `
    -FailureMessage "positive sema fixture emitted diagnostics unexpectedly" `
    -PassMessage "positive sema diagnostics are empty"

  $positiveManifestText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.manifest.json")
  $manifestHasSemaZeroDiag = (
    $positiveManifestText.IndexOf('"semantic": {"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0 -or
    $positiveManifestText.IndexOf('"semantic":{"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $manifestHasSemaZeroDiag `
    -Id "runtime.positive.manifest.semantic_stage_zero_diag" `
    -FailureMessage "positive sema manifest missing semantic stage diagnostics=0 marker" `
    -PassMessage "positive sema manifest reports semantic diagnostics=0"

  Assert-Contract `
    -Condition ($positiveManifestText.IndexOf('"semantic_skipped": false', [System.StringComparison]::Ordinal) -ge 0 -and
                $positiveManifestText.IndexOf('"semantic_surface": {', [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.manifest.semantic_surface_present" `
    -FailureMessage "positive sema manifest missing semantic surface presence markers" `
    -PassMessage "positive sema manifest reports semantic surface presence markers"

  $backendText = (Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.object-backend.txt")).Trim()
  Assert-Contract `
    -Condition ($backendText -eq "clang") `
    -Id "runtime.positive.object_backend.clang" `
    -FailureMessage ("positive sema expected object backend 'clang' but saw '{0}'" -f $backendText) `
    -PassMessage "positive sema uses explicit clang object backend"

  $caseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $positiveFixturePath -Root $repoRoot
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_dir = Get-RepoRelativePath -Path $positiveRun1Dir -Root $repoRoot
      run2_dir = Get-RepoRelativePath -Path $positiveRun2Dir -Root $repoRoot
      artifact_digests = $positiveDigests
    }) | Out-Null

  foreach ($negativeFixturePath in $negativeFixturePaths) {
    $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($negativeFixturePath)
    $expectedCodes = @(Get-ExpectedSemaCodesFromFixture -FixturePath $negativeFixturePath)
    $caseDir = Join-Path (Join-Path $runDir "negative_cases") $fixtureStem
    $run1Dir = Join-Path $caseDir "run1"
    $run2Dir = Join-Path $caseDir "run2"
    New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
    New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
    $run1Log = Join-Path $caseDir "run1.log"
    $run2Log = Join-Path $caseDir "run2.log"

    $argsRun1 = @($negativeFixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $argsRun2 = @($negativeFixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $run1Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $argsRun1 -LogPath $run1Log
    $run2Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $argsRun2 -LogPath $run2Log

    Assert-Contract `
      -Condition ($run1Exit -ne 0 -and $run2Exit -ne 0 -and $run1Exit -eq $run2Exit) `
      -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
      -PassMessage ("negative sema fixture fails deterministically: {0}" -f $fixtureStem) `
      -Evidence @{
        run1_log = Get-RepoRelativePath -Path $run1Log -Root $repoRoot
        run2_log = Get-RepoRelativePath -Path $run2Log -Root $repoRoot
      }

    $diagTxtRun1Path = Join-Path $run1Dir "module.diagnostics.txt"
    $diagTxtRun2Path = Join-Path $run2Dir "module.diagnostics.txt"
    $diagJsonRun1Path = Join-Path $run1Dir "module.diagnostics.json"
    $diagJsonRun2Path = Join-Path $run2Dir "module.diagnostics.json"
    Assert-Contract `
      -Condition ((Test-Path -LiteralPath $diagTxtRun1Path -PathType Leaf) -and (Test-Path -LiteralPath $diagTxtRun2Path -PathType Leaf) -and
                  (Test-Path -LiteralPath $diagJsonRun1Path -PathType Leaf) -and (Test-Path -LiteralPath $diagJsonRun2Path -PathType Leaf)) `
      -Id ("runtime.negative.diagnostics.exists.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture missing diagnostics artifact(s): {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics artifacts present: {0}" -f $fixtureStem)

    $diagTxtRun1Text = Read-NormalizedText -Path $diagTxtRun1Path
    $diagTxtRun2Text = Read-NormalizedText -Path $diagTxtRun2Path
    $diagJsonRun1Text = Read-NormalizedText -Path $diagJsonRun1Path
    $diagJsonRun2Text = Read-NormalizedText -Path $diagJsonRun2Path
    $diagTxtRun1Hash = Get-FileSha256Hex -Path $diagTxtRun1Path
    $diagTxtRun2Hash = Get-FileSha256Hex -Path $diagTxtRun2Path
    $diagJsonRun1Hash = Get-FileSha256Hex -Path $diagJsonRun1Path
    $diagJsonRun2Hash = Get-FileSha256Hex -Path $diagJsonRun2Path
    Assert-Contract `
      -Condition ((-not [string]::IsNullOrWhiteSpace($diagTxtRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagTxtRun2Text)) -and
                  (-not [string]::IsNullOrWhiteSpace($diagJsonRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagJsonRun2Text))) `
      -Id ("runtime.negative.diagnostics.nonempty.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture diagnostics are unexpectedly empty: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics are populated: {0}" -f $fixtureStem)
    Assert-Contract `
      -Condition ($diagTxtRun1Hash -eq $diagTxtRun2Hash) `
      -Id ("runtime.negative.diagnostics_txt.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture diagnostics text hash drift: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics text hash stable: {0}" -f $fixtureStem) `
      -Evidence @{ run1_sha256 = $diagTxtRun1Hash; run2_sha256 = $diagTxtRun2Hash }
    Assert-Contract `
      -Condition ($diagJsonRun1Hash -eq $diagJsonRun2Hash) `
      -Id ("runtime.negative.diagnostics_json.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative sema fixture diagnostics json hash drift: {0}" -f $fixtureStem) `
      -PassMessage ("negative sema fixture diagnostics json hash stable: {0}" -f $fixtureStem) `
      -Evidence @{ run1_sha256 = $diagJsonRun1Hash; run2_sha256 = $diagJsonRun2Hash }

    foreach ($expectedCode in $expectedCodes) {
      Assert-Contract `
        -Condition ($diagTxtRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $diagTxtRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $diagJsonRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and
                    $diagJsonRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
        -Id ("runtime.negative.diagnostics.expected_code.{0}.{1}" -f $fixtureStem, $expectedCode) `
        -FailureMessage ("negative sema fixture diagnostics missing expected code {0}: {1}" -f $expectedCode, $fixtureStem) `
        -PassMessage ("negative sema fixture diagnostics contain expected code {0}: {1}" -f $expectedCode, $fixtureStem)
    }

    foreach ($forbiddenArtifact in @("module.manifest.json", "module.ll", "module.obj", "module.object-backend.txt")) {
      $forbiddenRun1 = Join-Path $run1Dir $forbiddenArtifact
      $forbiddenRun2 = Join-Path $run2Dir $forbiddenArtifact
      $forbiddenPresent = (Test-Path -LiteralPath $forbiddenRun1 -PathType Leaf) -or (Test-Path -LiteralPath $forbiddenRun2 -PathType Leaf)
      Assert-Contract `
        -Condition (-not $forbiddenPresent) `
        -Id ("runtime.negative.artifact.absent.{0}.{1}" -f $fixtureStem, $forbiddenArtifact) `
        -FailureMessage ("negative sema fixture produced forbidden artifact {0}: {1}" -f $forbiddenArtifact, $fixtureStem) `
        -PassMessage ("negative sema fixture keeps {0} absent: {1}" -f $forbiddenArtifact, $fixtureStem)
    }

    $caseResults.Add([pscustomobject]@{
        kind = "negative"
        fixture = Get-RepoRelativePath -Path $negativeFixturePath -Root $repoRoot
        expected_codes = $expectedCodes
        run1_exit = $run1Exit
        run2_exit = $run2Exit
        run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $repoRoot
        run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $repoRoot
        diagnostics_txt_sha256 = $diagTxtRun1Hash
        diagnostics_json_sha256 = $diagJsonRun1Hash
      }) | Out-Null
  }
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
