function Assert-SemaPassManagerPositiveReplayExitCodes {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$Run1Log,
    [Parameter(Mandatory = $true)][string]$Run2Log,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition ($Run1Exit -eq 0 -and $Run2Exit -eq 0) `
    -Id $Id `
    -FailureMessage ($FailureMessage -f $Run1Exit, $Run2Exit) `
    -PassMessage $PassMessage `
    -Evidence @{
      run1_exit = $Run1Exit
      run2_exit = $Run2Exit
      run1_log = Get-RepoRelativePath -Path $Run1Log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $Run2Log -Root $RepoRoot
    }
}

function Assert-SemaPassManagerPositiveMatrixExitCodes {
  param(
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  Assert-Contract `
    -Condition ($Run1Exit -eq 0 -and $Run2Exit -eq 0) `
    -Id $Id `
    -FailureMessage ($FailureMessage -f $Run1Exit, $Run2Exit) `
    -PassMessage $PassMessage
}

function Assert-SemaPassManagerPositiveDeterministicArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][string]$IdPrefix,
    [Parameter(Mandatory = $true)][string]$MissingFailureTemplate,
    [Parameter(Mandatory = $true)][string]$ExistsPassTemplate,
    [Parameter(Mandatory = $true)][string]$EmptyObjectId,
    [Parameter(Mandatory = $true)][string]$EmptyObjectFailure,
    [Parameter(Mandatory = $true)][string]$EmptyObjectPass,
    [Parameter(Mandatory = $true)][string]$HashFailureTemplate,
    [Parameter(Mandatory = $true)][string]$HashPassTemplate
  )

  $digests = @{}
  foreach ($artifact in @(Get-SemaPassManagerPositiveRuntimeArtifacts)) {
    $artifactRun1 = Join-Path $Run1Dir $artifact
    $artifactRun2 = Join-Path $Run2Dir $artifact
    $existsRun1 = Test-Path -LiteralPath $artifactRun1 -PathType Leaf
    $existsRun2 = Test-Path -LiteralPath $artifactRun2 -PathType Leaf
    Assert-Contract `
      -Condition ($existsRun1 -and $existsRun2) `
      -Id ("{0}.artifact.exists.{1}" -f $IdPrefix, $artifact) `
      -FailureMessage ($MissingFailureTemplate -f $artifact) `
      -PassMessage ($ExistsPassTemplate -f $artifact)

    if ($artifact -eq "module.obj") {
      $objRun1Bytes = (Get-Item -LiteralPath $artifactRun1).Length
      $objRun2Bytes = (Get-Item -LiteralPath $artifactRun2).Length
      Assert-Contract `
        -Condition ($objRun1Bytes -gt 0 -and $objRun2Bytes -gt 0) `
        -Id $EmptyObjectId `
        -FailureMessage $EmptyObjectFailure `
        -PassMessage $EmptyObjectPass `
        -Evidence @{ run1_bytes = $objRun1Bytes; run2_bytes = $objRun2Bytes }
    }

    $hashRun1 = Get-FileSha256Hex -Path $artifactRun1
    $hashRun2 = Get-FileSha256Hex -Path $artifactRun2
    $digests[$artifact] = New-SemaPassManagerPositiveArtifactDigestRecord -Run1Sha256 $hashRun1 -Run2Sha256 $hashRun2
    Assert-Contract `
      -Condition ($hashRun1 -eq $hashRun2) `
      -Id ("{0}.artifact.deterministic_sha256.{1}" -f $IdPrefix, $artifact) `
      -FailureMessage ($HashFailureTemplate -f $artifact) `
      -PassMessage ($HashPassTemplate -f $artifact) `
      -Evidence @{ run1_sha256 = $hashRun1; run2_sha256 = $hashRun2 }
  }

  return $digests
}

