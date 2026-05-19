function Invoke-SemaPassManagerNegativeClangRuntimeCases {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string[]]$NegativeFixturePaths,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  foreach ($negativeFixturePath in $NegativeFixturePaths) {
    Invoke-SemaPassManagerNegativeClangRuntimeCase `
      -RepoRoot $RepoRoot `
      -RunDir $RunDir `
      -NativeExePath $NativeExePath `
      -NegativeFixturePath $negativeFixturePath `
      -CaseResults $CaseResults
  }
}

function Invoke-SemaPassManagerNegativeClangRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$NegativeFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $fixtureStem = [System.IO.Path]::GetFileNameWithoutExtension($NegativeFixturePath)
  $expectedCodes = @(Get-ExpectedSemaCodesFromFixture -FixturePath $NegativeFixturePath)
  $caseDir = Join-Path (Join-Path $RunDir "negative_cases") $fixtureStem
  $run1Dir = Join-Path $caseDir "run1"
  $run2Dir = Join-Path $caseDir "run2"
  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null
  $run1Log = Join-Path $caseDir "run1.log"
  $run2Log = Join-Path $caseDir "run2.log"

  $argsRun1 = @($NegativeFixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $argsRun2 = @($NegativeFixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $run1Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun1 -LogPath $run1Log
  $run2Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun2 -LogPath $run2Log

  Assert-Contract `
    -Condition ($run1Exit -ne 0 -and $run2Exit -ne 0 -and $run1Exit -eq $run2Exit) `
    -Id ("runtime.negative.exit_codes.{0}" -f $fixtureStem) `
    -FailureMessage ("negative sema fixture must fail deterministically ({0}: run1={1} run2={2})" -f $fixtureStem, $run1Exit, $run2Exit) `
    -PassMessage ("negative sema fixture fails deterministically: {0}" -f $fixtureStem) `
    -Evidence @{
      run1_log = Get-RepoRelativePath -Path $run1Log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $run2Log -Root $RepoRoot
    }

  $run1Diagnostics = Get-SemaPassManagerNegativeRuntimeDiagnosticsPaths -OutputDir $run1Dir
  $run2Diagnostics = Get-SemaPassManagerNegativeRuntimeDiagnosticsPaths -OutputDir $run2Dir
  Assert-SemaPassManagerNegativeRuntimeDiagnosticsExist `
    -FixtureStem $fixtureStem `
    -ContractId "runtime.negative.diagnostics.exists.{0}" `
    -FailureMessage "negative sema fixture missing diagnostics artifact(s): {0}" `
    -PassMessage "negative sema fixture diagnostics artifacts present: {0}" `
    -LeftDiagnostics $run1Diagnostics `
    -RightDiagnostics $run2Diagnostics

  $diagTxtRun1Text = Read-NormalizedText -Path $run1Diagnostics.Txt
  $diagTxtRun2Text = Read-NormalizedText -Path $run2Diagnostics.Txt
  $diagJsonRun1Text = Read-NormalizedText -Path $run1Diagnostics.Json
  $diagJsonRun2Text = Read-NormalizedText -Path $run2Diagnostics.Json
  $diagTxtRun1Hash = Get-FileSha256Hex -Path $run1Diagnostics.Txt
  $diagTxtRun2Hash = Get-FileSha256Hex -Path $run2Diagnostics.Txt
  $diagJsonRun1Hash = Get-FileSha256Hex -Path $run1Diagnostics.Json
  $diagJsonRun2Hash = Get-FileSha256Hex -Path $run2Diagnostics.Json

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

  Assert-SemaPassManagerNegativeRuntimeExpectedCodesPresent `
    -FixtureStem $fixtureStem `
    -ExpectedCodes $expectedCodes `
    -ContractId "runtime.negative.diagnostics.expected_code.{0}.{1}" `
    -FailureMessage "negative sema fixture diagnostics missing expected code {0}: {1}" `
    -PassMessage "negative sema fixture diagnostics contain expected code {0}: {1}" `
    -LeftTxtText $diagTxtRun1Text `
    -RightTxtText $diagTxtRun2Text `
    -LeftJsonText $diagJsonRun1Text `
    -RightJsonText $diagJsonRun2Text

  Assert-SemaPassManagerNegativeRuntimeForbiddenArtifactsAbsent `
    -FixtureStem $fixtureStem `
    -ContractId "runtime.negative.artifact.absent.{0}.{1}" `
    -FailureMessage "negative sema fixture produced forbidden artifact {0}: {1}" `
    -PassMessage "negative sema fixture keeps {0} absent: {1}" `
    -OutputDirs @($run1Dir, $run2Dir)

  $CaseResults.Add([pscustomobject]@{
      kind = "negative"
      backend = "clang"
      fixture = Get-RepoRelativePath -Path $NegativeFixturePath -Root $RepoRoot
      expected_codes = $expectedCodes
      run1_exit = $run1Exit
      run2_exit = $run2Exit
      run1_dir = Get-RepoRelativePath -Path $run1Dir -Root $RepoRoot
      run2_dir = Get-RepoRelativePath -Path $run2Dir -Root $RepoRoot
      diagnostics_txt_sha256 = $diagTxtRun1Hash
      diagnostics_json_sha256 = $diagJsonRun1Hash
    }) | Out-Null
}
