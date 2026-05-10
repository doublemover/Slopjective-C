$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function New-Objc3cLexerExtractionNegativeFixture {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath,
    [Parameter(Mandatory = $true)][string]$ExpectedCode
  )

  [pscustomobject]@{
    path = Join-Path $RepoRoot $RelativePath
    expected_code = $ExpectedCode
  }
}

function Get-Objc3cLexerExtractionNegativeFixtureCatalog {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  @(
    New-Objc3cLexerExtractionNegativeFixture `
      -RepoRoot $RepoRoot `
      -RelativePath "tests/tooling/fixtures/native/recovery/negative/negative_nil_literal_lexer_unexpected_character.objc3" `
      -ExpectedCode "O3L001"
    New-Objc3cLexerExtractionNegativeFixture `
      -RepoRoot $RepoRoot `
      -RelativePath "tests/tooling/fixtures/native/recovery/negative/negative_block_comment_lexer_unterminated.objc3" `
      -ExpectedCode "O3L002"
    New-Objc3cLexerExtractionNegativeFixture `
      -RepoRoot $RepoRoot `
      -RelativePath "tests/tooling/fixtures/native/recovery/negative/negative_block_comment_lexer_nested.objc3" `
      -ExpectedCode "O3L003"
    New-Objc3cLexerExtractionNegativeFixture `
      -RepoRoot $RepoRoot `
      -RelativePath "tests/tooling/fixtures/native/recovery/negative/negative_block_comment_lexer_stray_terminator.objc3" `
      -ExpectedCode "O3L004"
  )
}

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
    PositiveArtifacts = @("module.manifest.json", "module.diagnostics.txt", "module.ll", "module.obj", "module.object-backend.txt")
    NegativeFixtures = @(Get-Objc3cLexerExtractionNegativeFixtureCatalog -RepoRoot $repoRoot)
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cLexerExtractionNegativeFixtureCatalog",
  "New-Objc3cLexerExtractionTokenContractConfig"
)
