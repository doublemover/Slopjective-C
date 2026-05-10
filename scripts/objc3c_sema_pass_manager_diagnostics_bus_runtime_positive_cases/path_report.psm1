function New-SemaPassManagerPositiveReplayLayout {
  param(
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$CaseDirectoryName
  )

  $caseDir = Join-Path $RunDir $CaseDirectoryName
  $run1Dir = Join-Path $caseDir "run1"
  $run2Dir = Join-Path $caseDir "run2"
  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null

  return [pscustomobject]@{
    case_dir = $caseDir
    run1_dir = $run1Dir
    run2_dir = $run2Dir
    run1_log = Join-Path $caseDir "run1.log"
    run2_log = Join-Path $caseDir "run2.log"
  }
}

function New-SemaPassManagerPositiveArtifactDigestRecord {
  param(
    [Parameter(Mandatory = $true)][string]$Run1Sha256,
    [Parameter(Mandatory = $true)][string]$Run2Sha256
  )

  return [ordered]@{
    run1_sha256 = $Run1Sha256
    run2_sha256 = $Run2Sha256
    deterministic = ($Run1Sha256 -eq $Run2Sha256)
  }
}

function Add-SemaPassManagerPositiveCaseResult {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$CaseResults,
    [Parameter(Mandatory = $true)][string]$Backend,
    [Parameter(Mandatory = $true)][string]$Mode,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [Parameter(Mandatory = $true)][string]$Run1Dir,
    [Parameter(Mandatory = $true)][string]$Run2Dir,
    [object]$ArtifactDigests = $null,
    [string]$Status = "",
    [string]$LlcPath = ""
  )

  $result = [ordered]@{
    kind = "positive"
    backend = $Backend
    mode = $Mode
    fixture = Get-RepoRelativePath -Path $FixturePath -Root $RepoRoot
    run1_exit = $Run1Exit
    run2_exit = $Run2Exit
    run1_dir = Get-RepoRelativePath -Path $Run1Dir -Root $RepoRoot
    run2_dir = Get-RepoRelativePath -Path $Run2Dir -Root $RepoRoot
  }

  if ($null -ne $ArtifactDigests) {
    $result["artifact_digests"] = $ArtifactDigests
  }
  if (-not [string]::IsNullOrWhiteSpace($Status)) {
    $result["status"] = $Status
  }
  if (-not [string]::IsNullOrWhiteSpace($LlcPath)) {
    $result["llc_path"] = Get-RepoRelativePath -Path $LlcPath -Root $RepoRoot
  }

  $CaseResults.Add([pscustomobject]$result) | Out-Null
}
