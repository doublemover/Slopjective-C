Set-StrictMode -Version Latest

function Get-RepoRelativePathCompat {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $resolvedRoot = (Resolve-Path -LiteralPath $RootPath).Path
  if (Test-Path -LiteralPath $TargetPath) {
    $resolvedTarget = (Resolve-Path -LiteralPath $TargetPath).Path
  }
  else {
    $resolvedTarget = [System.IO.Path]::GetFullPath($TargetPath)
  }

  if ($resolvedRoot.EndsWith('\\') -or $resolvedRoot.EndsWith('/')) {
    $rootWithSeparator = $resolvedRoot
  }
  else {
    $rootWithSeparator = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  }

  $relativePath = $null
  $getRelativeMethod = [System.IO.Path].GetMethod("GetRelativePath", [Type[]]@([string], [string]))
  if ($null -ne $getRelativeMethod) {
    $relativePath = [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedTarget)
  }
  else {
    $rootUri = New-Object System.Uri($rootWithSeparator)
    $targetUri = New-Object System.Uri($resolvedTarget)
    $relativeUri = $rootUri.MakeRelativeUri($targetUri)
    $relativePath = [System.Uri]::UnescapeDataString($relativeUri.ToString())
  }

  return $relativePath.Replace('\', '/')
}

function Resolve-PackageRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [string]$RequestedRoot
  )

  if ([string]::IsNullOrWhiteSpace($RequestedRoot)) {
    $runId = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
    return (Join-Path $RepoRoot (Join-Path "tmp/pkg/objc3c-native-runnable-toolchain" $runId))
  }

  if ([System.IO.Path]::IsPathRooted($RequestedRoot)) {
    return [System.IO.Path]::GetFullPath($RequestedRoot)
  }

  return [System.IO.Path]::GetFullPath((Join-Path $RepoRoot $RequestedRoot))
}

function Resolve-PackageManifestPath {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestRelativePath
  )

  if ([string]::IsNullOrWhiteSpace($ManifestRelativePath)) {
    throw "runnable toolchain package FAIL: manifest relative path is required"
  }

  $normalizedRelativePath = $ManifestRelativePath.Replace('/', '\')
  if ([System.IO.Path]::IsPathRooted($normalizedRelativePath)) {
    throw "runnable toolchain package FAIL: manifest path must be package-relative: $ManifestRelativePath"
  }

  foreach ($segment in @($normalizedRelativePath -split '[\\/]+')) {
    if ($segment -eq ".." -or $segment -eq ".") {
      throw "runnable toolchain package FAIL: manifest path cannot contain traversal segments: $ManifestRelativePath"
    }
  }

  $packageRootFullPath = [System.IO.Path]::GetFullPath($PackageRoot).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
  $manifestPath = [System.IO.Path]::GetFullPath((Join-Path $packageRootFullPath $normalizedRelativePath))
  $packageRootPrefix = $packageRootFullPath + [System.IO.Path]::DirectorySeparatorChar

  if (-not $manifestPath.StartsWith($packageRootPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "runnable toolchain package FAIL: manifest path escaped package root: $ManifestRelativePath"
  }

  return $manifestPath
}

Export-ModuleMember -Function @(
  "Get-RepoRelativePathCompat",
  "Resolve-PackageManifestPath",
  "Resolve-PackageRoot"
)
