$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lexer-extraction-token-contract"
$configuredRunId = $env:OBJC3C_LEXER_CONTRACT_RUN_ID
$runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$runDirRel = "tmp/artifacts/objc3c-native/lexer-extraction-token-contract/$runId"
$summaryRel = "$runDirRel/summary.json"

$buildScriptPath = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$defaultNativeExePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
$nativeExePath = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExePath } else { $configuredNativeExe }
$nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)

$lexerHeaderPath = Join-Path $repoRoot "native/objc3c/src/lex/objc3_lexer.h"
$lexerSourcePath = Join-Path $repoRoot "native/objc3c/src/lex/objc3_lexer.cpp"
$tokenContractHeaderPath = Join-Path $repoRoot "native/objc3c/src/token/objc3_token_contract.h"
$tokenCompatHeaderPath = Join-Path $repoRoot "native/objc3c/src/token/objc3_token.h"
$pipelineSourcePath = Join-Path $repoRoot "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
$cmakePath = Join-Path $repoRoot "native/objc3c/CMakeLists.txt"

$positiveFixturePath = Join-Path $repoRoot "tests/tooling/fixtures/native/lexer_split/positive_token_contract_smoke.objc3"
$negativeFixtures = @(
  [pscustomobject]@{
    path = (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_nil_literal_lexer_unexpected_character.objc3")
    expected_code = "O3L001"
  },
  [pscustomobject]@{
    path = (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_block_comment_lexer_unterminated.objc3")
    expected_code = "O3L002"
  },
  [pscustomobject]@{
    path = (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_block_comment_lexer_nested.objc3")
    expected_code = "O3L003"
  },
  [pscustomobject]@{
    path = (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative/negative_block_comment_lexer_stray_terminator.objc3")
    expected_code = "O3L004"
  }
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
    throw "lexer extraction contract FAIL: $FailureMessage"
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

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

Push-Location $repoRoot
try {
  Assert-FileExists -Path $lexerHeaderPath -Id "source.lexer_header.exists" -Description "lexer header"
  Assert-FileExists -Path $lexerSourcePath -Id "source.lexer_source.exists" -Description "lexer source"
  Assert-FileExists -Path $tokenContractHeaderPath -Id "source.token_contract_header.exists" -Description "token contract header"
  Assert-FileExists -Path $tokenCompatHeaderPath -Id "source.token_compat_header.exists" -Description "token compatibility header"
  Assert-FileExists -Path $pipelineSourcePath -Id "source.pipeline_source.exists" -Description "pipeline source"
  Assert-FileExists -Path $cmakePath -Id "source.cmake.exists" -Description "native CMake file"
  Assert-FileExists -Path $positiveFixturePath -Id "fixture.positive.exists" -Description "positive lexer contract fixture"
  foreach ($negativeFixture in $negativeFixtures) {
    Assert-FileExists `
      -Path $negativeFixture.path `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixture.path)) `
      -Description "negative lexer fixture"
  }

  $lexerHeaderText = Read-NormalizedText -Path $lexerHeaderPath
  $lexerSourceText = Read-NormalizedText -Path $lexerSourcePath
  $tokenContractHeaderText = Read-NormalizedText -Path $tokenContractHeaderPath
  $tokenCompatHeaderText = Read-NormalizedText -Path $tokenCompatHeaderPath
  $pipelineSourceText = Read-NormalizedText -Path $pipelineSourcePath
  $cmakeText = Read-NormalizedText -Path $cmakePath

  $lexerHeaderRunSignatureValid = [regex]::IsMatch(
    $lexerHeaderText,
    '(?s)class\s+Objc3Lexer\s*\{.+?std::vector<\s*(?:Objc3LexToken|Token)\s*>\s+Run\s*\(\s*std::vector<std::string>\s*&\s*diagnostics\s*\)\s*;'
  )
  Assert-Contract `
    -Condition $lexerHeaderRunSignatureValid `
    -Id "contract.lexer_header.surface" `
    -FailureMessage "lexer header missing Objc3Lexer Run diagnostics surface" `
    -PassMessage "lexer header exposes Objc3Lexer Run diagnostics contract"

  $requiredTokenKinds = @(
    "KwModule", "KwLet", "KwFn", "KwPure", "KwExtern", "KwReturn",
    "KwIf", "KwElse", "KwDo", "KwFor", "KwSwitch", "KwCase", "KwDefault",
    "KwWhile", "KwBreak", "KwContinue", "KwI32", "KwBool", "KwTrue",
    "KwFalse", "KwNil", "LessLessEqual", "GreaterGreaterEqual",
    "PlusPlus", "MinusMinus", "PercentEqual", "Question", "Tilde"
  )
  $missingTokenKinds = @($requiredTokenKinds | Where-Object { $tokenContractHeaderText -notmatch ("(?<![A-Za-z0-9_])" + [regex]::Escape($_) + "(?![A-Za-z0-9_])") })
  Assert-Contract `
    -Condition ($missingTokenKinds.Count -eq 0) `
    -Id "contract.token_contract_header.required_kinds" `
    -FailureMessage ("token contract header missing required token kinds: {0}" -f ($missingTokenKinds -join ",")) `
    -PassMessage "token contract header includes required token contract kinds" `
    -Evidence @{ missing = $missingTokenKinds }

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $tokenCompatHeaderText -RequiredTokens @("using TokenKind = Objc3LexTokenKind;", "using Token = Objc3LexToken;")) `
    -Id "contract.token_compat_header.aliases" `
    -FailureMessage "token compatibility header missing backward-compatible aliases to Objc3LexTokenKind/Objc3LexToken" `
    -PassMessage "token compatibility header preserves backward-compatible aliases"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexerSourceText -RequiredTokens @("O3L001", "O3L002", "O3L003", "O3L004")) `
    -Id "contract.lexer_diagnostics.codes" `
    -FailureMessage "lexer source missing one or more lexical diagnostic codes (O3L001-004)" `
    -PassMessage "lexer source contains lexical diagnostic contract codes O3L001-004"

  $aliasPatterns = @(
    '(?s)ident\s*==\s*"YES".+?TokenKind::KwTrue',
    '(?s)ident\s*==\s*"NO".+?TokenKind::KwFalse',
    '(?s)ident\s*==\s*"NULL".+?TokenKind::KwNil'
  )
  $missingAliases = New-Object 'System.Collections.Generic.List[string]'
  foreach ($pattern in $aliasPatterns) {
    if (-not [regex]::IsMatch($lexerSourceText, $pattern)) {
      $missingAliases.Add($pattern) | Out-Null
    }
  }
  Assert-Contract `
    -Condition ($missingAliases.Count -eq 0) `
    -Id "contract.lexer_keyword_aliases" `
    -FailureMessage "lexer source missing one or more keyword alias contracts (YES/NO/NULL)" `
    -PassMessage "lexer source preserves YES/NO/NULL token alias contracts" `
    -Evidence @{ missing_patterns = $missingAliases }

  $pipelineUsesLexer = (
    $pipelineSourceText.IndexOf('#include "lex/objc3_lexer.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("Objc3Lexer lexer(source);", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineSourceText.IndexOf("lexer.Run(result.stage_diagnostics.lexer)", [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($pipelineSourceText, '(?m)^\s*std::vector<\s*(?:Objc3LexToken|Token)\s*>\s+tokens\s*=\s*lexer\.Run\(')
  )
  Assert-Contract `
    -Condition $pipelineUsesLexer `
    -Id "contract.pipeline.consumes_lexer_module" `
    -FailureMessage "pipeline no longer consumes extracted lexer module surface" `
    -PassMessage "pipeline consumes extracted lexer module surface"

  Assert-Contract `
    -Condition ($pipelineSourceText.IndexOf("class Objc3Lexer {", [System.StringComparison]::Ordinal) -lt 0) `
    -Id "contract.pipeline.no_inline_lexer" `
    -FailureMessage "pipeline source contains inline lexer class definition" `
    -PassMessage "pipeline source does not inline lexer implementation"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $cmakeText -RequiredTokens @("add_library(objc3c_lex STATIC", "src/lex/objc3_lexer.cpp")) `
    -Id "contract.cmake.lexer_target_registered" `
    -FailureMessage "CMake missing objc3c_lex target registration for src/lex/objc3_lexer.cpp" `
    -PassMessage "CMake registers objc3c_lex target with extracted lexer source"

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
    -FailureMessage ("positive token smoke compile exits must be zero (run1={0} run2={1})" -f $positiveRun1Exit, $positiveRun2Exit) `
    -PassMessage "positive token smoke fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_log = Get-RepoRelativePath -Path $positiveRun1Log -Root $repoRoot
      run2_log = Get-RepoRelativePath -Path $positiveRun2Log -Root $repoRoot
    }

  $positiveArtifacts = @("module.manifest.json", "module.diagnostics.txt", "module.ll", "module.obj", "module.object-backend.txt")
  $positiveDigests = @{}
  foreach ($artifact in $positiveArtifacts) {
    $artifactRun1 = Join-Path $positiveRun1Dir $artifact
    $artifactRun2 = Join-Path $positiveRun2Dir $artifact
    $existsRun1 = Test-Path -LiteralPath $artifactRun1 -PathType Leaf
    $existsRun2 = Test-Path -LiteralPath $artifactRun2 -PathType Leaf
    Assert-Contract `
      -Condition ($existsRun1 -and $existsRun2) `
      -Id ("runtime.positive.artifact.exists.{0}" -f $artifact) `
      -FailureMessage ("positive smoke missing artifact across replay: {0}" -f $artifact) `
      -PassMessage ("positive smoke artifact present across replay: {0}" -f $artifact)

    if ($artifact -eq "module.obj") {
      $objRun1Bytes = (Get-Item -LiteralPath $artifactRun1).Length
      $objRun2Bytes = (Get-Item -LiteralPath $artifactRun2).Length
      Assert-Contract `
        -Condition ($objRun1Bytes -gt 0 -and $objRun2Bytes -gt 0) `
        -Id "runtime.positive.artifact.nonempty.module.obj" `
        -FailureMessage "positive smoke module.obj is empty in one or more runs" `
        -PassMessage "positive smoke module.obj is non-empty across replay" `
        -Evidence @{ run1_bytes = $objRun1Bytes; run2_bytes = $objRun2Bytes }
    }

    $hashRun1 = Get-FileSha256Hex -Path $artifactRun1
    $hashRun2 = Get-FileSha256Hex -Path $artifactRun2
    $positiveDigests[$artifact] = [ordered]@{
      run1_sha256 = $hashRun1
      run2_sha256 = $hashRun2
      deterministic = ($hashRun1 -eq $hashRun2)
    }
    if ($artifact -eq "module.obj") {
      # COFF object payloads can contain non-deterministic metadata; keep this informational.
      Add-Check `
        -Id "runtime.positive.artifact.hash_recorded.module.obj" `
        -Passed $true `
        -Detail "positive smoke module.obj hashes recorded; determinism is not enforced for this artifact" `
        -Evidence @{ run1_sha256 = $hashRun1; run2_sha256 = $hashRun2 }
      continue
    }
    Assert-Contract `
      -Condition ($hashRun1 -eq $hashRun2) `
      -Id ("runtime.positive.artifact.deterministic_sha256.{0}" -f $artifact) `
      -FailureMessage ("positive smoke artifact hash drift detected for {0}" -f $artifact) `
      -PassMessage ("positive smoke artifact hash stable for {0}" -f $artifact) `
      -Evidence @{ run1_sha256 = $hashRun1; run2_sha256 = $hashRun2 }
  }

  $positiveDiagnosticsText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($positiveDiagnosticsText)) `
    -Id "runtime.positive.diagnostics.empty" `
    -FailureMessage "positive token smoke emitted diagnostics unexpectedly" `
    -PassMessage "positive token smoke diagnostics are empty"

  $positiveLlText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.ll")
  Assert-Contract `
    -Condition ($positiveLlText.IndexOf("define i32 @objc3c_entry", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.ll.contains_objc3c_entry" `
    -FailureMessage "positive token smoke LLVM IR missing objc3c_entry" `
    -PassMessage "positive token smoke LLVM IR contains objc3c_entry"

  $positiveManifestText = Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.manifest.json")
  Assert-Contract `
    -Condition ($positiveManifestText.IndexOf('"lexer":{"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0 -or $positiveManifestText.IndexOf('"lexer": {"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.manifest.lexer_stage_zero_diag" `
    -FailureMessage "positive token smoke manifest missing lexer stage diagnostics=0 contract marker" `
    -PassMessage "positive token smoke manifest reports lexer diagnostics=0"

  $backendText = (Read-NormalizedText -Path (Join-Path $positiveRun1Dir "module.object-backend.txt")).Trim()
  Assert-Contract `
    -Condition ($backendText -eq "clang") `
    -Id "runtime.positive.object_backend.clang" `
    -FailureMessage ("positive token smoke expected object backend 'clang' but saw '{0}'" -f $backendText) `
    -PassMessage "positive token smoke uses explicit clang object backend"

  $caseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $positiveFixturePath -Root $repoRoot
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_dir = Get-RepoRelativePath -Path $positiveRun1Dir -Root $repoRoot
      run2_dir = Get-RepoRelativePath -Path $positiveRun2Dir -Root $repoRoot
      artifact_digests = $positiveDigests
    }) | Out-Null

  foreach ($negativeFixture in $negativeFixtures) {
    $fixturePath = $negativeFixture.path
    $expectedCode = $negativeFixture.expected_code
    $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($fixturePath)
    $caseDir = Join-Path (Join-Path $runDir "negative_cases") $fixtureStem
    $run1Dir = Join-Path $caseDir "run1"
    $run2Dir = Join-Path $caseDir "run2"
    New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
    New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
    $run1Log = Join-Path $caseDir "run1.log"
    $run2Log = Join-Path $caseDir "run2.log"

    $argsRun1 = @($fixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $argsRun2 = @($fixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $run1Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $argsRun1 -LogPath $run1Log
    $run2Exit = Invoke-LoggedCommand -Command $nativeExePath -Arguments $argsRun2 -LogPath $run2Log

    Assert-Contract `
      -Condition ($run1Exit -ne 0 -and $run2Exit -ne 0 -and $run1Exit -eq $run2Exit) `
      -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
      -FailureMessage ("negative lexer fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
      -PassMessage ("negative lexer fixture fails deterministically: {0}" -f $fixtureStem) `
      -Evidence @{
        run1_log = Get-RepoRelativePath -Path $run1Log -Root $repoRoot
        run2_log = Get-RepoRelativePath -Path $run2Log -Root $repoRoot
      }

    $diagRun1Path = Join-Path $run1Dir "module.diagnostics.txt"
    $diagRun2Path = Join-Path $run2Dir "module.diagnostics.txt"
    Assert-Contract `
      -Condition ((Test-Path -LiteralPath $diagRun1Path -PathType Leaf) -and (Test-Path -LiteralPath $diagRun2Path -PathType Leaf)) `
      -Id ("runtime.negative.diagnostics.exists.{0}" -f $fixtureStem) `
      -FailureMessage ("negative lexer fixture missing diagnostics artifact(s): {0}" -f $fixtureStem) `
      -PassMessage ("negative lexer fixture diagnostics artifacts present: {0}" -f $fixtureStem)

    $diagRun1Text = Read-NormalizedText -Path $diagRun1Path
    $diagRun2Text = Read-NormalizedText -Path $diagRun2Path
    $diagRun1Hash = Get-FileSha256Hex -Path $diagRun1Path
    $diagRun2Hash = Get-FileSha256Hex -Path $diagRun2Path
    Assert-Contract `
      -Condition ((-not [string]::IsNullOrWhiteSpace($diagRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagRun2Text))) `
      -Id ("runtime.negative.diagnostics.nonempty.{0}" -f $fixtureStem) `
      -FailureMessage ("negative lexer fixture diagnostics are unexpectedly empty: {0}" -f $fixtureStem) `
      -PassMessage ("negative lexer fixture diagnostics are populated: {0}" -f $fixtureStem)
    Assert-Contract `
      -Condition ($diagRun1Hash -eq $diagRun2Hash) `
      -Id ("runtime.negative.diagnostics.deterministic_sha256.{0}" -f $fixtureStem) `
      -FailureMessage ("negative lexer fixture diagnostics hash drift: {0}" -f $fixtureStem) `
      -PassMessage ("negative lexer fixture diagnostics hash stable: {0}" -f $fixtureStem) `
      -Evidence @{ run1_sha256 = $diagRun1Hash; run2_sha256 = $diagRun2Hash }
    Assert-Contract `
      -Condition ($diagRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and $diagRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
      -Id ("runtime.negative.diagnostics.expected_code.{0}" -f $fixtureStem) `
      -FailureMessage ("negative lexer fixture diagnostics missing expected code {0}: {1}" -f $expectedCode, $fixtureStem) `
      -PassMessage ("negative lexer fixture diagnostics contain expected code {0}: {1}" -f $expectedCode, $fixtureStem)

    foreach ($forbiddenArtifact in @("module.manifest.json", "module.ll", "module.obj", "module.object-backend.txt")) {
      $forbiddenRun1 = Join-Path $run1Dir $forbiddenArtifact
      $forbiddenRun2 = Join-Path $run2Dir $forbiddenArtifact
      $forbiddenPresent = (Test-Path -LiteralPath $forbiddenRun1 -PathType Leaf) -or (Test-Path -LiteralPath $forbiddenRun2 -PathType Leaf)
      Assert-Contract `
        -Condition (-not $forbiddenPresent) `
        -Id ("runtime.negative.artifact.absent.{0}.{1}" -f $fixtureStem, $forbiddenArtifact) `
        -FailureMessage ("negative lexer fixture produced forbidden artifact {0}: {1}" -f $forbiddenArtifact, $fixtureStem) `
        -PassMessage ("negative lexer fixture keeps {0} absent: {1}" -f $forbiddenArtifact, $fixtureStem)
    }

    $caseResults.Add([pscustomobject]@{
        kind = "negative"
        fixture = Get-RepoRelativePath -Path $fixturePath -Root $repoRoot
        expected_code = $expectedCode
        run1_exit = $run1Exit
        run2_exit = $run2Exit
        run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $repoRoot
        run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $repoRoot
        diagnostics_sha256 = $diagRun1Hash
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
  contract = "objc3c-lexer-extraction-token-contract-v1"
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
$summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output ("summary_path: {0}" -f $summaryRel)
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
