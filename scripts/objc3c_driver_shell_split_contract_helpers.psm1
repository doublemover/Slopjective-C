$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$script:DriverShellSplitContractChecks = $null
$script:DriverShellSplitContractRepoRoot = $null

function Set-Objc3cDriverShellSplitContractContext {
  param(
    [Parameter(Mandatory = $true)]$Checks,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $script:DriverShellSplitContractChecks = $Checks
  $script:DriverShellSplitContractRepoRoot = $RepoRoot
}

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $pathText = "$Path"
  $rootText = "$Root"
  $fullPath = [System.IO.Path]::GetFullPath($pathText)
  $fullRoot = [System.IO.Path]::GetFullPath($rootText)
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart([char[]]@('\', '/')).Replace('\', '/')
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

function Add-Check {
  param(
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][bool]$Passed,
    [Parameter(Mandatory = $true)][string]$Detail,
    [Parameter()][object]$Evidence = $null
  )

  if ($null -eq $script:DriverShellSplitContractChecks) {
    throw "driver shell split FAIL: check sink has not been initialized"
  }

  $script:DriverShellSplitContractChecks.Add([pscustomobject]@{
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
    throw "driver shell split FAIL: $FailureMessage"
  }
  Add-Check -Id $Id -Passed $true -Detail $PassMessage -Evidence $Evidence
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

function Assert-FileExists {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Id,
    [Parameter(Mandatory = $true)][string]$Description
  )

  if ([string]::IsNullOrWhiteSpace($script:DriverShellSplitContractRepoRoot)) {
    throw "driver shell split FAIL: repo root has not been initialized"
  }

  $exists = Test-Path -LiteralPath $Path -PathType Leaf
  $relative = Get-RepoRelativePath -Path $Path -Root $script:DriverShellSplitContractRepoRoot
  Assert-Contract `
    -Condition $exists `
    -Id $Id `
    -FailureMessage "missing $Description at $relative" `
    -PassMessage "found $Description at $relative" `
    -Evidence @{ path = $relative }
}

Export-ModuleMember -Function @(
  "Add-Check",
  "Assert-Contract",
  "Assert-FileExists",
  "Get-FileSha256Hex",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Read-NormalizedText",
  "Set-Objc3cDriverShellSplitContractContext"
)
