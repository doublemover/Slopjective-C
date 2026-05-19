param(
  [string]$FixtureList = "",
  [string]$FixtureGlob = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_helpers.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_native_recovery_contract_runner.psm1") -Force -DisableNameChecking

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$outRoot = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native/contract_check"
$outDir = Join-Path $outRoot $runId
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$compileWrapperScript = Join-Path $repoRoot "scripts/objc3c_native_compile.ps1"
$pwsh = if (Get-Command pwsh -ErrorAction SilentlyContinue) { "pwsh" } elseif (Get-Command powershell -ErrorAction SilentlyContinue) { "powershell" } else { "pwsh" }

# Suite ownership: this script is the authoritative owner for positive recovery
# compile success and negative recovery deterministic diagnostics replay. Other
# suites may consume its summary, but they must not recompile the same recovery
# negative corpus just to restate pass/fail.

& $buildScript -ExecutionMode binaries-only
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Invoke-Objc3cNativeRecoveryContract `
  -RepoRoot $repoRoot `
  -OutDir $outDir `
  -CompilerPath (Join-Path $repoRoot "artifacts/bin/objc3c-native.exe") `
  -CompileWrapperScript $compileWrapperScript `
  -PowerShellExecutable $pwsh `
  -FixtureList $FixtureList `
  -FixtureGlob $FixtureGlob `
  -ShardIndex $ShardIndex `
  -ShardCount $ShardCount `
  -Limit $Limit
