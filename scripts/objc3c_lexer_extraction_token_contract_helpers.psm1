$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:LexerTokenContractChecks = $null
$script:LexerTokenContractRepoRoot = $null

function Set-Objc3cLexerExtractionTokenContractContext {
  param(
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $script:LexerTokenContractChecks = $Checks
  $script:LexerTokenContractRepoRoot = $RepoRoot
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
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter()][System.Collections.Generic.HashSet[string]]$Seen = $null
  )

  if ([string]::IsNullOrWhiteSpace($script:LexerTokenContractRepoRoot)) {
    throw "lexer extraction contract FAIL: repo root has not been initialized"
  }

  if ($null -eq $Seen) {
    $Seen = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::OrdinalIgnoreCase)
  }
  $fullPath = [System.IO.Path]::GetFullPath($Path)
  if (-not $Seen.Add($fullPath)) {
    return ""
  }

  $text = Get-Content -LiteralPath $Path -Raw
  $text = $text -replace "`r`n", "`n"
  $text = $text -replace "`r", "`n"
  $expanded = [System.Text.StringBuilder]::new()
  foreach ($line in ($text -split "`n")) {
    [void]$expanded.AppendLine($line)
    $match = [regex]::Match($line, '^\s*#include\s+"([^"]+)"')
    if (-not $match.Success) {
      continue
    }
    $includePath = $match.Groups[1].Value
    $includeFullPath = Join-Path $script:LexerTokenContractRepoRoot ("native/objc3c/src/{0}" -f $includePath)
    if (Test-Path -LiteralPath $includeFullPath -PathType Leaf) {
      [void]$expanded.AppendLine((Read-NormalizedText -Path $includeFullPath -Seen $Seen))
    }
  }
  return $expanded.ToString()
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

function Add-Check {
  param(
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][string]$Detail,
    [Parameter()][object]$Evidence = $null
  )

  if ($null -eq $script:LexerTokenContractChecks) {
    throw "lexer extraction contract FAIL: check sink has not been initialized"
  }

  $script:LexerTokenContractChecks.Add([pscustomobject]@{
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
    throw "lexer extraction contract FAIL: $FailureMessage"
  }
  Add-Check -Id $Id -Passed $true -Detail $PassMessage -Evidence $Evidence
}

function Assert-FileExists {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$Description
  )

  if ([string]::IsNullOrWhiteSpace($script:LexerTokenContractRepoRoot)) {
    throw "lexer extraction contract FAIL: repo root has not been initialized"
  }

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $script:LexerTokenContractRepoRoot
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

Export-ModuleMember -Function @(
  "Add-Check",
  "Assert-Contract",
  "Assert-FileExists",
  "Assert-TokensPresent",
  "Get-FileSha256Hex",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Read-NormalizedText",
  "Set-Objc3cLexerExtractionTokenContractContext"
)
