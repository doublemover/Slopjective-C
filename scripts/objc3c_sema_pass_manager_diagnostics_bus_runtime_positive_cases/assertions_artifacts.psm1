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