function Assert-SemaPassManagerPositiveClangOutputs {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Dir
  )

  $positiveDiagnosticsText = Read-NormalizedText -Path (Join-Path $Run1Dir "module.diagnostics.txt")
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($positiveDiagnosticsText)) `
    -Id "runtime.positive.diagnostics.empty" `
    -FailureMessage "positive sema fixture emitted diagnostics unexpectedly" `
    -PassMessage "positive sema diagnostics are empty"

  $positiveManifestText = Read-NormalizedText -Path (Join-Path $Run1Dir "module.manifest.json")
  $manifestHasSemaZeroDiag = (
    $positiveManifestText.IndexOf('"semantic": {"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0 -or
    $positiveManifestText.IndexOf('"semantic":{"diagnostics":0}', [System.StringComparison]::Ordinal) -ge 0
  )
  Assert-Contract `
    -Condition $manifestHasSemaZeroDiag `
    -Id "runtime.positive.manifest.semantic_stage_zero_diag" `
    -FailureMessage "positive sema manifest missing semantic stage diagnostics=0 marker" `
    -PassMessage "positive sema manifest reports semantic diagnostics=0"

  Assert-Contract `
    -Condition ($positiveManifestText.IndexOf('"semantic_skipped": false', [System.StringComparison]::Ordinal) -ge 0 -and
                $positiveManifestText.IndexOf('"semantic_surface": {', [System.StringComparison]::Ordinal) -ge 0) `
    -Id "runtime.positive.manifest.semantic_surface_present" `
    -FailureMessage "positive sema manifest missing semantic surface presence markers" `
    -PassMessage "positive sema manifest reports semantic surface presence markers"
}

function Assert-SemaPassManagerPositiveBackendText {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$ExpectedBackend,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage
  )

  $backendText = (Read-NormalizedText -Path (Join-Path $Run1Dir "module.object-backend.txt")).Trim()
  Assert-Contract `
    -Condition ($backendText -eq $ExpectedBackend) `
    -Id $Id `
    -FailureMessage ($FailureMessage -f $backendText) `
    -PassMessage $PassMessage
}

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

function Assert-SemaPassManagerForbiddenArtifactsAbsent {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][string[]]$Artifacts,
    [Parameter(Mandatory = $true)][string]$IdTemplate,
    [Parameter(Mandatory = $true)][string]$FailureTemplate,
    [Parameter(Mandatory = $true)][string]$PassTemplate
  )

  foreach ($artifact in $Artifacts) {
    $artifactRun1 = Join-Path $Run1Dir $artifact
    $artifactRun2 = Join-Path $Run2Dir $artifact
    $present = (Test-Path -LiteralPath $artifactRun1 -PathType Leaf) -or (Test-Path -LiteralPath $artifactRun2 -PathType Leaf)
    Assert-Contract `
      -Condition (-not $present) `
      -Id ($IdTemplate -f $artifact) `
      -FailureMessage ($FailureTemplate -f $artifact) `
      -PassMessage ($PassTemplate -f $artifact)
  }
}

function Get-SemaPassManagerForcedMissingLlcMarkerKey {
  param([Parameter(Mandatory = $true)][string]$LogText)

  $driverShellMarker = "llc executable not found:"
  $backendCompileMarker = "llvm-direct object emission failed: llc executable not found:"
  $markerSet = New-Object 'System.Collections.Generic.List[string]'
  if ($LogText.IndexOf($driverShellMarker, [System.StringComparison]::Ordinal) -ge 0) {
    $markerSet.Add("driver-shell") | Out-Null
  }
  if ($LogText.IndexOf($backendCompileMarker, [System.StringComparison]::Ordinal) -ge 0) {
    $markerSet.Add("backend-compile") | Out-Null
  }
  return [string]::Join("|", $markerSet.ToArray())
}

function Assert-SemaPassManagerForcedMissingLlcUnavailable {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][object]$Layout,
    [Parameter(Mandatory = $true)][string]$ForcedMissingLlcPath,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit
  )

  $forcedMissingLlcExitInExpectedFamily = ($Run1Exit -in @(2, 3) -and $Run2Exit -in @(2, 3))
  Assert-Contract `
    -Condition (
      $Run1Exit -ne 0 -and
      $Run2Exit -ne 0 -and
      $Run1Exit -eq $Run2Exit -and
      $forcedMissingLlcExitInExpectedFamily
    ) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes" `
    -FailureMessage ("forced missing-llc replay must fail-closed deterministically with exit code 2 or 3 (run1={0} run2={1})" -f $Run1Exit, $Run2Exit) `
    -PassMessage "forced missing-llc replay fail-closes deterministically with expected exit-code family (2|3)" `
    -Evidence @{
      run1_log = Get-RepoRelativePath -Path $Layout.run1_log -Root $RepoRoot
      run2_log = Get-RepoRelativePath -Path $Layout.run2_log -Root $RepoRoot
    }

  $run1LogText = Read-NormalizedText -Path $Layout.run1_log
  $run2LogText = Read-NormalizedText -Path $Layout.run2_log
  $driverShellMarker = "llc executable not found:"
  $backendCompileMarker = "llvm-direct object emission failed: llc executable not found:"
  $hasDriverShellMarker = (
    $run1LogText.IndexOf($driverShellMarker, [System.StringComparison]::Ordinal) -ge 0 -and
    $run2LogText.IndexOf($driverShellMarker, [System.StringComparison]::Ordinal) -ge 0
  )
  $hasBackendCompileMarker = (
    $run1LogText.IndexOf($backendCompileMarker, [System.StringComparison]::Ordinal) -ge 0 -and
    $run2LogText.IndexOf($backendCompileMarker, [System.StringComparison]::Ordinal) -ge 0
  )
  $run1MarkerKey = Get-SemaPassManagerForcedMissingLlcMarkerKey -LogText $run1LogText
  $run2MarkerKey = Get-SemaPassManagerForcedMissingLlcMarkerKey -LogText $run2LogText
  Assert-Contract `
    -Condition ($hasDriverShellMarker -or $hasBackendCompileMarker) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.fail_closed_marker" `
    -FailureMessage "forced missing-llc replay logs are missing deterministic llc-not-found fail-closed marker" `
    -PassMessage "forced missing-llc replay logs include deterministic llc-not-found fail-closed marker"
  Assert-Contract `
    -Condition ((-not [string]::IsNullOrWhiteSpace($run1MarkerKey)) -and ($run1MarkerKey -eq $run2MarkerKey)) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.fail_closed_marker_set_deterministic" `
    -FailureMessage ("forced missing-llc replay marker set drift detected between runs (run1={0}; run2={1})" -f $run1MarkerKey, $run2MarkerKey) `
    -PassMessage ("forced missing-llc replay marker set is deterministic across runs ({0})" -f $run1MarkerKey)

  $forcedMissingLlcCompileStage = ($Run1Exit -eq 3 -and $Run2Exit -eq 3)
  Assert-SemaPassManagerForcedMissingLlcPreObjectArtifacts `
    -Run1Dir $Layout.run1_dir `
    -Run2Dir $Layout.run2_dir `
    -CompileStage $forcedMissingLlcCompileStage
  Assert-SemaPassManagerForbiddenArtifactsAbsent `
    -Run1Dir $Layout.run1_dir `
    -Run2Dir $Layout.run2_dir `
    -Artifacts @(Get-SemaPassManagerPositiveForbiddenObjectArtifacts) `
    -IdTemplate "runtime.positive.matrix.llvm_direct_forced_missing_llc.forbidden_artifact.absent.{0}" `
    -FailureTemplate "forced missing-llc replay produced forbidden artifact {0}" `
    -PassTemplate "forced missing-llc replay keeps {0} absent"

  if ($forcedMissingLlcCompileStage) {
    Assert-SemaPassManagerForcedMissingLlcDiagnosticsDeterministic -Run1Dir $Layout.run1_dir -Run2Dir $Layout.run2_dir
  }
}

