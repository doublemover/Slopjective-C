$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_lexer_extraction_token_contract_helpers.psm1") -Force -DisableNameChecking

function New-Objc3cLexerExtractionTokenContractConfig {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
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

  [pscustomobject]@{
    RepoRoot = $repoRoot
    SuiteRoot = $suiteRoot
    RunId = $runId
    RunDir = $runDir
    SummaryPath = $summaryPath
    RunDirRel = $runDirRel
    SummaryRel = $summaryRel
    BuildScriptPath = $buildScriptPath
    NativeExePath = $nativeExePath
    NativeExeExplicit = $nativeExeExplicit
    LexerHeaderPath = Join-Path $repoRoot "native/objc3c/src/lex/objc3_lexer.h"
    LexerSourcePath = Join-Path $repoRoot "native/objc3c/src/lex/objc3_lexer.cpp"
    TokenContractHeaderPath = Join-Path $repoRoot "native/objc3c/src/token/objc3_token_contract.h"
    TokenHeaderPath = Join-Path $repoRoot "native/objc3c/src/token/objc3_token.h"
    PipelineStageRunnerPath = Join-Path $repoRoot "native/objc3c/src/pipeline/frontend_pipeline_stage_runner.cpp"
    LexCmakePath = Join-Path $repoRoot "native/objc3c/src/lex/CMakeLists.txt"
    PositiveFixturePath = Join-Path $repoRoot "tests/tooling/fixtures/native/lexer_split/positive_token_contract_smoke.objc3"
    NegativeFixtures = @(
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
  }
}

function Invoke-Objc3cLexerExtractionTokenContractSourceAssertions {
  param([Parameter(Mandatory = $true)]$Config)

  Assert-FileExists -Path $Config.LexerHeaderPath -Id "source.lexer_header.exists" -Description "lexer header"
  Assert-FileExists -Path $Config.LexerSourcePath -Id "source.lexer_source.exists" -Description "lexer source"
  Assert-FileExists -Path $Config.TokenContractHeaderPath -Id "source.token_contract_header.exists" -Description "token contract header"
  Assert-FileExists -Path $Config.TokenHeaderPath -Id "source.token_header.exists" -Description "token header"
  Assert-FileExists -Path $Config.PipelineStageRunnerPath -Id "source.pipeline_stage_runner.exists" -Description "pipeline stage runner source"
  Assert-FileExists -Path $Config.LexCmakePath -Id "source.lex_cmake.exists" -Description "lexer CMake file"
  Assert-FileExists -Path $Config.PositiveFixturePath -Id "fixture.positive.exists" -Description "positive lexer contract fixture"
  foreach ($negativeFixture in $Config.NegativeFixtures) {
    Assert-FileExists `
      -Path $negativeFixture.path `
      -Id ("fixture.negative.exists.{0}" -f [System.IO.Path]::GetFileNameWithoutExtension($negativeFixture.path)) `
      -Description "negative lexer fixture"
  }

  $lexerHeaderText = Read-NormalizedText -Path $Config.LexerHeaderPath
  $lexerSourceText = Read-NormalizedText -Path $Config.LexerSourcePath
  $tokenContractHeaderText = Read-NormalizedText -Path $Config.TokenContractHeaderPath
  $tokenHeaderText = Read-NormalizedText -Path $Config.TokenHeaderPath
  $pipelineStageRunnerText = Read-NormalizedText -Path $Config.PipelineStageRunnerPath
  $lexCmakeText = Read-NormalizedText -Path $Config.LexCmakePath

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
    -Condition (-not (Assert-TokensPresent -Text $tokenHeaderText -RequiredTokens @("using TokenKind = Objc3LexTokenKind;", "using Token = Objc3LexToken;"))) `
    -Id "contract.token_contract_header.canonical_names" `
    -FailureMessage "token contract header still publishes retired short token names" `
    -PassMessage "token contract header publishes only explicit Objc3LexToken names"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexerSourceText -RequiredTokens @("O3L001", "O3L002", "O3L003", "O3L004", "O3C002")) `
    -Id "contract.lexer_diagnostics.codes" `
    -FailureMessage "lexer source missing one or more lexical/canonical diagnostic codes (O3L001-004, O3C002)" `
    -PassMessage "lexer source contains lexical and canonical rejection diagnostic contract codes"

  $removedAliasPatterns = @(
    '(?s)Objc3RejectedCanonicalLiteralKind::Yes:.+?\+\+canonical_literal_rejection_counts_\.yes_literal_sites;',
    '(?s)Objc3RejectedCanonicalLiteralKind::No:.+?\+\+canonical_literal_rejection_counts_\.no_literal_sites;',
    '(?s)Objc3RejectedCanonicalLiteralKind::Null:.+?\+\+canonical_literal_rejection_counts_\.null_literal_sites;'
  )
  $missingRemovedAliasPatterns = New-Object 'System.Collections.Generic.List[string]'
  foreach ($pattern in $removedAliasPatterns) {
    if (-not [regex]::IsMatch($lexerSourceText, $pattern)) {
      $missingRemovedAliasPatterns.Add($pattern) | Out-Null
    }
  }
  Assert-Contract `
    -Condition ($missingRemovedAliasPatterns.Count -eq 0) `
    -Id "contract.lexer_removed_literal_aliases" `
    -FailureMessage "lexer source missing one or more removed literal alias rejection contracts (YES/NO/NULL)" `
    -PassMessage "lexer source records canonical literal rejection counts for YES/NO/NULL" `
    -Evidence @{ missing_patterns = $missingRemovedAliasPatterns }

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexerSourceText -RequiredTokens @("ClassifyObjc3RejectedCanonicalLiteral(ident)", "Objc3RejectedCanonicalLiteralReplacementSpelling(rejected_literal)", "O3C002")) `
    -Id "contract.lexer_removed_literal_aliases.diagnostic_table" `
    -FailureMessage "lexer source missing rejected canonical literal classifier or diagnostic spelling table use" `
    -PassMessage "lexer source rejects removed literal aliases through canonical classifier and diagnostic table"

  $pipelineUsesLexer = (
    $pipelineStageRunnerText.IndexOf('#include "lex/objc3_lexer.h"', [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($pipelineStageRunnerText, '(?m)^\s*Objc3Lexer\s+lexer\s*\(\s*source(?:\s*,\s*[^)]+)?\s*\)\s*;') -and
    $pipelineStageRunnerText.IndexOf("lexer.Run(result.stage_diagnostics.lexer)", [System.StringComparison]::Ordinal) -ge 0 -and
    $pipelineStageRunnerText.IndexOf("lexer.CanonicalLiteralRejectionCounts()", [System.StringComparison]::Ordinal) -ge 0 -and
    [regex]::IsMatch($pipelineStageRunnerText, '(?m)^\s*std::vector<\s*(?:Objc3LexToken|Token)\s*>\s+tokens\s*=\s*lexer\.Run\(')
  )
  Assert-Contract `
    -Condition $pipelineUsesLexer `
    -Id "contract.pipeline.consumes_lexer_module" `
    -FailureMessage "pipeline no longer consumes extracted lexer module surface" `
    -PassMessage "pipeline consumes extracted lexer module surface"

  Assert-Contract `
    -Condition ($pipelineStageRunnerText.IndexOf("class Objc3Lexer {", [System.StringComparison]::Ordinal) -lt 0) `
    -Id "contract.pipeline.no_inline_lexer" `
    -FailureMessage "pipeline source contains inline lexer class definition" `
    -PassMessage "pipeline source does not inline lexer implementation"

  Assert-Contract `
    -Condition (Assert-TokensPresent -Text $lexCmakeText -RequiredTokens @("add_library(objc3c_lex STATIC", "objc3_lexer.cpp")) `
    -Id "contract.cmake.lexer_target_registered" `
    -FailureMessage "lexer CMake missing objc3c_lex target registration for objc3_lexer.cpp" `
    -PassMessage "CMake registers objc3c_lex target with extracted lexer source"
}

function Invoke-Objc3cLexerExtractionTokenContractRuntimeCases {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  if (-not $Config.NativeExeExplicit -and !(Test-Path -LiteralPath $Config.NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $Config.RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $Config.BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $Config.RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $Config.RepoRoot) }
  }

  Assert-FileExists -Path $Config.NativeExePath -Id "runtime.native_executable.exists" -Description "native executable"

  $positiveCaseDir = Join-Path $Config.RunDir "positive_smoke"
  $positiveRun1Dir = Join-Path $positiveCaseDir "run1"
  $positiveRun2Dir = Join-Path $positiveCaseDir "run2"
  New-Item -ItemType Directory -Force -Path $positiveRun1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $positiveRun2Dir | Out-Null
  $positiveRun1Log = Join-Path $positiveCaseDir "run1.log"
  $positiveRun2Log = Join-Path $positiveCaseDir "run2.log"
  $positiveArgsRun1 = @($Config.PositiveFixturePath, "--out-dir", $positiveRun1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveArgsRun2 = @($Config.PositiveFixturePath, "--out-dir", $positiveRun2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $positiveRun1Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $positiveArgsRun1 -LogPath $positiveRun1Log
  $positiveRun2Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $positiveArgsRun2 -LogPath $positiveRun2Log

  Assert-Contract `
    -Condition ($positiveRun1Exit -eq 0 -and $positiveRun2Exit -eq 0) `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage ("positive token smoke compile exits must be zero (run1={0} run2={1})" -f $positiveRun1Exit, $positiveRun2Exit) `
    -PassMessage "positive token smoke fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_log = Get-RepoRelativePath -Path $positiveRun1Log -Root $Config.RepoRoot
      run2_log = Get-RepoRelativePath -Path $positiveRun2Log -Root $Config.RepoRoot
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

  $CaseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $Config.PositiveFixturePath -Root $Config.RepoRoot
      run1_exit = $positiveRun1Exit
      run2_exit = $positiveRun2Exit
      run1_dir = Get-RepoRelativePath -Path $positiveRun1Dir -Root $Config.RepoRoot
      run2_dir = Get-RepoRelativePath -Path $positiveRun2Dir -Root $Config.RepoRoot
      artifact_digests = $positiveDigests
    }) | Out-Null

  foreach ($negativeFixture in $Config.NegativeFixtures) {
    $fixturePath = $negativeFixture.path
    $expectedCode = $negativeFixture.expected_code
    $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($fixturePath)
    $caseDir = Join-Path (Join-Path $Config.RunDir "negative_cases") $fixtureStem
    $run1Dir = Join-Path $caseDir "run1"
    $run2Dir = Join-Path $caseDir "run2"
    New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
    New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
    $run1Log = Join-Path $caseDir "run1.log"
    $run2Log = Join-Path $caseDir "run2.log"

    $argsRun1 = @($fixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $argsRun2 = @($fixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
    $run1Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $argsRun1 -LogPath $run1Log
    $run2Exit = Invoke-LoggedCommand -Command $Config.NativeExePath -Arguments $argsRun2 -LogPath $run2Log

    Assert-Contract `
      -Condition ($run1Exit -ne 0 -and $run2Exit -ne 0 -and $run1Exit -eq $run2Exit) `
      -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
      -FailureMessage ("negative lexer fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
      -PassMessage ("negative lexer fixture fails deterministically: {0}" -f $fixtureStem) `
      -Evidence @{
        run1_log = Get-RepoRelativePath -Path $run1Log -Root $Config.RepoRoot
        run2_log = Get-RepoRelativePath -Path $run2Log -Root $Config.RepoRoot
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

    $CaseResults.Add([pscustomobject]@{
        kind = "negative"
        fixture = Get-RepoRelativePath -Path $fixturePath -Root $Config.RepoRoot
        expected_code = $expectedCode
        run1_exit = $run1Exit
        run2_exit = $run2Exit
        run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $Config.RepoRoot
        run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $Config.RepoRoot
        diagnostics_sha256 = $diagRun1Hash
      }) | Out-Null
  }
}

function Write-Objc3cLexerExtractionTokenContractSummary {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][bool]$HadFatalError,
    [Parameter(Mandatory = $true)][string]$FatalErrorMessage
  )

  $checkArray = $Checks.ToArray()
  $caseResultArray = $CaseResults.ToArray()
  $total = $checkArray.Count
  $passed = @($checkArray | Where-Object { $_.passed }).Count
  $failed = $total - $passed
  $status = if (-not $HadFatalError -and $total -gt 0 -and $failed -eq 0) { "PASS" } else { "FAIL" }

  $nativeExeSummary = "$($Config.NativeExePath)"
  if (Test-Path -LiteralPath $Config.NativeExePath -PathType Leaf) {
    try {
      $nativeExeSummary = Get-RepoRelativePath -Path $Config.NativeExePath -Root $Config.RepoRoot
    }
    catch {
      $nativeExeSummary = "$($Config.NativeExePath)"
    }
  }

  $summary = @{
    contract = "objc3c-lexer-extraction-token-contract-v1"
    run_id = $Config.RunId
    run_dir = $Config.RunDirRel
    summary_path = $Config.SummaryRel
    native_executable = $nativeExeSummary
    status = $status
    total = $total
    passed = $passed
    failed = $failed
    fatal_error = $FatalErrorMessage
    checks = $checkArray
    cases = $caseResultArray
  }
  $summary | ConvertTo-Json -Depth 9 | Set-Content -LiteralPath $Config.SummaryPath -Encoding utf8

  Write-Output ("summary_path: {0}" -f $Config.SummaryRel)
  Write-Output ("status: {0}" -f $status)

  if ($status -ne "PASS") {
    exit 1
  }
}

function Invoke-Objc3cLexerExtractionTokenContract {
  param([Parameter(Mandatory = $true)][string]$ScriptRoot)

  $config = New-Objc3cLexerExtractionTokenContractConfig -ScriptRoot $ScriptRoot
  $checks = New-Object 'System.Collections.Generic.List[object]'
  $caseResults = New-Object 'System.Collections.Generic.List[object]'
  $hadFatalError = $false
  $fatalErrorMessage = ""

  Set-Objc3cLexerExtractionTokenContractContext -Checks $checks -RepoRoot $config.RepoRoot

  New-Item -ItemType Directory -Force -Path $config.RunDir | Out-Null

  Push-Location $config.RepoRoot
  try {
    Invoke-Objc3cLexerExtractionTokenContractSourceAssertions -Config $config
    Invoke-Objc3cLexerExtractionTokenContractRuntimeCases -Config $config -CaseResults $caseResults
  }
  catch {
    $hadFatalError = $true
    $fatalErrorMessage = $_.Exception.Message
    Write-Output ("error: {0}" -f $fatalErrorMessage)
  }
  finally {
    Pop-Location
  }

  Write-Objc3cLexerExtractionTokenContractSummary `
    -Config $config `
    -Checks $checks `
    -CaseResults $caseResults `
    -HadFatalError $hadFatalError `
    -FatalErrorMessage $fatalErrorMessage
}

Export-ModuleMember -Function "Invoke-Objc3cLexerExtractionTokenContract"
