function Invoke-SemaPassManagerPositiveClangDefaultRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $layout = New-SemaPassManagerPositiveReplayLayout -RunDir $RunDir -CaseDirectoryName "positive_smoke"
  $replay = Invoke-SemaPassManagerPositiveReplay `
    -NativeExePath $NativeExePath `
    -FixturePath $PositiveFixturePath `
    -Backend "clang" `
    -Run1Dir $layout.run1_dir `
    -Run2Dir $layout.run2_dir `
    -Run1Log $layout.run1_log `
    -Run2Log $layout.run2_log

  Assert-SemaPassManagerPositiveReplayExitCodes `
    -RepoRoot $RepoRoot `
    -Run1Log $layout.run1_log `
    -Run2Log $layout.run2_log `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Id "runtime.positive.exit_codes" `
    -FailureMessage "positive sema fixture compile exits must be zero (run1={0} run2={1})" `
    -PassMessage "positive sema fixture compiles successfully across replay"
  Assert-SemaPassManagerPositiveMatrixExitCodes `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Id "runtime.positive.matrix.clang_default.exit_codes" `
    -FailureMessage "clang positive replay matrix leg must succeed deterministically (run1={0} run2={1})" `
    -PassMessage "clang positive replay matrix leg compiles successfully across replay"

  $digests = Assert-SemaPassManagerPositiveDeterministicArtifacts `
    -Run1Dir $layout.run1_dir `
    -Run2Dir $layout.run2_dir `
    -IdPrefix "runtime.positive" `
    -MissingFailureTemplate "positive sema fixture missing artifact across replay: {0}" `
    -ExistsPassTemplate "positive sema artifact present across replay: {0}" `
    -EmptyObjectId "runtime.positive.artifact.nonempty.module.obj" `
    -EmptyObjectFailure "positive sema module.obj is empty in one or more runs" `
    -EmptyObjectPass "positive sema module.obj is non-empty across replay" `
    -HashFailureTemplate "positive sema artifact hash drift detected for {0}" `
    -HashPassTemplate "positive sema artifact hash stable for {0}"

  Assert-SemaPassManagerPositiveClangOutputs -Run1Dir $layout.run1_dir
  Assert-SemaPassManagerPositiveBackendText `
    -Run1Dir $layout.run1_dir `
    -ExpectedBackend "clang" `
    -Id "runtime.positive.object_backend.clang" `
    -FailureMessage "positive sema expected object backend 'clang' but saw '{0}'" `
    -PassMessage "positive sema uses explicit clang object backend"
  Assert-SemaPassManagerPositiveBackendText `
    -Run1Dir $layout.run1_dir `
    -ExpectedBackend "clang" `
    -Id "runtime.positive.matrix.clang_default.object_backend.clang" `
    -FailureMessage "clang positive replay matrix leg expected object backend 'clang' but saw '{0}'" `
    -PassMessage "clang positive replay matrix leg reports clang backend provenance"

  Add-SemaPassManagerPositiveCaseResult `
    -RepoRoot $RepoRoot `
    -CaseResults $CaseResults `
    -Backend "clang" `
    -Mode "default" `
    -FixturePath $PositiveFixturePath `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Run1Dir $layout.run1_dir `
    -Run2Dir $layout.run2_dir `
    -ArtifactDigests $digests
}

