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

if ((Test-Path -LiteralPath $installHome) -and -not $Force.IsPresent) {
  throw "installer target already exists: $installHome"
}

if (Test-Path -LiteralPath $installHome) {
  Remove-Item -LiteralPath $installHome -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $resolvedInstallRoot | Out-Null
Copy-Item -LiteralPath $sourceRoot -Destination $installHome -Recurse -Force
Copy-Item -LiteralPath $bootstrapSource -Destination $bootstrapTarget -Force

$receipt = [ordered]@{
  contract_id = "objc3c.packaging.channels.install-receipt.v1"
  install_root = $resolvedInstallRoot
  install_home = $installHome
  bootstrap_script = $bootstrapTarget
  installed_at_utc = [DateTime]::UtcNow.ToString("o")
}
$receipt | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $receiptPath -Encoding utf8

Write-Output ("install_root: " + $resolvedInstallRoot)
Write-Output ("install_home: " + $installHome)
Write-Output ("receipt_path: " + $receiptPath)
Write-Output ("bootstrap_script: " + $bootstrapTarget)
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

if (Test-Path -LiteralPath $installHome) {
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
$installerImageRoot = Join-Path $stagingRoot "installer-image"
$installerScript = Join-Path $installerImageRoot "Install-objc3c.ps1"

if (Test-Path -LiteralPath $installerImageRoot) {
  Remove-Item -LiteralPath $installerImageRoot -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $stagingRoot | Out-Null
Expand-Archive -LiteralPath $installerArchive -DestinationPath $installerImageRoot -Force

& $installerScript -InstallRoot $InstallRoot -Force
if (-not $?) {
  exit $LASTEXITCODE
}

Write-Output ("offline_bundle_root: " + $bundleRoot)
Write-Output ("installer_archive: " + $installerArchive)
"""