function Assert-SemaPassManagerForcedMissingLlcPreObjectArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [Parameter(Mandatory = $true)][bool]$CompileStage
  )

  foreach ($artifact in @(Get-SemaPassManagerPositivePreObjectArtifacts)) {
    $artifactRun1 = Join-Path $Run1Dir $artifact
    $artifactRun2 = Join-Path $Run2Dir $artifact
    if ($CompileStage) {
      Assert-Contract `
        -Condition ((Test-Path -LiteralPath $artifactRun1 -PathType Leaf) -and (Test-Path -LiteralPath $artifactRun2 -PathType Leaf)) `
        -Id ("runtime.positive.matrix.llvm_direct_forced_missing_llc.required_artifact.exists.{0}" -f $artifact) `
        -FailureMessage ("forced missing-llc replay (compile-stage failure) missing expected pre-object artifact {0}" -f $artifact) `
        -PassMessage ("forced missing-llc replay (compile-stage failure) preserves pre-object artifact {0}" -f $artifact)
    }
    else {
      Assert-Contract `
        -Condition ((-not (Test-Path -LiteralPath $artifactRun1 -PathType Leaf)) -and (-not (Test-Path -LiteralPath $artifactRun2 -PathType Leaf))) `
        -Id ("runtime.positive.matrix.llvm_direct_forced_missing_llc.required_artifact.absent_shell_stage.{0}" -f $artifact) `
        -FailureMessage ("forced missing-llc replay (shell-stage failure) unexpectedly produced artifact {0}" -f $artifact) `
        -PassMessage ("forced missing-llc replay (shell-stage failure) keeps {0} absent" -f $artifact)
    }
  }
}