function Invoke-SemaPassManagerPositiveLlvmDirectDefaultRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $layout = New-SemaPassManagerPositiveReplayLayout -RunDir $RunDir -CaseDirectoryName "positive_smoke_llvm_direct"
  $replay = Invoke-SemaPassManagerPositiveReplay `
    -NativeExePath $NativeExePath `
    -FixturePath $PositiveFixturePath `
    -Backend "llvm-direct" `
    -Run1Dir $layout.run1_dir `
    -Run2Dir $layout.run2_dir `
    -Run1Log $layout.run1_log `
    -Run2Log $layout.run2_log

  if ($replay.run1_exit -eq 0 -and $replay.run2_exit -eq 0) {
    Assert-SemaPassManagerPositiveReplayExitCodes `
      -RepoRoot $RepoRoot `
      -Run1Log $layout.run1_log `
      -Run2Log $layout.run2_log `
      -Run1Exit $replay.run1_exit `
      -Run2Exit $replay.run2_exit `
      -Id "runtime.positive.matrix.llvm_direct_default.exit_codes" `
      -FailureMessage "llvm-direct positive replay unexpectedly failed" `
      -PassMessage "llvm-direct positive replay compiles successfully across replay"

    $digests = Assert-SemaPassManagerPositiveDeterministicArtifacts `
      -Run1Dir $layout.run1_dir `
      -Run2Dir $layout.run2_dir `
      -IdPrefix "runtime.positive.matrix.llvm_direct_default" `
      -MissingFailureTemplate "llvm-direct positive replay missing artifact across runs: {0}" `
      -ExistsPassTemplate "llvm-direct positive artifact present across replay: {0}" `
      -EmptyObjectId "runtime.positive.matrix.llvm_direct_default.artifact.nonempty.module.obj" `
      -EmptyObjectFailure "llvm-direct positive replay produced empty module.obj in one or more runs" `
      -EmptyObjectPass "llvm-direct positive replay produced non-empty module.obj across replay" `
      -HashFailureTemplate "llvm-direct positive replay artifact hash drift detected: {0}" `
      -HashPassTemplate "llvm-direct positive replay artifact hash stable: {0}"
    Assert-SemaPassManagerPositiveBackendText `
      -Run1Dir $layout.run1_dir `
      -ExpectedBackend "llvm-direct" `
      -Id "runtime.positive.matrix.llvm_direct_default.object_backend.llvm_direct" `
      -FailureMessage "llvm-direct positive replay expected object backend 'llvm-direct' but saw '{0}'" `
      -PassMessage "llvm-direct positive replay reports llvm-direct backend provenance"

    Add-SemaPassManagerPositiveCaseResult `
      -RepoRoot $RepoRoot `
      -CaseResults $CaseResults `
      -Backend "llvm-direct" `
      -Mode "default" `
      -FixturePath $PositiveFixturePath `
      -Run1Exit $replay.run1_exit `
      -Run2Exit $replay.run2_exit `
      -Run1Dir $layout.run1_dir `
      -Run2Dir $layout.run2_dir `
      -ArtifactDigests $digests
  }
  else {
    Assert-SemaPassManagerLlvmDirectDefaultUnavailable `
      -RepoRoot $RepoRoot `
      -Layout $layout `
      -Run1Exit $replay.run1_exit `
      -Run2Exit $replay.run2_exit

    Add-SemaPassManagerPositiveCaseResult `
      -RepoRoot $RepoRoot `
      -CaseResults $CaseResults `
      -Backend "llvm-direct" `
      -Mode "default" `
      -FixturePath $PositiveFixturePath `
      -Run1Exit $replay.run1_exit `
      -Run2Exit $replay.run2_exit `
      -Run1Dir $layout.run1_dir `
      -Run2Dir $layout.run2_dir `
      -Status "unavailable-fail-closed"
  }
}

function Invoke-SemaPassManagerPositiveLlvmDirectForcedMissingLlcRuntimeCase {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][string]$PositiveFixturePath,
    [Parameter(Mandatory = $true)]$CaseResults
  )

  $layout = New-SemaPassManagerPositiveReplayLayout -RunDir $RunDir -CaseDirectoryName "positive_smoke_llvm_direct_forced_missing_llc"
  $forcedMissingLlcPath = Join-Path $RunDir "missing-llc-executable.exe"
  Assert-Contract `
    -Condition (-not (Test-Path -LiteralPath $forcedMissingLlcPath -PathType Leaf)) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.path_absent" `
    -FailureMessage ("forced missing-llc path unexpectedly exists: {0}" -f (Get-RepoRelativePath -Path $forcedMissingLlcPath -Root $RepoRoot)) `
    -PassMessage "forced missing-llc path is absent before fail-closed replay"

  $replay = Invoke-SemaPassManagerPositiveReplay `
    -NativeExePath $NativeExePath `
    -FixturePath $PositiveFixturePath `
    -Backend "llvm-direct" `
    -Run1Dir $layout.run1_dir `
    -Run2Dir $layout.run2_dir `
    -Run1Log $layout.run1_log `
    -Run2Log $layout.run2_log `
    -AdditionalArgs @("--llc", $forcedMissingLlcPath)

  Assert-SemaPassManagerForcedMissingLlcUnavailable `
    -RepoRoot $RepoRoot `
    -Layout $layout `
    -ForcedMissingLlcPath $forcedMissingLlcPath `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit

  Add-SemaPassManagerPositiveCaseResult `
    -RepoRoot $RepoRoot `
    -CaseResults $CaseResults `
    -Backend "llvm-direct" `
    -Mode "forced-missing-llc" `
    -FixturePath $PositiveFixturePath `
    -Run1Exit $replay.run1_exit `
    -Run2Exit $replay.run2_exit `
    -Run1Dir $layout.run1_dir `
    -Run2Dir $layout.run2_dir `
    -LlcPath $forcedMissingLlcPath
}
