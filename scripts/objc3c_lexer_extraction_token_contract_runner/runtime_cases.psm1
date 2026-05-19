$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "../objc3c_lexer_extraction_token_contract_helpers.psm1") -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "compiler_invocation.psm1") -Force -DisableNameChecking

function Add-Objc3cLexerExtractionPositiveRuntimeCaseResult {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)]$Replay,
    [Parameter(Mandatory = $true)]$ArtifactDigests
  )

  $CaseResults.Add([pscustomobject]@{
      kind = "positive"
      fixture = Get-RepoRelativePath -Path $Config.PositiveFixturePath -Root $Config.RepoRoot
      run1_exit = $Replay.Run1Exit
      run2_exit = $Replay.Run2Exit
      run1_dir = Get-RepoRelativePath -Path $Replay.Run1Dir -Root $Config.RepoRoot
      run2_dir = Get-RepoRelativePath -Path $Replay.Run2Dir -Root $Config.RepoRoot
      artifact_digests = $ArtifactDigests
    }) | Out-Null
}

function Invoke-Objc3cLexerExtractionPositiveRuntimeCase {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $positiveCaseDir = Join-Path $Config.RunDir "positive_smoke"
  $layout = New-Objc3cLexerExtractionReplayLayout -CaseDir $positiveCaseDir
  $replay = Invoke-Objc3cLexerExtractionCompileReplay -Config $Config -FixturePath $Config.PositiveFixturePath -Layout $layout

  Assert-Contract `
    -Condition ($replay.Run1Exit -eq 0 -and $replay.Run2Exit -eq 0) `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage ("positive token smoke compile exits must be zero (run1={0} run2={1})" -f $replay.Run1Exit, $replay.Run2Exit) `
    -PassMessage "positive token smoke fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $replay.Run1Exit
      run2_exit = $replay.Run2Exit
      run1_log = Get-RepoRelativePath -Path $replay.Run1Log -Root $Config.RepoRoot
      run2_log = Get-RepoRelativePath -Path $replay.Run2Log -Root $Config.RepoRoot
    }

  $positiveDigests = @{}
  foreach ($artifact in $Config.PositiveArtifacts) {
    $artifactRun1 = Join-Path $replay.Run1Dir $artifact
    $artifactRun2 = Join-Path $replay.Run2Dir $artifact
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

  $positiveDiagnosticsText = Read-NormalizedText -Path (Join-Path $replay.Run1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($positiveDiagnosticsText)) `
    -Id "runtime.positive.diagnostics.empty" `
    -FailureMessage "positive token smoke emitted diagnostics unexpectedly" `
    -PassMessage "positive token smoke diagnostics are empty"

  $positiveLlText = Read-NormalizedText -Path (Join-Path $replay.Run1Dir "module.ll")
  Assert-Contract `
    -Condition ($positiveLlText.IndexOf("define i32 @objc3c_entry", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.ll.contains_objc3c_entry" `
    -FailureMessage "positive token smoke LLVM IR missing objc3c_entry" `
    -PassMessage "positive token smoke LLVM IR contains objc3c_entry"

  $positiveManifestText = Read-NormalizedText -Path (Join-Path $replay.Run1Dir "module.manifest.json")
  Assert-Contract `
    -Condition ($positiveManifestText.IndexOf('"lexer":{"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0 -or $positiveManifestText.IndexOf('"lexer": {"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.manifest.lexer_stage_zero_diag" `
    -FailureMessage "positive token smoke manifest missing lexer stage diagnostics=0 contract marker" `
    -PassMessage "positive token smoke manifest reports lexer diagnostics=0"

  $backendText = (Read-NormalizedText -Path (Join-Path $replay.Run1Dir "module.object-backend.txt")).Trim()
  Assert-Contract `
    -Condition ($backendText -eq "clang") `
    -Id "runtime.positive.object_backend.clang" `
    -FailureMessage ("positive token smoke expected object backend 'clang' but saw '{0}'" -f $backendText) `
    -PassMessage "positive token smoke uses explicit clang object backend"

  Add-Objc3cLexerExtractionPositiveRuntimeCaseResult `
    -Config $Config `
    -CaseResults $CaseResults `
    -Replay $replay `
    -ArtifactDigests $positiveDigests
}

function Invoke-Objc3cLexerExtractionNegativeRuntimeCase {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)]$Fixture
  )

  $fixturePath = $Fixture.path
  $expectedCode = $Fixture.expected_code
  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($fixturePath)
  $caseDir = Join-Path (Join-Path $Config.RunDir "negative_cases") $fixtureStem
  $layout = New-Objc3cLexerExtractionReplayLayout -CaseDir $caseDir
  $replay = Invoke-Objc3cLexerExtractionCompileReplay -Config $Config -FixturePath $fixturePath -Layout $layout

  Assert-Contract `
    -Condition ($replay.Run1Exit -ne 0 -and $replay.Run2Exit -ne 0 -and $replay.Run1Exit -eq $replay.Run2Exit) `
    -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
    -FailureMessage ("negative lexer fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $replay.Run1Exit, $replay.Run2Exit) `
    -PassMessage ("negative lexer fixture fails deterministically: {0}" -f $fixtureStem) `
    -Evidence @{
      run1_log = Get-RepoRelativePath -Path $replay.Run1Log -Root $Config.RepoRoot
      run2_log = Get-RepoRelativePath -Path $replay.Run2Log -Root $Config.RepoRoot
    }

  $diagRun1Path = Join-Path $replay.Run1Dir "module.diagnostics.txt"
  $diagRun2Path = Join-Path $replay.Run2Dir "module.diagnostics.txt"
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
    $forbiddenRun1 = Join-Path $replay.Run1Dir $forbiddenArtifact
    $forbiddenRun2 = Join-Path $replay.Run2Dir $forbiddenArtifact
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
      run1_exit = $replay.Run1Exit
      run2_exit = $replay.Run2Exit
      run1_dir = Get-RepoRelativePath -Path $replay.Run1Dir -Root $Config.RepoRoot
      run2_dir = Get-RepoRelativePath -Path $replay.Run2Dir -Root $Config.RepoRoot
      diagnostics_sha256 = $diagRun1Hash
    }) | Out-Null
}

function Invoke-Objc3cLexerExtractionTokenContractRuntimeCases {
  param(
    [Parameter(Mandatory = $true)]$Config,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  Assert-Objc3cLexerExtractionNativeExecutableReady -Config $Config
  Invoke-Objc3cLexerExtractionPositiveRuntimeCase -Config $Config -CaseResults $CaseResults

  foreach ($negativeFixture in $Config.NegativeFixtures) {
    Invoke-Objc3cLexerExtractionNegativeRuntimeCase -Config $Config -CaseResults $CaseResults -Fixture $negativeFixture
  }
}

Export-ModuleMember -Function "Invoke-Objc3cLexerExtractionTokenContractRuntimeCases"
