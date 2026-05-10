function Invoke-ParserExtractionAstBuilderRuntimeCases {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)][string[]]$NegativeFixturePaths,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  Assert-ParserAstBuilderNativeExecutableReady `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -BuildScriptPath $BuildScriptPath `
    -NativeExePath $NativeExePath `
    -NativeExeExplicit $NativeExeExplicit

  Invoke-ParserAstBuilderPositiveRuntimeCase `
    -RepoRoot $RepoRoot `
    -RunDir $RunDir `
    -NativeExePath $NativeExePath `
    -PositiveFixturePath $PositiveFixturePath `
    -CaseResults $CaseResults

  foreach ($negativeFixturePath in $NegativeFixturePaths) {
    Invoke-ParserAstBuilderNegativeRuntimeCase `
      -RepoRoot $RepoRoot `
      -RunDir $RunDir `
      -NativeExePath $NativeExePath `
      -NegativeFixturePath $negativeFixturePath `
      -CaseResults $CaseResults
  }
}

function Assert-ParserAstBuilderNativeExecutableReady {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit
  )

  if (-not $NativeExeExplicit -and !(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    $buildLog = Join-Path $RunDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $BuildScriptPath) `
      -LogPath $buildLog
    Assert-Contract `
      -Condition ($buildExit -eq 0) `
      -Id "runtime.build.native_executable" `
      -FailureMessage ("native build failed with exit={0} (log={1})" -f $buildExit, (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot)) `
      -PassMessage "native executable build completed" `
      -Evidence @{ exit_code = $buildExit; log = (Get-RepoRelativePath -Path $buildLog -Root $RepoRoot) }
  }

  Assert-FileExists -Path $NativeExePath -Id "runtime.native_executable.exists" -Description "native executable"
}

function Invoke-ParserAstBuilderNativeReplay {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][string]$Run1Log,
    [Parameter(Mandatory = $true)][string]$Run2Log
  )

  $argsRun1 = @($FixturePath, "--out-dir", $Run1Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $argsRun2 = @($FixturePath, "--out-dir", $Run2Dir, "--emit-prefix", "module", "--objc3-ir-object-backend", "clang")
  $run1Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun1 -LogPath $Run1Log
  $run2Exit = Invoke-LoggedCommand -Command $NativeExePath -Arguments $argsRun2 -LogPath $Run2Log

  return [pscustomobject]@{
    run1_exit = $run1Exit
    run2_exit = $run2Exit
  }
}

function Invoke-ParserAstBuilderPositiveRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $positiveCaseDir = Join-Path $RunDir "positive_smoke"
  $positiveRun1Dir = Join-Path $positiveCaseDir "run1"
  $positiveRun2Dir = Join-Path $positiveCaseDir "run2"
  New-Item -ItemType Directory -Force -Path $positiveRun1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $positiveRun2Dir | Out-Null
  $positiveRun1Log = Join-Path $positiveCaseDir "run1.log"
  $positiveRun2Log = Join-Path $positiveCaseDir "run2.log"
  $replay = Invoke-ParserAstBuilderNativeReplay `
    -NativeExePath $NativeExePath `
    -FixturePath $PositiveFixturePath `
    -Run1Dir $positiveRun1Dir `
    -Run2Dir $positiveRun2Dir `
    -Run1Log $positiveRun1Log `
    -Run2Log $positiveRun2Log

  Assert-Contract `
    -Condition ($replay.run1_exit -eq 0 -and $replay.run2_exit -eq 0) `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage ("positive parser scaffold fixture compile exits must be zero (run1={0} run2={1})" -f $replay.run1_exit, $replay.run2_exit) `
    -PassMessage "positive parser scaffold fixture compiles successfully across replay" `
    -Evidence @{
      run1_exit = $replay.run1_exit
      run2_exit = $replay.run2_exit
      run1_log = Get-RepoRelativePath -Path $positiveRun1Log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $positiveRun2Log -Root $RepoRoot
    }

  $positiveDigests = @{}
  foreach ($artifact in (Get-ParserAstBuilderPositiveArtifacts)) {
    $artifactRun1 = Join-Path $positiveRun1Dir $artifact
    $artifactRun2 = Join-Path $positiveRun2Dir $artifact
    Assert-ParserAstBuilderReplayedArtifactExists -ArtifactName $artifact -Run1Path $artifactRun1 -Run2Path $artifactRun2

    if ($artifact -eq "module.obj") {
      Assert-ParserAstBuilderObjectArtifactNonEmpty -Run1Path $artifactRun1 -Run2Path $artifactRun2
    }

    $digestRecord = New-ParserAstBuilderDigestRecord -Run1Path $artifactRun1 -Run2Path $artifactRun2
    $positiveDigests[$artifact] = $digestRecord
    Assert-ParserAstBuilderReplayDigest -ArtifactName $artifact -DigestRecord $digestRecord
  }

  Assert-ParserAstBuilderPositiveDiagnostics -RunDir $positiveRun1Dir
  Assert-ParserAstBuilderPositiveLl -RunDir $positiveRun1Dir
  Assert-ParserAstBuilderPositiveManifest -RunDir $positiveRun1Dir
  Assert-ParserAstBuilderPositiveBackend -RunDir $positiveRun1Dir

  Add-ParserAstBuilderPositiveCaseResult `
    -CaseResults $CaseResults `
    -RepoRoot $RepoRoot `
    -FixturePath $PositiveFixturePath `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Run1Dir $positiveRun1Dir `
    -Run2Dir $positiveRun2Dir `
    -ArtifactDigests $positiveDigests
}

function Assert-ParserAstBuilderPositiveDiagnostics {
  param([Parameter(Mandatory = $true)][string]$RunDir)

  $positiveDiagnosticsText = Read-NormalizedText -Path (Join-Path $RunDir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($positiveDiagnosticsText)) `
    -Id "runtime.positive.diagnostics.empty" `
    -FailureMessage "positive parser scaffold fixture emitted diagnostics unexpectedly" `
    -PassMessage "positive parser scaffold diagnostics are empty"
}

