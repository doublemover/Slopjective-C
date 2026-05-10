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

  return $relativePath.Replace('\\', '/')
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

function Assert-RepoFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $fullPath = Join-Path $RepoRoot ($RelativePath.Replace('/', '\\'))
  if (!(Test-Path -LiteralPath $fullPath -PathType Leaf)) {
    throw "runnable toolchain package FAIL: missing required file $RelativePath"
  }

  return $fullPath
}

function Copy-RepoRelativeFile {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$PackageRoot,
    [Parameter(Mandatory = $true)][string]$RelativePath
  )

  $sourcePath = Assert-RepoFile -RepoRoot $RepoRoot -RelativePath $RelativePath
  $destinationPath = Join-Path $PackageRoot ($RelativePath.Replace('/', '\\'))
  $destinationDir = Split-Path -Parent $destinationPath
  New-Item -ItemType Directory -Force -Path $destinationDir | Out-Null
  Copy-Item -LiteralPath $sourcePath -Destination $destinationPath -Force
  return $destinationPath
}

function Get-RepoRelativeFilesUnderRoot {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RelativeRoot,
    [Parameter(Mandatory = $true)][string]$MissingRootMessage,
    [string]$Filter = "*"
  )

  $root = Join-Path $RepoRoot $RelativeRoot
  if (!(Test-Path -LiteralPath $root -PathType Container)) {
    throw $MissingRootMessage
  }

  return @(
    Get-ChildItem -LiteralPath $root -Recurse -File -Filter $Filter |
      Sort-Object -Property FullName |
      ForEach-Object { Get-RepoRelativePathCompat -RootPath $RepoRoot -TargetPath $_.FullName }
  )
}

function Get-RepoRelativeExecutionFixtureFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $fixtureRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/execution"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native/execution" `
    -MissingRootMessage "runnable toolchain package FAIL: missing execution fixture root $fixtureRoot")
}

function Get-RepoRelativeStdlibFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $stdlibRoot = Join-Path $RepoRoot "stdlib"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "stdlib" `
    -MissingRootMessage "runnable toolchain package FAIL: missing stdlib root $stdlibRoot")
}

function Get-RepoRelativeConformanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $conformanceRoot = Join-Path $RepoRoot "tests/conformance"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/conformance" `
    -MissingRootMessage "runnable toolchain package FAIL: missing conformance root $conformanceRoot")
}

function Get-RepoRelativeNativeDocsFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $docsRoot = Join-Path $RepoRoot "docs/objc3c-native"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "docs/objc3c-native" `
    -MissingRootMessage "runnable toolchain package FAIL: missing native docs root $docsRoot")
}

function Get-RepoRelativePythonToolingFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $toolingRoot = Join-Path $RepoRoot "scripts/objc3c_tooling"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_tooling" `
    -MissingRootMessage "runnable toolchain package FAIL: missing Python tooling root $toolingRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeRuntimeAcceptanceFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $acceptanceRoot = Join-Path $RepoRoot "scripts/objc3c_runtime_acceptance"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "scripts/objc3c_runtime_acceptance" `
    -MissingRootMessage "runnable toolchain package FAIL: missing runtime acceptance package root $acceptanceRoot" `
    -Filter "*.py")
}

function Get-RepoRelativeRecoveryPositiveFiles {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  $recoveryRoot = Join-Path $RepoRoot "tests/tooling/fixtures/native/recovery/positive"
  return @(Get-RepoRelativeFilesUnderRoot `
    -RepoRoot $RepoRoot `
    -RelativeRoot "tests/tooling/fixtures/native/recovery/positive" `
    -MissingRootMessage "runnable toolchain package FAIL: missing recovery-positive root $recoveryRoot")
}

function Assert-RequiredPackageSurfaceKeys {
  param(
    [Parameter(Mandatory = $true)][hashtable]$Payload,
    [Parameter(Mandatory = $true)][string]$RelativePath,
    [Parameter(Mandatory = $true)][string[]]$RequiredKeys
  )

  foreach ($requiredKey in $RequiredKeys) {
    if (-not $Payload.ContainsKey($requiredKey)) {
      throw "runnable toolchain package FAIL: missing $requiredKey in $RelativePath"
    }
  }
}

Export-ModuleMember -Function @(
  "Assert-RequiredPackageSurfaceKeys",
  "Copy-RepoRelativeFile",
  "Get-RepoRelativeConformanceFiles",
  "Get-RepoRelativeExecutionFixtureFiles",
  "Get-RepoRelativeNativeDocsFiles",
  "Get-RepoRelativePathCompat",
  "Get-RepoRelativePythonToolingFiles",
  "Get-RepoRelativeRecoveryPositiveFiles",
  "Get-RepoRelativeRuntimeAcceptanceFiles",
  "Get-RepoRelativeStdlibFiles",
  "Resolve-PackageRoot"
)
