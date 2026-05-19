$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-Objc3cNativeCompileCacheRestore {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$ParsedArgs,
    [Parameter(Mandatory = $true)]$CacheContext,
    [Parameter(Mandatory = $true)][string]$InputPath,
    [Parameter(Mandatory = $true)][string]$CompilerBinaryPath,
    [Parameter(Mandatory = $true)][string]$WrapperScriptPath
  )

  if (-not $ParsedArgs.use_cache -or $null -eq $CacheContext.cache_key) {
    return $null
  }

  $cacheRestore = Restore-Objc3cNativeCompileCacheEntry `
    -CacheContext $CacheContext `
    -DestinationRoot $ParsedArgs.out_dir
  if ($cacheRestore.restored) {
    Write-Objc3cNativeCompileSuccessArtifacts `
      -RepoRoot $RepoRoot `
      -ParsedArgs $ParsedArgs `
      -InputPath $InputPath `
      -CompilerBinaryPath $CompilerBinaryPath `
      -WrapperScriptPath $WrapperScriptPath
    Write-Output "cache_hit=true"
    exit ([int]$cacheRestore.exit_code)
  }

  return
}

function Save-Objc3cNativeCompileCacheIfEnabled {
  param(
    [Parameter(Mandatory = $true)]$ParsedArgs,
    [Parameter(Mandatory = $true)]$CacheContext,
    [Parameter(Mandatory = $true)][int]$CompileExit
  )

  if ($ParsedArgs.use_cache -and $null -ne $CacheContext.cache_key) {
    Save-Objc3cNativeCompileCacheEntry `
      -CacheContext $CacheContext `
      -SourceRoot $ParsedArgs.out_dir `
      -CompileExit $CompileExit
  }
}
