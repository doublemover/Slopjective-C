Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "path_normalization.psm1") -Force -DisableNameChecking

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

Export-ModuleMember -Function @(
  "Copy-RepoRelativeFile",
  "Get-RepoRelativeConformanceFiles",
  "Get-RepoRelativeExecutionFixtureFiles",
  "Get-RepoRelativeNativeDocsFiles",
  "Get-RepoRelativePythonToolingFiles",
  "Get-RepoRelativeRecoveryPositiveFiles",
  "Get-RepoRelativeRuntimeAcceptanceFiles",
  "Get-RepoRelativeStdlibFiles"
)
