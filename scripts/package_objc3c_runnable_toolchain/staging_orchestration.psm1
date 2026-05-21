Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "..\objc3c_runnable_toolchain_package_helpers.psm1") -Force -DisableNameChecking

function Get-RunnableToolchainPackagePrivateBuildRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $normalizedPackageRoot = [System.IO.Path]::GetFullPath($PackageRoot).ToLowerInvariant()
  $packageRootBytes = [System.Text.Encoding]::UTF8.GetBytes($normalizedPackageRoot)
  $hashBytes = [System.Security.Cryptography.SHA256]::HashData($packageRootBytes)
  $hashPrefix = -join (
    $hashBytes[0..5] |
      ForEach-Object { $_.ToString("x2") }
  )
  return Join-Path (Join-Path $RepoRoot "tmp/b/pkg") $hashPrefix
}

function Get-RunnableToolchainPackageGeneratedArtifactPaths {
  return @(
    "artifacts/bin/objc3c-native.exe",
    "artifacts/bin/objc3c-frontend-c-api-runner.exe",
    "artifacts/lib/objc3_runtime.lib",
    "tmp/artifacts/objc3c-native/frontend_source_graph.json",
    "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
    "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
    "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
    "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
    "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
    "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
    "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
    "tmp/build-objc3c-native/repo_superclean_source_of_truth.json"
  )
}

function Test-RunnableToolchainPackageGeneratedArtifactPath {
  param([Parameter(Mandatory = $true)][string]$RelativePath)

  $normalized = $RelativePath.Replace('\', '/')
  return @(Get-RunnableToolchainPackageGeneratedArtifactPaths) -contains $normalized
}

function Convert-RunnableToolchainPackageGeneratedPathString {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$Value
  )

  $normalized = $Value.Replace('\', '/')
  $packageRelativeRoot = (Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $PackageRoot).Replace('\', '/')
  $packageFullRoot = [System.IO.Path]::GetFullPath($PackageRoot).Replace('\', '/').TrimEnd('/')
  foreach ($prefix in @($packageRelativeRoot, $packageFullRoot)) {
    if ([string]::IsNullOrWhiteSpace($prefix)) {
      continue
    }
    $prefixWithSlash = $prefix + "/"
    if ($normalized.StartsWith($prefixWithSlash, [System.StringComparison]::OrdinalIgnoreCase)) {
      return $normalized.Substring($prefixWithSlash.Length)
    }
  }
  return $Value
}

function Convert-RunnableToolchainPackageGeneratedPayloadPaths {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)]$Value
  )

  if ($Value -is [string]) {
    return Convert-RunnableToolchainPackageGeneratedPathString `
      -RepoRoot $RepoRoot `
      -PackageRoot $PackageRoot `
      -Value $Value
  }

  if ($Value -is [System.Collections.IDictionary]) {
    foreach ($key in @($Value.Keys)) {
      $Value[$key] = Convert-RunnableToolchainPackageGeneratedPayloadPaths `
        -RepoRoot $RepoRoot `
        -PackageRoot $PackageRoot `
        -Value $Value[$key]
    }
    return $Value
  }

  if ($Value -is [array]) {
    for ($index = 0; $index -lt $Value.Count; $index++) {
      $Value[$index] = Convert-RunnableToolchainPackageGeneratedPayloadPaths `
        -RepoRoot $RepoRoot `
        -PackageRoot $PackageRoot `
        -Value $Value[$index]
    }
    return $Value
  }

  return $Value
}

function Write-RunnableToolchainPackageJsonPayload {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)]$Payload
  )

  $Payload | ConvertTo-Json -Depth 20 | Set-Content -LiteralPath $Path -Encoding utf8
}

function Normalize-RunnableToolchainPackageFrontendArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $artifactRoot = Join-Path $PackageRoot "tmp/artifacts/objc3c-native"
  if (!(Test-Path -LiteralPath $artifactRoot -PathType Container)) {
    throw "runnable toolchain package FAIL: isolated build did not publish frontend artifacts: $artifactRoot"
  }

  foreach ($artifactPath in @(Get-ChildItem -LiteralPath $artifactRoot -Filter "*.json" -File)) {
    $payload = Get-Content -LiteralPath $artifactPath.FullName -Raw | ConvertFrom-Json -AsHashtable
    $payload = Convert-RunnableToolchainPackageGeneratedPayloadPaths `
      -RepoRoot $RepoRoot `
      -PackageRoot $PackageRoot `
      -Value $payload
    Write-RunnableToolchainPackageJsonPayload -Path $artifactPath.FullName -Payload $payload
  }

  $sourceGraphPath = Join-Path $artifactRoot "frontend_source_graph.json"
  $invocationLockPath = Join-Path $artifactRoot "frontend_invocation_lock.json"
  if ((Test-Path -LiteralPath $sourceGraphPath -PathType Leaf) -and (Test-Path -LiteralPath $invocationLockPath -PathType Leaf)) {
    $invocationLock = Get-Content -LiteralPath $invocationLockPath -Raw | ConvertFrom-Json -AsHashtable
    $invocationLock["scaffold"]["sha256"] = (Get-FileHash -LiteralPath $sourceGraphPath -Algorithm SHA256).Hash.ToLowerInvariant()
    Write-RunnableToolchainPackageJsonPayload -Path $invocationLockPath -Payload $invocationLock
  }
}

