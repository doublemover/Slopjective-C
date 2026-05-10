$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$artifactsModule = Join-Path $PSScriptRoot "artifacts.psm1"
$processesModule = Join-Path $PSScriptRoot "processes.psm1"
$readinessModule = Join-Path $PSScriptRoot "readiness.psm1"
foreach ($dependencyModule in @($artifactsModule, $processesModule, $readinessModule)) {
  if (!(Test-Path -LiteralPath $dependencyModule -PathType Leaf)) {
    Write-Error "native compile toolchain dependency missing at $dependencyModule"
    exit 2
  }
  Import-Module $dependencyModule -Force -DisableNameChecking
}

function Ensure-NativeCompilerAvailable {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult
  )

  if (Test-NativeCompilerBuildArtifactsReady -CompilerRepoRoot $RepoRoot -ExistingBuildResult $BuildResult) {
    return $BuildResult
  }

  $nextBuildResult = Invoke-BuildNativeCompiler -RepoRoot $RepoRoot
  foreach ($lineText in @($nextBuildResult.build_output_lines)) {
    Write-Host $lineText
  }
  $buildExit = [int]$nextBuildResult.exit_code
  if ($buildExit -ne 0) {
    exit $buildExit
  }

  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $RepoRoot
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "native compiler executable missing at $exe"
    exit 2
  }

  return $nextBuildResult
}

Export-ModuleMember -Function "Ensure-NativeCompilerAvailable"