function Assert-ParserAstBuilderPositiveLl {
  param([Parameter(Mandatory = $true)][string]$RunDir)

  $positiveLlText = Read-NormalizedText -Path (Join-Path $RunDir "module.ll")
  Assert-Contract `
    -Condition ($positiveLlText.IndexOf("define i32 @objc3c_entry", [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.ll.contains_objc3c_entry" `
    -FailureMessage "positive parser scaffold LLVM IR missing objc3c_entry" `
    -PassMessage "positive parser scaffold LLVM IR contains objc3c_entry"
}

function Assert-ParserAstBuilderPositiveManifest {
  param([Parameter(Mandatory = $true)][string]$RunDir)

  $positiveManifestText = Read-NormalizedText -Path (Join-Path $RunDir "module.manifest.json")
  $manifestHasParserZeroDiag = (
    $positiveManifestText.IndexOf('"parser":{"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0 -or
    $positiveManifestText.IndexOf('"parser": {"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $manifestHasParserZeroDiag `
    -Id "runtime.positive.manifest.parser_stage_zero_diag" `
    -FailureMessage "positive parser scaffold manifest missing parser stage diagnostics=0 marker" `
    -PassMessage "positive parser scaffold manifest reports parser diagnostics=0"

  Assert-Contract `
    -Condition ($positiveManifestText.IndexOf('"name":"choose"', [System.StringComparison]::Ordinal) -ge 0 -and $positiveManifestText.IndexOf('"name":"main"', [System.StringComparison]::Ordinal) -ge 0 -and $positiveManifestText.IndexOf('"name":"seed"', [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.manifest.ast_surface_names" `
    -FailureMessage "positive parser scaffold manifest missing expected function/global names from AST surface" `
    -PassMessage "positive parser scaffold manifest includes expected AST surface function/global names"
}

function Assert-ParserAstBuilderPositiveBackend {
  param([Parameter(Mandatory = $true)][string]$RunDir)

  $backendText = (Read-NormalizedText -Path (Join-Path $RunDir "module.object-backend.txt")).Trim()
  Assert-Contract `
    -Condition ($backendText -eq "clang") `
    -Id "runtime.positive.object_backend.clang" `
    -FailureMessage ("positive parser scaffold expected object backend 'clang' but saw '{0}'" -f $backendText) `
    -PassMessage "positive parser scaffold uses explicit clang object backend"
}

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