function Assert-SemaPassManagerForcedMissingLlcDiagnosticsDeterministic {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir
  )

  $diagRun1TxtPath = Join-Path $Run1Dir "module.diagnostics.txt"
  $diagRun2TxtPath = Join-Path $Run2Dir "module.diagnostics.txt"
  $diagRun1JsonPath = Join-Path $Run1Dir "module.diagnostics.json"
  $diagRun2JsonPath = Join-Path $Run2Dir "module.diagnostics.json"
  $diagRun1Text = Read-NormalizedText -Path $diagRun1TxtPath
  $diagRun2Text = Read-NormalizedText -Path $diagRun2TxtPath
  $diagRun1JsonText = Read-NormalizedText -Path $diagRun1JsonPath
  $diagRun2JsonText = Read-NormalizedText -Path $diagRun2JsonPath
  $diagRun1TxtHash = Get-FileSha256Hex -Path $diagRun1TxtPath
  $diagRun2TxtHash = Get-FileSha256Hex -Path $diagRun2TxtPath
  $diagRun1JsonHash = Get-FileSha256Hex -Path $diagRun1JsonPath
  $diagRun2JsonHash = Get-FileSha256Hex -Path $diagRun2JsonPath
  Assert-Contract `
    -Condition ($diagRun1TxtHash -eq $diagRun2TxtHash) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.diagnostics_txt.deterministic_sha256" `
    -FailureMessage "forced missing-llc replay diagnostics text hash drift detected" `
    -PassMessage "forced missing-llc replay diagnostics text hash is deterministic"
  Assert-Contract `
    -Condition ($diagRun1JsonHash -eq $diagRun2JsonHash) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.diagnostics_json.deterministic_sha256" `
    -FailureMessage "forced missing-llc replay diagnostics json hash drift detected" `
    -PassMessage "forced missing-llc replay diagnostics json hash is deterministic"
  Assert-Contract `
    -Condition ([string]::IsNullOrWhiteSpace($diagRun1Text) -and [string]::IsNullOrWhiteSpace($diagRun2Text)) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.diagnostics.empty" `
    -FailureMessage "forced missing-llc replay emitted semantic diagnostics unexpectedly" `
    -PassMessage "forced missing-llc replay keeps semantic diagnostics empty"
  Assert-Contract `
    -Condition ((-not [regex]::IsMatch($diagRun1JsonText, 'O3[A-Z]\d{3}')) -and (-not [regex]::IsMatch($diagRun2JsonText, 'O3[A-Z]\d{3}'))) `
    -Id "runtime.positive.matrix.llvm_direct_forced_missing_llc.diagnostics.codes_absent" `
    -FailureMessage "forced missing-llc replay diagnostics json unexpectedly contains frontend diagnostic codes" `
    -PassMessage "forced missing-llc replay diagnostics json keeps frontend diagnostic codes absent"
}
