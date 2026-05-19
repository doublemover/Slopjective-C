function Invoke-ParserAstBuilderNegativeRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$NegativeFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($NegativeFixturePath)
  $expectedCodes = @(Get-ExpectedParserCodesFromFixture -FixturePath $NegativeFixturePath)
  $caseDir = Join-Path (Join-Path $RunDir "negative_cases") $fixtureStem
  $run1Dir = Join-Path $caseDir "run1"
  $run2Dir = Join-Path $caseDir "run2"
  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
  $run1Log = Join-Path $caseDir "run1.log"
  $run2Log = Join-Path $caseDir "run2.log"

  $replay = Invoke-ParserAstBuilderNativeReplay `
    -NativeExePath $NativeExePath `
    -FixturePath $NegativeFixturePath `
    -Run1Dir $run1Dir `
    -Run2Dir $run2Dir `
    -Run1Log $run1Log `
    -Run2Log $run2Log

  Assert-ParserAstBuilderNegativeReplay `
    -RepoRoot $RepoRoot `
    -FixtureStem $fixtureStem `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Run1Log $run1Log `
    -Run2Log $run2Log

  $diagRun1Path = Join-Path $run1Dir "module.diagnostics.txt"
  $diagRun2Path = Join-Path $run2Dir "module.diagnostics.txt"
  $diagnosticsSha256 = Assert-ParserAstBuilderNegativeDiagnostics `
    -FixtureStem $fixtureStem `
    -ExpectedCodes $expectedCodes `
    -Run1Path $diagRun1Path `
    -Run2Path $diagRun2Path

  Assert-ParserAstBuilderNegativeForbiddenArtifacts `
    -FixtureStem $fixtureStem `
    -Run1Dir $run1Dir `
    -Run2Dir $run2Dir

  Add-ParserAstBuilderNegativeCaseResult `
    -CaseResults $CaseResults `
    -RepoRoot $RepoRoot `
    -FixturePath $NegativeFixturePath `
    -ExpectedCodes $expectedCodes `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Run1Dir $run1Dir `
    -Run2Dir $run2Dir `
    -DiagnosticsSha256 $diagnosticsSha256
}

function Assert-ParserAstBuilderNegativeReplay {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$FixtureStem,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Run1Log,
    [Parameter(Mandatory = $true)][string]$Run2Log
  )

  Assert-Contract `
    -Condition ($Run1Exit -ne 0 -and $Run2Exit -ne 0 -and $Run1Exit -eq $Run2Exit) `
    -Id ("runtime.negative.exit_codes.{0}" -f $FixtureStem) `
    -FailureMessage ("negative parser fixture must fail deterministically ({0}: run1={1} run2={2})" -f $FixtureStem, $Run1Exit, $Run2Exit) `
    -PassMessage ("negative parser fixture fails deterministically: {0}" -f $FixtureStem) `
    -Evidence @{
      run1_log = Get-RepoRelativePath -Path $Run1Log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $Run2Log -Root $RepoRoot
    }
}

function Assert-ParserAstBuilderNegativeDiagnostics {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureStem,
    [Parameter(Mandatory = $true)][string[]]$ExpectedCodes,
    [Parameter(Mandatory = $true)][string]$Run1Path,
    [Parameter(Mandatory = $true)][string]$Run2Path
  )

  Assert-Contract `
    -Condition ((Test-Path -LiteralPath $Run1Path -PathType Leaf) -and (Test-Path -LiteralPath $Run2Path -PathType Leaf)) `
    -Id ("runtime.negative.diagnostics.exists.{0}" -f $FixtureStem) `
    -FailureMessage ("negative parser fixture missing diagnostics artifact(s): {0}" -f $FixtureStem) `
    -PassMessage ("negative parser fixture diagnostics artifacts present: {0}" -f $FixtureStem)

  $diagRun1Text = Read-NormalizedText -Path $Run1Path
  $diagRun2Text = Read-NormalizedText -Path $Run2Path
  $diagRun1Hash = Get-FileSha256Hex -Path $Run1Path
  $diagRun2Hash = Get-FileSha256Hex -Path $Run2Path
  Assert-Contract `
    -Condition ((-not [string]::IsNullOrWhiteSpace($diagRun1Text)) -and (-not [string]::IsNullOrWhiteSpace($diagRun2Text))) `
    -Id ("runtime.negative.diagnostics.nonempty.{0}" -f $FixtureStem) `
    -FailureMessage ("negative parser fixture diagnostics are unexpectedly empty: {0}" -f $FixtureStem) `
    -PassMessage ("negative parser fixture diagnostics are populated: {0}" -f $FixtureStem)
  Assert-Contract `
    -Condition ($diagRun1Hash -eq $diagRun2Hash) `
    -Id ("runtime.negative.diagnostics.deterministic_sha256.{0}" -f $FixtureStem) `
    -FailureMessage ("negative parser fixture diagnostics hash drift: {0}" -f $FixtureStem) `
    -PassMessage ("negative parser fixture diagnostics hash stable: {0}" -f $FixtureStem) `
    -Evidence @{ run1_sha256 = $diagRun1Hash; run2_sha256 = $diagRun2Hash }

  foreach ($expectedCode in $ExpectedCodes) {
    Assert-Contract `
      -Condition ($diagRun1Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0 -and $diagRun2Text.IndexOf($expectedCode, [System.StringComparison]::Ordinal) -ge 0) `
      -Id ("runtime.negative.diagnostics.expected_code.{0}.{1}" -f $FixtureStem, $expectedCode) `
      -FailureMessage ("negative parser fixture diagnostics missing expected code {0}: {1}" -f $expectedCode, $FixtureStem) `
      -PassMessage ("negative parser fixture diagnostics contain expected code {0}: {1}" -f $expectedCode, $FixtureStem)
  }

  return $diagRun1Hash
}

function Assert-ParserAstBuilderNegativeForbiddenArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureStem,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir
  )

  foreach ($forbiddenArtifact in @("module.manifest.json", "module.ll", "module.obj", "module.object-backend.txt")) {
    $forbiddenRun1 = Join-Path $Run1Dir $forbiddenArtifact
    $forbiddenRun2 = Join-Path $Run2Dir $forbiddenArtifact
    $forbiddenPresent = (Test-Path -LiteralPath $forbiddenRun1 -PathType Leaf) -or (Test-Path -LiteralPath $forbiddenRun2 -PathType Leaf)
    Assert-Contract `
      -Condition (-not $forbiddenPresent) `
      -Id ("runtime.negative.artifact.absent.{0}.{1}" -f $FixtureStem, $forbiddenArtifact) `
      -FailureMessage ("negative parser fixture produced forbidden artifact {0}: {1}" -f $forbiddenArtifact, $FixtureStem) `
      -PassMessage ("negative parser fixture keeps {0} absent: {1}" -f $forbiddenArtifact, $FixtureStem)
  }
}
