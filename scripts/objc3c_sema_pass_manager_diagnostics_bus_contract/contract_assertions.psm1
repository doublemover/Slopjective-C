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
