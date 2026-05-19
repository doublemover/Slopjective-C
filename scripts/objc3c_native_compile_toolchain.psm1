$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Export-ModuleMember {
  param(
    [string[]]$Function,
    [string[]]$Cmdlet,
    [string[]]$Variable,
    [string[]]$Alias
  )
}

$toolchainModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_toolchain"
$toolchainHelperModules = @(
  "artifacts.psm1",
  "commands.psm1",
  "results.psm1",
  "processes.psm1",
  "readiness.psm1",
  "availability.psm1"
)

foreach ($helperModuleName in $toolchainHelperModules) {
  $helperModulePath = Join-Path $toolchainModuleRoot $helperModuleName
  if (!(Test-Path -LiteralPath $helperModulePath -PathType Leaf)) {
    Write-Error "native compile toolchain helper missing at $helperModulePath"
    exit 2
  }
  $helperRootLiteral = (Split-Path -Parent $helperModulePath).Replace("'", "''")
  $helperScript = [scriptblock]::Create(
    "`$PSScriptRoot = '$helperRootLiteral'`n" +
    (Get-Content -LiteralPath $helperModulePath -Raw)
  )
  . $helperScript
}

Microsoft.PowerShell.Core\Export-ModuleMember -Function @(
  "Ensure-NativeCompilerAvailable",
  "Invoke-NativeCompiler",
  "Resolve-FrontendConformanceCorpusPath",
  "Resolve-FrontendConformanceMatrixPath",
  "Resolve-FrontendCoreFeatureExpansionPath",
  "Resolve-FrontendDiagnosticsHardeningPath",
  "Resolve-FrontendEdgeCompatibilityPath",
  "Resolve-FrontendEdgeRobustnessPath",
  "Resolve-FrontendIntegrationCloseoutPath",
  "Resolve-FrontendInvocationLockPath",
  "Resolve-FrontendRecoveryDeterminismHardeningPath",
  "Resolve-FrontendScaffoldPath",
  "Resolve-NativeCompilerExecutablePath"
)
