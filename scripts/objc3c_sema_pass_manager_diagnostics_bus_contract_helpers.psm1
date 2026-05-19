Set-StrictMode -Version Latest

$script:ContractChecks = $null
$script:ContractRepoRoot = $null

function Set-SemaPassManagerDiagnosticsBusContractContext {
  param(
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $script:ContractChecks = $Checks
  $script:ContractRepoRoot = $RepoRoot
}

function Add-Check {
  param(
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][string]$Detail,
    [Parameter()][object]$Evidence = $null
  )

  if ($null -eq $script:ContractChecks) {
    throw "sema extraction contract FAIL: check sink has not been initialized"
  }

  $script:ContractChecks.Add([pscustomobject]@{
      id = $Id
      passed = $Passed
      detail = $Detail
      evidence = $Evidence
    }) | Out-Null
}

function Assert-Contract {
  param(
    [Parameter(Mandatory = $true)][bool]$Condition,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$FailureMessage,
    [Parameter(Mandatory = $true)][string]$PassMessage,
    [Parameter()][object]$Evidence = $null
  )

  if (-not $Condition) {
    Add-Check -Id $Id -Passed $false -Detail $FailureMessage -Evidence $Evidence
    throw "sema extraction contract FAIL: $FailureMessage"
  }
  Add-Check -Id $Id -Passed $true -Detail $PassMessage -Evidence $Evidence
}

function Assert-FileExists {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$Description
  )

  if ([string]::IsNullOrWhiteSpace($script:ContractRepoRoot)) {
    throw "sema extraction contract FAIL: repo root has not been initialized"
  }

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $script:ContractRepoRoot
  Assert-Contract `
    -Condition $exists `
    -Id $Id `
    -FailureMessage "missing $Description at $relative" `
    -PassMessage "found $Description at $relative" `
    -Evidence @{ path = $relative }
}

function Resolve-ValidatedRunId {
  param(
    [Parameter()][string]$ConfiguredRunId,
    [Parameter(Mandatory = $true)][string]$DefaultRunId
  )

  if ([string]::IsNullOrWhiteSpace($ConfiguredRunId)) {
    return $DefaultRunId
  }

  $candidate = $ConfiguredRunId.Trim()
  if ($candidate.Length -gt 80) {
    throw "sema extraction FAIL: configured run id exceeds 80 characters"
  }
  if ($candidate -notmatch '^[A-Za-z0-9_-]+$') {
    throw "sema extraction FAIL: configured run id must match ^[A-Za-z0-9_-]+$"
  }
  return $candidate
}

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = [System.IO.Path]::GetFullPath("$Path")
  $fullRoot = [System.IO.Path]::GetFullPath("$Root")
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Read-NormalizedText {
  param([Parameter(Mandatory = $true)][string]$Path)

  $text = Get-Content -LiteralPath $Path -Raw
  $text = $text -replace "`r`n", "`n"
  $text = $text -replace "`r", "`n"
  return $text
}

function Get-FileSha256Hex {
  param([Parameter(Mandatory = $true)][string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Assert-TokensPresent {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$RequiredTokens
  )

  foreach ($token in $RequiredTokens) {
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      return $false
    }
  }
  return $true
}

function Get-ExpectedSemaCodesFromFixture {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $text = Read-NormalizedText -Path $FixturePath
  $match = [regex]::Match($text, '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$')
  if (-not $match.Success) {
    throw "sema extraction FAIL: missing expected diagnostic header in $FixturePath"
  }
  $codes = [regex]::Matches($match.Groups[1].Value, 'O3[A-Z]\d{3}') | ForEach-Object { $_.Value.ToUpperInvariant() }
  $normalized = @($codes | Sort-Object -Unique)
  if ($normalized.Count -eq 0) {
    throw "sema extraction FAIL: expected diagnostic header has no parseable codes in $FixturePath"
  }
  $nonSemaCodes = @($normalized | Where-Object { $_ -notmatch '^O3S\d{3}$' })
  if ($nonSemaCodes.Count -gt 0) {
    throw "sema extraction FAIL: expected diagnostics for sema fixture must be O3S* only in $FixturePath (found: $($nonSemaCodes -join ','))"
  }
  return $normalized
}


. (Join-Path $PSScriptRoot "objc3c_sema_pass_manager_diagnostics_bus_runtime_cases.psm1")
