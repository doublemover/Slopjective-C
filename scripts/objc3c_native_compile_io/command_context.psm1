$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileIoScriptRoot = Split-Path -Parent $PSScriptRoot
$hashIoModule = Join-Path $compileIoScriptRoot "objc3c_native_compile_provenance/hash_io.psm1"
if (!(Test-Path -LiteralPath $hashIoModule -PathType Leaf)) {
  Write-Error "native compile hash IO helper missing at $hashIoModule"
  exit 2
}
Import-Module $hashIoModule -Force -DisableNameChecking

function Get-Objc3cNativeCompileCacheKey {
  param(
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$CompilerSourcePath,
    [string]$WrapperScriptPath
  )

  if ([string]::IsNullOrWhiteSpace($InputPath)) {
    return $null
  }
  if (!(Test-Path -LiteralPath $InputPath -PathType Leaf)) {
    return $null
  }

  $inputHash = Get-FileSha256Hex -Path $InputPath
  $compilerSourceHash = Get-OptionalFileHash -Path $CompilerSourcePath
  $wrapperScriptHash = Get-OptionalFileHash -Path $WrapperScriptPath
  $payload = [ordered]@{
    version = 2
    input_sha256 = $inputHash
    compiler_source_sha256 = $compilerSourceHash
    wrapper_script_sha256 = $wrapperScriptHash
    args = $ArgsWithoutOutDir
  }
  $payloadJson = $payload | ConvertTo-Json -Compress -Depth 6
  $payloadBytes = [System.Text.Encoding]::UTF8.GetBytes($payloadJson)
  return Get-Sha256HexFromBytes -Bytes $payloadBytes
}

function New-Objc3cNativeCompileCacheContext {
  param(
    [string]$RepoRoot,
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$WrapperScriptPath
  )

  $cacheRoot = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/cache"
  $compilerSourcePath = Join-Path $RepoRoot "native/objc3c/src/main.cpp"
  $cacheEntryContractId = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
  $cacheKey = Get-Objc3cNativeCompileCacheKey `
    -InputPath $InputPath `
    -ArgsWithoutOutDir $ArgsWithoutOutDir `
    -CompilerSourcePath $compilerSourcePath `
    -WrapperScriptPath $WrapperScriptPath

  return [pscustomobject]@{
    cache_root = $cacheRoot
    compiler_source_path = $compilerSourcePath
    entry_contract_id = $cacheEntryContractId
    cache_key = $cacheKey
  }
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeCompileCacheKey",
  "New-Objc3cNativeCompileCacheContext"
)
