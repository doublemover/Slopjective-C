param(
  [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$suiteModuleRoot = Join-Path $PSScriptRoot "check_conformance_suite"
Import-Module (Join-Path $suiteModuleRoot "runner.psm1") -Force -DisableNameChecking

Invoke-ConformanceSuite -RepoRoot $RepoRoot