function Copy-RunnableToolchainPackageRepoSupercleanSurface {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$BuildDir
  )

  $sourcePath = Join-Path $BuildDir "repo_superclean_source_of_truth.json"
  if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: isolated build did not publish repo superclean surface: $sourcePath"
  }

  $payload = Get-Content -LiteralPath $sourcePath -Raw | ConvertFrom-Json -AsHashtable
  $payload["native_build_outputs"]["native_executable"] = "artifacts/bin/objc3c-native.exe"
  $payload["native_build_outputs"]["frontend_c_api_runner"] = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  $payload["native_build_outputs"]["runtime_library"] = "artifacts/lib/objc3_runtime.lib"
  $payload["native_build_outputs"]["compile_commands"] = "tmp/build-objc3c-native/compile_commands.json"

  foreach ($entry in @($payload["frontend_contract_artifacts"])) {
    $artifactPath = [string]$entry["artifact_path"]
    $packageRelativeRoot = (Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $PackageRoot).Replace('\', '/')
    $prefix = $packageRelativeRoot + "/"
    if ($artifactPath.StartsWith($prefix, [System.StringComparison]::OrdinalIgnoreCase)) {
      $entry["artifact_path"] = $artifactPath.Substring($prefix.Length)
    }
  }

  $destinationPath = Join-Path $PackageRoot "tmp/build-objc3c-native/repo_superclean_source_of_truth.json"
  $destinationDir = Split-Path -Parent $destinationPath
  New-Item -ItemType Directory -Force -Path $destinationDir | Out-Null
  $payload | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $destinationPath -Encoding utf8
}

function Remove-RunnableToolchainPackagePrivateBuildRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot
  )

  $privateBuildRoot = Get-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  if (!(Test-Path -LiteralPath $privateBuildRoot -PathType Container)) {
    return
  }
  Assert-RunnableToolchainPackageTreeHasNoReparsePoints -PackageRoot $privateBuildRoot
  Remove-Item -LiteralPath $privateBuildRoot -Recurse -Force
}

function Invoke-RunnableToolchainPackageBuild {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$BuildScript,
    [int]$Parallelism = 0
  )

  $privateBuildRoot = Get-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  Remove-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  $buildDir = Join-Path $privateBuildRoot "b"
  $summaryPath = Join-Path $privateBuildRoot "native_build_summary.json"
  $runtimeOutputDir = Join-Path $PackageRoot "artifacts/bin"
  $libraryOutputDir = Join-Path $PackageRoot "artifacts/lib"
  $frontendArtifactRoot = Join-Path $PackageRoot "tmp/artifacts/objc3c-native"

  & $BuildScript `
    -ExecutionMode full `
    -BuildDir $buildDir `
    -RuntimeOutputDir $runtimeOutputDir `
    -LibraryOutputDir $libraryOutputDir `
    -FrontendArtifactRoot $frontendArtifactRoot `
    -SummaryPath $summaryPath `
    -Parallelism $Parallelism |
    ForEach-Object { Write-Host $_ }
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }

  Normalize-RunnableToolchainPackageFrontendArtifacts `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
  Copy-RunnableToolchainPackageRepoSupercleanSurface `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -BuildDir $buildDir
  Remove-RunnableToolchainPackagePrivateBuildRoot `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot
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
    @(Get-RepoRelativeNativeFixtureFiles -RepoRoot $RepoRoot) +
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
  ) |
    Where-Object { !(Test-RunnableToolchainPackageGeneratedArtifactPath -RelativePath $_) } |
    Sort-Object -Unique
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
    [Parameter(Mandatory = $true)][string]$BuildScript,
    [int]$Parallelism = 0
  )

  Initialize-RunnableToolchainPackageRoot -RepoRoot $RepoRoot -PackageRoot $PackageRoot
  Invoke-RunnableToolchainPackageBuild `
    -RepoRoot $RepoRoot `
    -PackageRoot $PackageRoot `
    -BuildScript $BuildScript `
    -Parallelism $Parallelism
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
