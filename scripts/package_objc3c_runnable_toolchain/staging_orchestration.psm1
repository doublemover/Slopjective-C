Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function Invoke-RunnableToolchainPackageBuild {
  param([Parameter(Mandatory = $true)][string]$BuildScript)

  & $BuildScript | ForEach-Object { Write-Host $_ }
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

function Get-RunnableToolchainPackageOwnedRunRoot {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return [System.IO.Path]::GetFullPath(
    (Join-Path $RepoRoot "tmp/pkg/objc3c-native-runnable-toolchain")
  ).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
}

function Test-RunnableToolchainPackagePathIsUnderOwnedRunRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $ownedRoot = Get-RunnableToolchainPackageOwnedRunRoot -RepoRoot $RepoRoot
  $packageRootFullPath = [System.IO.Path]::GetFullPath($PackageRoot).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))

  return $packageRootFullPath.StartsWith(
    $ownedRoot + [System.IO.Path]::DirectorySeparatorChar,
    [System.StringComparison]::OrdinalIgnoreCase
  )
}

function Test-RunnableToolchainPackageRootIsReparsePoint {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  $packageRootItem = Get-Item -LiteralPath $PackageRoot -Force
  return ($packageRootItem.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0
}

function Assert-RunnableToolchainPackagePathHasNoReparseAncestor {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $ownedRoot = Get-RunnableToolchainPackageOwnedRunRoot -RepoRoot $RepoRoot
  $packageRootFullPath = [System.IO.Path]::GetFullPath($PackageRoot).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
  $relativePath = [System.IO.Path]::GetRelativePath($ownedRoot, $packageRootFullPath)
  $currentPath = $ownedRoot

  if ((Test-Path -LiteralPath $currentPath) -and (Test-RunnableToolchainPackageRootIsReparsePoint -PackageRoot $currentPath)) {
    throw "runnable toolchain package FAIL: refusing to stage through reparse-point owned root: $currentPath"
  }

  foreach ($segment in @($relativePath -split '[\\/]+')) {
    if ([string]::IsNullOrWhiteSpace($segment) -or $segment -eq ".") {
      continue
    }
    if ($segment -eq "..") {
      throw "runnable toolchain package FAIL: package root escaped owned run root: $PackageRoot"
    }
    $currentPath = Join-Path $currentPath $segment
    if ((Test-Path -LiteralPath $currentPath) -and (Test-RunnableToolchainPackageRootIsReparsePoint -PackageRoot $currentPath)) {
      throw "runnable toolchain package FAIL: refusing to stage through reparse-point package ancestor: $currentPath"
    }
  }
}

function Assert-RunnableToolchainPackageTreeHasNoReparsePoints {
  param([Parameter(Mandatory = $true)][string]$PackageRoot)

  if (!(Test-Path -LiteralPath $PackageRoot -PathType Container)) {
    return
  }
  if (Test-RunnableToolchainPackageRootIsReparsePoint -PackageRoot $PackageRoot) {
    throw "runnable toolchain package FAIL: refusing to clean reparse-point package root: $PackageRoot"
  }

  $reparseChild = Get-ChildItem `
    -LiteralPath $PackageRoot `
    -Force `
    -Recurse `
    -Attributes ReparsePoint `
    -ErrorAction Stop |
    Select-Object -First 1
  if ($null -ne $reparseChild) {
    throw "runnable toolchain package FAIL: refusing to recursively clean package tree containing reparse point: $($reparseChild.FullName)"
  }
}

function Initialize-RunnableToolchainPackageRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $isUnderOwnedRunRoot = Test-RunnableToolchainPackagePathIsUnderOwnedRunRoot -RepoRoot $RepoRoot -PackageRoot $PackageRoot
  if ($isUnderOwnedRunRoot) {
    Assert-RunnableToolchainPackagePathHasNoReparseAncestor -RepoRoot $RepoRoot -PackageRoot $PackageRoot
  }

  if (!(Test-Path -LiteralPath $PackageRoot -PathType Container)) {
    New-Item -ItemType Directory -Force -Path $PackageRoot | Out-Null
    return
  }

  if ($isUnderOwnedRunRoot) {
    Assert-RunnableToolchainPackageTreeHasNoReparsePoints -PackageRoot $PackageRoot
    Remove-Item -LiteralPath $PackageRoot -Recurse -Force
    New-Item -ItemType Directory -Force -Path $PackageRoot | Out-Null
    return
  }

  $existingEntry = Get-ChildItem -LiteralPath $PackageRoot -Force | Select-Object -First 1
  if ($null -ne $existingEntry) {
    throw "runnable toolchain package FAIL: package root must be empty for clean-room staging: $PackageRoot"
  }
}

function Get-RunnableToolchainPackageInputFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return @(
    @(Get-RequiredRunnableToolchainPackageFiles) +
    @(Get-RepoRelativeNativeCompileSupportFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeExecutionSupportFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeExecutionFixtureFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeDocsFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeNativeRuntimeSourceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePythonSharedFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePythonToolingFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeConformanceSurfacePythonFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeWorkflowPythonFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePerformanceBenchmarkFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRuntimeAcceptanceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRuntimeProbeFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeRecoveryPositiveFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeStdlibFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativeConformanceFiles -RepoRoot $RepoRoot) +
    @(Get-RepoRelativePackagedPythonScriptFiles -RepoRoot $RepoRoot)
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
    $copiedRelativePaths.Add($relativePath.Replace('\', '/')) | Out-Null
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

  Initialize-RunnableToolchainPackageRoot -RepoRoot $RepoRoot -PackageRoot $PackageRoot
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

  return [pscustomobject]@{
    CopiedRelativePaths = $copiedRelativePaths
    StagedRelativePaths = $stagedRelativePaths
  }
}

Export-ModuleMember -Function @(
  "Invoke-RunnableToolchainPackageBuild",
  "Invoke-RunnableToolchainPackageStaging"
)
