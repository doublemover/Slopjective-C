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
