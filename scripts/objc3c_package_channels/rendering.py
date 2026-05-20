"""PowerShell script rendering for package channels."""

from __future__ import annotations


def install_script_text() -> str:
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot,
  [switch]$Force
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$sourceRoot = Join-Path $PSScriptRoot "payload"
$resolvedInstallRoot = [System.IO.Path]::GetFullPath($InstallRoot)
$installHome = Join-Path $resolvedInstallRoot "objc3c"
$receiptPath = Join-Path $resolvedInstallRoot "objc3c-install-receipt.json"
$bootstrapSource = Join-Path $PSScriptRoot "Bootstrap-objc3cEnvironment.ps1"
$bootstrapTarget = Join-Path $resolvedInstallRoot "Bootstrap-objc3cEnvironment.ps1"

function Assert-NoReparsePointInExistingPath {
  param([Parameter(Mandatory = $true)][string]$Path)

  $fullPath = [System.IO.Path]::GetFullPath($Path)
  $pathRoot = [System.IO.Path]::GetPathRoot($fullPath)
  $currentPath = $pathRoot
  foreach ($segment in @($fullPath.Substring($pathRoot.Length) -split '[\\/]+')) {
    if ([string]::IsNullOrWhiteSpace($segment)) {
      continue
    }
    $currentPath = Join-Path $currentPath $segment
    if (Test-Path -LiteralPath $currentPath) {
      $item = Get-Item -LiteralPath $currentPath -Force
      if (($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "installer refuses to operate through reparse point: $currentPath"
      }
    }
  }
}

function Assert-NoReparsePointInExistingTree {
  param([Parameter(Mandatory = $true)][string]$Path)

  Assert-NoReparsePointInExistingPath -Path $Path
  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return
  }
  $reparseChild = Get-ChildItem -LiteralPath $Path -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "installer refuses to recursively remove tree containing reparse point: $($reparseChild.FullName)"
  }
}

function Assert-ReceiptOwnsInstallHome {
  if (!(Test-Path -LiteralPath $receiptPath -PathType Leaf)) {
    throw "installer target exists without an objc3c install receipt: $installHome"
  }
  $receipt = Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
  if ($receipt.contract_id -ne "objc3c.packaging.channels.install-receipt.v1" -or
      [System.IO.Path]::GetFullPath([string]$receipt.install_home) -ne $installHome -or
      [string]$receipt.bootstrap_entrypoint -ne "Bootstrap-objc3cEnvironment.ps1" -or
      [string]$receipt.package_bridge -ne "objc3c" -or
      [string]$receipt.install_command -ne "npm run objc3c -- build-package-channels") {
    throw "installer target receipt does not own install home: $installHome"
  }
}

if ((Test-Path -LiteralPath $installHome) -and -not $Force.IsPresent) {
  throw "installer target already exists: $installHome"
}

