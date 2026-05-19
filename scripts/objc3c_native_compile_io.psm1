$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot "objc3c_native_compile_provenance.ps1")

$compileIoModuleRoot = Join-Path $PSScriptRoot "objc3c_native_compile_io"
$compileIoHelperModules = @(
  "path_normalization.psm1",
  "command_context.psm1",
  "cache_io.psm1"
)

foreach ($helperModuleName in $compileIoHelperModules) {
  $helperModulePath = Join-Path $compileIoModuleRoot $helperModuleName
  if (!(Test-Path -LiteralPath $helperModulePath -PathType Leaf)) {
    Write-Error "native compile IO helper missing at $helperModulePath"
    exit 2
  }
  Import-Module $helperModulePath -Force -DisableNameChecking -Prefix NativeCompileIo
}

function Resolve-RepoBoundPath {
  param(
    [string]$RepoRoot,
    [string]$RelativeOrAbsolutePath,
    [string]$Label
  )

  return Resolve-NativeCompileIoRepoBoundPath @PSBoundParameters
}

function Get-Objc3cNativeCompileInputPath {
  param([string[]]$ArgsWithoutOutDir)

  return Get-NativeCompileIoObjc3cNativeCompileInputPath @PSBoundParameters
}

function New-Objc3cNativeCompileCacheContext {
  param(
    [string]$RepoRoot,
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$WrapperScriptPath
  )

  return New-NativeCompileIoObjc3cNativeCompileCacheContext @PSBoundParameters
}

function Restore-Objc3cNativeCompileCacheEntry {
  param(
    [object]$CacheContext,
    [string]$DestinationRoot
  )

  return Restore-NativeCompileIoObjc3cNativeCompileCacheEntry @PSBoundParameters
}

function Save-Objc3cNativeCompileCacheEntry {
  param(
    [object]$CacheContext,
    [string]$SourceRoot,
    [int]$CompileExit
  )

  Save-NativeCompileIoObjc3cNativeCompileCacheEntry @PSBoundParameters
}

Export-ModuleMember -Function @(
  "Get-FileSha256Hex",
  "Get-Objc3cNativeCompileInputPath",
  "New-Objc3cNativeCompileCacheContext",
  "Resolve-RepoBoundPath",
  "Restore-Objc3cNativeCompileCacheEntry",
  "Save-Objc3cNativeCompileCacheEntry",
  "Write-CompileOutputProvenance"
)
