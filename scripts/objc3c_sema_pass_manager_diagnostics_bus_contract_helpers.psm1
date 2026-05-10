Set-StrictMode -Version Latest

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

Export-ModuleMember -Function @(
  "Assert-TokensPresent",
  "Get-ExpectedSemaCodesFromFixture",
  "Get-FileSha256Hex",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Read-NormalizedText",
  "Resolve-ValidatedRunId"
)
