$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
$repo = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
Import-Module (Join-Path $repo "scripts/objc3c_native_compile_io/command_context.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $repo "scripts/objc3c_native_compile_io/cache_io.psm1") -Force -DisableNameChecking
function Assert([bool]$Condition,[string]$Message) { if (-not $Condition) { throw $Message } }
function Assert-TestOwnedPath([string]$Path) {
  $full = [IO.Path]::GetFullPath($Path)
  $owned = [IO.Path]::GetFullPath($root)
  $relative = [IO.Path]::GetRelativePath($owned, $full).Replace('\','/')
  Assert (-not [IO.Path]::IsPathRooted($relative) -and -not ($relative.Split('/') -contains '..')) "test target escaped its temporary workspace"
}
function Key([string]$src,[string]$compiler,[string]$wrapper) {
  return Get-Objc3cNativeCompileCacheKey -InputPath $src -ArgsWithoutOutDir @("--emit-ir","--profile=objc3") -CompilerSourcePath $scriptCompilerSource -WrapperScriptPath $wrapper -CompilerBinaryPath $compiler -RepoRoot $scriptRoot
}
$root = Join-Path ([System.IO.Path]::GetTempPath()) ("objc3c-cache-test-" + [Guid]::NewGuid().ToString("N"))
$scriptRoot = Join-Path $root "repo"
$scriptCompilerSource = Join-Path $scriptRoot "native/objc3c/src/main.cpp"
$src = Join-Path $root "sample.objc3"
$compiler = Join-Path $root "objc3c-native.exe"
$wrapperRoot = Join-Path $scriptRoot "scripts"
$wrapper = Join-Path $wrapperRoot "objc3c_native_compile.ps1"
$wrapperDepRoot = Join-Path $scriptRoot "scripts/objc3c_native_compile_io"
New-Item -ItemType Directory -Force -Path $wrapperRoot,$wrapperDepRoot,(Split-Path $scriptCompilerSource) | Out-Null
Set-Content $src "func main() {}" -Encoding utf8
Set-Content $compiler "binary-v1" -Encoding ascii
Set-Content $scriptCompilerSource "compiler source" -Encoding utf8
Set-Content $wrapper "wrapper-v1" -Encoding utf8
Set-Content (Join-Path $wrapperDepRoot "helper.psm1") "helper-v1" -Encoding utf8
try {
  $k1 = Key $src $compiler $wrapper
  Assert ($k1 -and $k1 -eq (Key $src $compiler $wrapper)) "identical key did not hit"
  Set-Content $compiler "binary-v2" -Encoding ascii
  Assert ($k1 -ne (Key $src $compiler $wrapper)) "same-path compiler mutation did not invalidate"
  $k2 = Key $src $compiler $wrapper
  Set-Content (Join-Path $wrapperDepRoot "helper.psm1") "helper-v2" -Encoding utf8
  Assert ($k2 -ne (Key $src $compiler $wrapper)) "transitive wrapper mutation did not invalidate"
  Add-Content $src "import OtherModule"
  Assert ($null -eq (Key $src $compiler $wrapper)) "unresolved imports should disable cache"
  Set-Content $src '#include "provider.h"' -Encoding utf8
  Assert ($null -eq (Key $src $compiler $wrapper)) "preprocessor dependencies should disable cache"
  Set-Content $src '/* comment */ @import OtherModule;' -Encoding utf8
  Assert ($null -eq (Key $src $compiler $wrapper)) "non-line-leading imports should disable cache"
  Set-Content $src "func main() {}" -Encoding utf8

  $cacheRoot = Join-Path $root "cache"
  $outSource = Join-Path $root "produced"
  $dest = Join-Path $root "restore"
  New-Item -ItemType Directory -Force -Path $outSource | Out-Null
  Set-Content (Join-Path $outSource "module.ll") "result" -Encoding utf8
  $recursiveRoot = Join-Path $outSource 'nested-cache'
  $recursive = [pscustomobject]@{cache_root=$recursiveRoot;cache_key='recursive';entry_contract_id='test-contract'}
  Save-Objc3cNativeCompileCacheEntry -CacheContext $recursive -SourceRoot $outSource -CompileExit 0
  Assert (!(Test-Path -LiteralPath $recursiveRoot)) "cache population recursed into its own stage"
  $ctx = [pscustomobject]@{ cache_root=$cacheRoot; cache_key="test-key"; entry_contract_id="test-contract"; allowed_destination_root=$root }
  Save-Objc3cNativeCompileCacheEntry -CacheContext $ctx -SourceRoot $outSource -CompileExit 0
  New-Item -ItemType Directory -Force -Path $dest | Out-Null
  Set-Content (Join-Path $dest "stale.txt") "stale" -Force
  $hit = Restore-Objc3cNativeCompileCacheEntry -CacheContext $ctx -DestinationRoot $dest
  Assert $hit.restored "valid cache entry did not restore"
  Assert (!(Test-Path (Join-Path $dest "stale.txt"))) "restore overlaid stale extra output"
  Assert (@(Get-ChildItem -LiteralPath $root -Directory -Filter '.cache-previous-*').Count -eq 1) "old output was not preserved recoverably"
  $outside = Join-Path ([IO.Path]::GetTempPath()) ('objc3c-unowned-' + [Guid]::NewGuid().ToString('N'))
  Assert (!(Restore-Objc3cNativeCompileCacheEntry -CacheContext $ctx -DestinationRoot $outside).restored) "restore escaped its declared workspace"
  Assert (!(Test-Path -LiteralPath $outside)) "out-of-scope restore wrote files"
  $entry = Join-Path $cacheRoot "test-key"
  $metaPath = Join-Path $entry "metadata.json"
  $meta = Get-Content $metaPath -Raw | ConvertFrom-Json
  $meta.output_files[0].sha256 = ("0" * 64)
  $meta | ConvertTo-Json -Depth 6 | Set-Content $metaPath -Encoding utf8
  Assert (!(Restore-Objc3cNativeCompileCacheEntry -CacheContext $ctx -DestinationRoot $dest).restored) "corrupt manifest was reused"
  Save-Objc3cNativeCompileCacheEntry -CacheContext $ctx -SourceRoot $outSource -CompileExit 0
  $partial = [pscustomobject]@{ cache_root=$cacheRoot; cache_key="partial"; entry_contract_id="test-contract"; allowed_destination_root=$root }
  New-Item -ItemType Directory -Force -Path (Join-Path $cacheRoot "partial/files") | Out-Null
  Assert (!(Restore-Objc3cNativeCompileCacheEntry -CacheContext $partial -DestinationRoot $dest).restored) "partial entry was accepted"
  # Existing key collision preserves a valid immutable winner; unexpected files invalidate it.
  Set-Content (Join-Path $entry "files/extra.txt") "extra" -Encoding ascii
  Assert (!(Restore-Objc3cNativeCompileCacheEntry -CacheContext $ctx -DestinationRoot $dest).restored) "extra file was accepted"
  Assert-TestOwnedPath $cacheRoot
  Remove-Item -LiteralPath $cacheRoot -Recurse -Force
  $writer = { param($module,$cache,$source,$contract) Import-Module $module -Force -DisableNameChecking; $ctx=[pscustomobject]@{cache_root=$cache;cache_key="race";entry_contract_id=$contract}; Save-Objc3cNativeCompileCacheEntry -CacheContext $ctx -SourceRoot $source -CompileExit 0 }
  $jobs = 1..2 | ForEach-Object { Start-Job -ScriptBlock $writer -ArgumentList (Join-Path $repo "scripts/objc3c_native_compile_io/cache_io.psm1"),$cacheRoot,$outSource,"test-contract" }
  $jobs | Wait-Job | Out-Null
  $jobs | Receive-Job | Out-Null
  $jobs | Remove-Job
  $race = [pscustomobject]@{cache_root=$cacheRoot;cache_key="race";entry_contract_id="test-contract";allowed_destination_root=$root}
  Assert (Restore-Objc3cNativeCompileCacheEntry -CacheContext $race -DestinationRoot $dest).restored "concurrent writers published invalid entry"
  "cache key, corruption, staging and writer tests passed"
} finally {
  Assert-TestOwnedPath $root
  Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
}