Assert-NoReparsePointInExistingPath -Path $resolvedInstallRoot
if (Test-Path -LiteralPath $installHome) {
  Assert-ReceiptOwnsInstallHome
  Assert-NoReparsePointInExistingTree -Path $installHome
  Remove-Item -LiteralPath $installHome -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $resolvedInstallRoot | Out-Null
Copy-Item -LiteralPath $sourceRoot -Destination $installHome -Recurse -Force
Copy-Item -LiteralPath $bootstrapSource -Destination $bootstrapTarget -Force

$receipt = [ordered]@{
  contract_id = "objc3c.packaging.channels.install-receipt.v1"
  install_root = $resolvedInstallRoot
  install_home = $installHome
  bootstrap_entrypoint = "Bootstrap-objc3cEnvironment.ps1"
  package_bridge = "objc3c"
  install_command = "npm run objc3c -- build-package-channels"
  installed_at_utc = [DateTime]::UtcNow.ToString("o")
}
$receipt | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $receiptPath -Encoding utf8

Write-Output ("install_root: " + $resolvedInstallRoot)
Write-Output ("install_home: " + $installHome)
Write-Output ("receipt_path: " + $receiptPath)
Write-Output ("bootstrap_entrypoint: " + $bootstrapTarget)
"""


def uninstall_script_text() -> str:
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$resolvedInstallRoot = [System.IO.Path]::GetFullPath($InstallRoot)
$installHome = Join-Path $resolvedInstallRoot "objc3c"
$receiptPath = Join-Path $resolvedInstallRoot "objc3c-install-receipt.json"
$bootstrapTarget = Join-Path $resolvedInstallRoot "Bootstrap-objc3cEnvironment.ps1"

function Assert-NoReparsePointInExistingPath {
  param([Parameter(Mandatory = $true)][string]$Path)

  $fullPath = [System.IO.Path]::GetFullPath($Path)
  $pathRoot = [System.IO.Path]::GetPathRoot($fullPath)
  $currentPath = $pathRoot
  foreach ($segment in @($fullPath.Substring($pathRoot.Length) -split '[\\/]+')) {
    if ([string]::IsNullOrWhiteSpace($segment)) {
      continue
    }
    $currentPath = Join-Path $currentPath $segment
    if (Test-Path -LiteralPath $currentPath) {
      $item = Get-Item -LiteralPath $currentPath -Force
      if (($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
        throw "uninstaller refuses to operate through reparse point: $currentPath"
      }
    }
  }
}

function Assert-NoReparsePointInExistingTree {
  param([Parameter(Mandatory = $true)][string]$Path)

  Assert-NoReparsePointInExistingPath -Path $Path
  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return
  }
  $reparseChild = Get-ChildItem -LiteralPath $Path -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "uninstaller refuses to recursively remove tree containing reparse point: $($reparseChild.FullName)"
  }
}

function Assert-ReceiptOwnsInstallHome {
  if (!(Test-Path -LiteralPath $receiptPath -PathType Leaf)) {
    throw "uninstaller target exists without an objc3c install receipt: $installHome"
  }
  $receipt = Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
  if ($receipt.contract_id -ne "objc3c.packaging.channels.install-receipt.v1" -or
      [System.IO.Path]::GetFullPath([string]$receipt.install_home) -ne $installHome -or
      [string]$receipt.bootstrap_entrypoint -ne "Bootstrap-objc3cEnvironment.ps1" -or
      [string]$receipt.package_bridge -ne "objc3c" -or
      [string]$receipt.install_command -ne "npm run objc3c -- build-package-channels") {
    throw "uninstaller target receipt does not own install home: $installHome"
  }
}

if (Test-Path -LiteralPath $installHome) {
  Assert-ReceiptOwnsInstallHome
  Assert-NoReparsePointInExistingTree -Path $installHome
  Remove-Item -LiteralPath $installHome -Recurse -Force
}
if (Test-Path -LiteralPath $bootstrapTarget) {
  Remove-Item -LiteralPath $bootstrapTarget -Force
}
if (Test-Path -LiteralPath $receiptPath) {
  Remove-Item -LiteralPath $receiptPath -Force
}

Write-Output ("rollback_root: " + $resolvedInstallRoot)
"""


def bootstrap_script_text() -> str:
    return """param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$installRoot = $PSScriptRoot
$toolchainHome = Join-Path $installRoot "objc3c"
$binPath = Join-Path $toolchainHome "artifacts/bin"

$env:OBJC3C_HOME = $toolchainHome
if ($env:PATH -notmatch [regex]::Escape($binPath)) {
  $env:PATH = $binPath + [System.IO.Path]::PathSeparator + $env:PATH
}

Write-Output ("objc3c_home: " + $env:OBJC3C_HOME)
Write-Output ("objc3c_bin: " + $binPath)
"""


def offline_bootstrap_script_text() -> str:
    return """param(
  [Parameter(Mandatory = $true)][string]$InstallRoot
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$bundleRoot = $PSScriptRoot
$stagingRoot = Join-Path $bundleRoot "staging"
$installerArchive = Join-Path $bundleRoot "channels/objc3c-windows-x64-installer.zip"
$installerImageRoot = Join-Path $stagingRoot ("installer-image-" + [Guid]::NewGuid().ToString("N"))
$installerScript = Join-Path $installerImageRoot "Install-objc3c.ps1"

function Assert-NoReparsePointInExistingTree {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return
  }
  $root = Get-Item -LiteralPath $Path -Force
  if (($root.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw "offline bootstrap refuses to remove reparse-point staging root: $Path"
  }
  $reparseChild = Get-ChildItem -LiteralPath $Path -Force -Recurse -Attributes ReparsePoint -ErrorAction Stop | Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "offline bootstrap refuses to recursively remove staging tree containing reparse point: $($reparseChild.FullName)"
  }
}

try {
  New-Item -ItemType Directory -Force -Path $stagingRoot | Out-Null
  Expand-Archive -LiteralPath $installerArchive -DestinationPath $installerImageRoot -Force

  & $installerScript -InstallRoot $InstallRoot -Force
  if (-not $?) {
    exit $LASTEXITCODE
  }
}
finally {
  if (Test-Path -LiteralPath $installerImageRoot) {
    Assert-NoReparsePointInExistingTree -Path $installerImageRoot
    Remove-Item -LiteralPath $installerImageRoot -Recurse -Force
  }
}

Write-Output ("offline_bundle_root: " + $bundleRoot)
Write-Output ("installer_archive: " + $installerArchive)
"""
