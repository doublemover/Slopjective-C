param(
  [string]$FixtureList = "",
  [string]$FixtureGlob = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_native_fixture_matrix/orchestration.psm1") -Force -DisableNameChecking

# Suite ownership: this script owns the broad positive dispatch/artifact-sanity
# sweep only. Recovery pass/fail and negative diagnostics ownership stays on the
# dedicated recovery contract suite, so this matrix no longer recompiles the
# baseline recovery-positive corpus.

Invoke-Objc3cNativeFixtureMatrix `
  -ScriptRoot $PSScriptRoot `
  -FixtureList $FixtureList `
  -FixtureGlob $FixtureGlob `
  -ShardIndex $ShardIndex `
  -ShardCount $ShardCount `
  -Limit $Limit
