Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function Invoke-RunnableToolchainPackageBuild {
  param([Parameter(Mandatory = $true)][string]$BuildScript)

  & $BuildScript
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

function Get-RunnableToolchainPackageInputFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return @(
    @(Get-RequiredRunnableToolchainPackageFiles) +
    @(Get-RepoRelativeExecutionFixtureFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeDocsFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePythonToolingFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRuntimeAcceptanceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRecoveryPositiveFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeStdlibFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeConformanceFiles -RepoRoot $RepoRoot)
  )
}

function Copy-RunnableToolchainPackageInputs {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string[]]$RelativePaths
  )

  $copiedRelativePaths = New-Object System.Collections.Generic.List[string]
  foreach ($relativePath in $RelativePaths) {
    Copy-RepoRelativeFile -RepoRoot $RepoRoot -PackageRoot $PackageRoot -RelativePath $relativePath | Out-Null
    $copiedRelativePaths.Add($relativePath.Replace('\\', '/')) | Out-Null
  }

  return @($copiedRelativePaths)
}

function Get-RunnableToolchainStagedRelativePaths {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath
  )

  return @(
    Get-ChildItem -LiteralPath $PackageRoot -Recurse -File |
      Where-Object { $_.FullName -ne $ManifestPath } |
      Sort-Object -Property FullName |
      ForEach-Object { (Get-RepoRelativePathCompat -RootPath $PackageRoot -TargetPath $_.FullName).Replace('\', '/') }
  )
}

function Set-RunnableToolchainPackagedOutputTimestamps {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  $packagedNativeExecutablePath = Join-Path $PackageRoot "artifacts\bin\objc3c-native.exe"
  $packagedFrontendRunnerPath = Join-Path $PackageRoot "artifacts\bin\objc3c-frontend-c-api-runner.exe"
  $packagedRuntimeLibraryPath = Join-Path $PackageRoot "artifacts\lib\objc3_runtime.lib"
  $normalizedOutputTimestamp = [datetime]::UtcNow
  foreach ($outputPath in @($packagedNativeExecutablePath, $packagedFrontendRunnerPath, $packagedRuntimeLibraryPath)) {
    if (Test-Path -LiteralPath $outputPath -PathType Leaf) {
      $item = Get-Item -LiteralPath $outputPath
      $item.LastWriteTimeUtc = $normalizedOutputTimestamp
    }
  }
}

function Invoke-RunnableToolchainPackageStaging {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestPath,
    [Parameter(Mandatory = $true)][string]$BuildScript
  )

  Invoke-RunnableToolchainPackageBuild -BuildScript $BuildScript
  $inputFiles = @(Get-RunnableToolchainPackageInputFiles -RepoRoot $RepoRoot)
  $copiedRelativePaths = @(Copy-RunnableToolchainPackageInputs `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -RelativePaths $inputFiles)
  $stagedRelativePaths = @(Get-RunnableToolchainStagedRelativePaths `
    -PackageRoot $PackageRoot `
    -ManifestPath $ManifestPath)
  Set-RunnableToolchainPackagedOutputTimestamps -PackageRoot $PackageRoot

  return [ordered]@{
    CopiedRelativePaths = $copiedRelativePaths
    StagedRelativePaths = $stagedRelativePaths
  }
}

Export-ModuleMember -Function @(
  "Invoke-RunnableToolchainPackageBuild",
  "Invoke-RunnableToolchainPackageStaging"
)
