function Assert-SemaPassManagerLlvmDirectDefaultUnavailable {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][object]$Layout,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit
  )

  $llvmDirectDefaultExitInExpectedFamily = ($Run1Exit -in @(2, 3) -and $Run2Exit -in @(2, 3))
  Assert-Contract `
    -Condition (
      $Run1Exit -ne 0 -and
      $Run2Exit -ne 0 -and
      $Run1Exit -eq $Run2Exit -and
      $llvmDirectDefaultExitInExpectedFamily
    ) `
    -Id "runtime.positive.matrix.llvm_direct_default.fail_closed_exit_codes" `
    -FailureMessage ("llvm-direct default replay must fail-closed deterministically with exit code 2 or 3 when unavailable (run1={0} run2={1})" -f $Run1Exit, $Run2Exit) `
    -PassMessage "llvm-direct default replay is unavailable and fails closed deterministically with expected exit-code family (2|3)" `
    -Evidence @{
      run1_log = Get-RepoRelativePath -Path $Layout.run1_log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $Layout.run2_log -Root $RepoRoot
    }

  $run1LogText = Read-NormalizedText -Path $Layout.run1_log
  $run2LogText = Read-NormalizedText -Path $Layout.run2_log
  $hasKnownMarkers = $false
  foreach ($marker in @(Get-SemaPassManagerPositiveLlvmDirectUnavailableMarkers)) {
    if ($run1LogText.IndexOf($marker, [System.StringComparison]::Ordinal) -ge 0 -and
        $run2LogText.IndexOf($marker, [System.StringComparison]::Ordinal) -ge 0) {
      $hasKnownMarkers = $true
      break
    }
  }
  Assert-Contract `
    -Condition $hasKnownMarkers `
    -Id "runtime.positive.matrix.llvm_direct_default.fail_closed_markers" `
    -FailureMessage "llvm-direct unavailable replay logs are missing deterministic fail-closed backend diagnostics markers" `
    -PassMessage "llvm-direct unavailable replay logs include deterministic fail-closed backend diagnostics markers"

  Assert-SemaPassManagerForbiddenArtifactsAbsent `
    -Run1Dir $Layout.run1_dir `
    -Run2Dir $Layout.run2_dir `
    -Artifacts @(Get-SemaPassManagerPositiveForbiddenObjectArtifacts) `
    -IdTemplate "runtime.positive.matrix.llvm_direct_default.fail_closed_artifact_absent.{0}" `
    -FailureTemplate "llvm-direct unavailable replay produced forbidden artifact {0}" `
    -PassTemplate "llvm-direct unavailable replay keeps {0} absent"
}
