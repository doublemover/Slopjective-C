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

function Get-PackagePathComparison {
  if ([System.IO.Path]::DirectorySeparatorChar -eq '\') {
    return [System.StringComparison]::OrdinalIgnoreCase
  }

  return [System.StringComparison]::Ordinal
}

function Normalize-PackageManifestRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  if ([string]::IsNullOrWhiteSpace($RelativePath)) {
    throw "runnable toolchain package FAIL: package-relative path is required"
  }

  $trimmedPath = $RelativePath.Trim()
  if (
    [System.IO.Path]::IsPathRooted($trimmedPath) -or
    $trimmedPath.StartsWith("/") -or
    $trimmedPath.StartsWith("\") -or
    $trimmedPath -match '^[A-Za-z]:'
  ) {
    throw "runnable toolchain package FAIL: path must be package-relative: $RelativePath"
  }

  $normalizedPath = $trimmedPath.Replace('\', '/')
  if ($normalizedPath.Contains('//')) {
    throw "runnable toolchain package FAIL: path cannot contain empty segments: $RelativePath"
  }

  $segments = @($normalizedPath.Split('/'))
  if ($segments.Count -eq 0) {
    throw "runnable toolchain package FAIL: package-relative path is empty: $RelativePath"
  }

  foreach ($segment in $segments) {
    if ([string]::IsNullOrWhiteSpace($segment)) {
      throw "runnable toolchain package FAIL: path cannot contain empty segments: $RelativePath"
    }
    if ($segment -eq ".." -or $segment -eq ".") {
      throw "runnable toolchain package FAIL: path cannot contain traversal segments: $RelativePath"
    }
  }

  return $normalizedPath
}

function Convert-PackageRelativePathSegments {
  param(
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  return @((Normalize-PackageManifestRelativePath -RelativePath $RelativePath).Split('/'))
}

function Test-PathUnderHostRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  $rootFullPath = [System.IO.Path]::GetFullPath($RootPath).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))
  $targetFullPath = [System.IO.Path]::GetFullPath($TargetPath)
  $rootPrefix = $rootFullPath + [System.IO.Path]::DirectorySeparatorChar
  $comparison = Get-PackagePathComparison

  return $targetFullPath.StartsWith($rootPrefix, $comparison)
}

function Resolve-PackageRelativeHostPath {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $normalizedPath = Normalize-PackageManifestRelativePath -RelativePath $RelativePath
  $segments = @($normalizedPath.Split('/'))
  $rootFullPath = [System.IO.Path]::GetFullPath($RootPath).TrimEnd([char[]]@(
    [System.IO.Path]::DirectorySeparatorChar,
    [System.IO.Path]::AltDirectorySeparatorChar
  ))

  $candidatePath = $rootFullPath
  foreach ($segment in $segments) {
    $candidatePath = Join-Path $candidatePath $segment
  }

  $resolvedPath = [System.IO.Path]::GetFullPath($candidatePath)
  if (-not (Test-PathUnderHostRoot -RootPath $rootFullPath -TargetPath $resolvedPath)) {
    throw "runnable toolchain package FAIL: path escaped package root: $RelativePath"
  }

  return $resolvedPath
}

function Join-PackageRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  return Resolve-PackageRelativeHostPath -RootPath $RootPath -RelativePath $RelativePath
}

function Get-PackageManifestRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$RootPath,
    [Parameter(Mandatory = $true)][string]$TargetPath
  )

  if (-not (Test-PathUnderHostRoot -RootPath $RootPath -TargetPath $TargetPath)) {
    throw "runnable toolchain package FAIL: target path escaped package root: $TargetPath"
  }

  return Get-RepoRelativePathCompat -RootPath $RootPath -TargetPath $TargetPath
}

function Resolve-PackageManifestPath {
  param(
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$ManifestRelativePath
  )

  return Resolve-PackageRelativeHostPath -RootPath $PackageRoot -RelativePath $ManifestRelativePath
}

Export-ModuleMember -Function @(
  "Convert-PackageRelativePathSegments",
  "Get-PackageManifestRelativePath",
  "Get-RepoRelativePathCompat",
  "Join-PackageRelativePath",
  "Normalize-PackageManifestRelativePath",
  "Resolve-PackageRelativeHostPath",
  "Resolve-PackageManifestPath",
  "Resolve-PackageRoot",
  "Test-PathUnderHostRoot"
)
