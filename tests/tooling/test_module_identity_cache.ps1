$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$packet = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$module = Join-Path $packet "scripts/objc3c_native_compile_io/command_context.psm1"
Import-Module $module -Force -DisableNameChecking
function Assert([bool]$Condition,[string]$Message) { if (-not $Condition) { throw $Message } }

$root = Join-Path ([System.IO.Path]::GetTempPath()) ("objc3c-module-cache-id-" + [Guid]::NewGuid().ToString("N"))
$repo = Join-Path $root "repo"
$source = Join-Path $repo "consumer.m"
$compiler = Join-Path $repo "bin/objc3c.exe"
$wrapper = Join-Path $repo "scripts/objc3c_native_compile.ps1"
$resource = Join-Path $repo "sdk/lib/clang/22"
New-Item -ItemType Directory -Force -Path (Split-Path $source),(Split-Path $compiler),(Split-Path $wrapper),$resource | Out-Null
Set-Content -LiteralPath $source -Value "int main(void) { return 0; }" -Encoding utf8
Set-Content -LiteralPath $compiler -Value "binary-one" -Encoding ascii
Set-Content -LiteralPath $wrapper -Value "wrapper" -Encoding utf8
Set-Content -LiteralPath (Join-Path $resource "stddef.h") -Value "resource-one" -Encoding ascii

function Key([string[]]$Arguments) {
  return Get-Objc3cNativeCompileCacheKey -InputPath $source `
    -ArgsWithoutOutDir $Arguments -CompilerSourcePath $wrapper `
    -WrapperScriptPath $wrapper -CompilerBinaryPath $compiler -RepoRoot $repo
}

try {
  $argsBase = @("-resource-dir",$resource,"--target=x86_64-unknown-linux-gnu","-fobjc-runtime=gnustep-2.0")
  $first = Key $argsBase
  Assert ($first -and $first -eq (Key $argsBase)) "identical tool/resource inputs did not produce a stable key"
  Set-Content -LiteralPath (Join-Path $resource "stddef.h") -Value "resource-two" -Encoding ascii
  Assert ($first -ne (Key $argsBase)) "same-path resource header change did not invalidate the key"
  $resourceKey = Key $argsBase
  $unusedResource = Join-Path $repo "unused-resource"
  New-Item -ItemType Directory -Path $unusedResource | Out-Null
  $lastResourceArgs = @("-resource-dir",$unusedResource,("-resource-dir=" + $resource))
  $lastResourceKey = Key $lastResourceArgs
  Assert ([bool]$lastResourceKey) "multiple resource-dir options did not produce a valid key"
  Set-Content -LiteralPath (Join-Path $resource "stddef.h") -Value "resource-three" -Encoding ascii
  Assert ($lastResourceKey -ne (Key $lastResourceArgs)) "last resource-dir selection did not track same-path content changes"
  $runtimeArgs = @("-resource-dir",$resource,"--target=x86_64-unknown-linux-gnu","-fobjc-runtime=gcc")
  Assert ($resourceKey -ne (Key $runtimeArgs)) "runtime ABI argument change did not invalidate the key"
  $runtimeKey = Key $runtimeArgs
  Set-Content -LiteralPath $compiler -Value "binary-two" -Encoding ascii
  Assert ($runtimeKey -ne (Key $runtimeArgs)) "same-path compiler binary change did not invalidate the key"
  Add-Content -LiteralPath $source -Value "@import Provider;"
  Assert ($null -eq (Key $argsBase)) "imports without an emitted dependency closure must remain uncached"
  Assert ($null -eq (Key @("-resource-dir",(Join-Path $repo "missing-resource")))) "missing resource directory must fail closed"
  "module cache identity tests passed"
} finally {
  $resolvedRoot = [System.IO.Path]::GetFullPath($root)
  $expectedParent = [System.IO.Path]::GetFullPath([System.IO.Path]::GetTempPath()).TrimEnd('\','/')
  if ([System.IO.Path]::GetDirectoryName($resolvedRoot).TrimEnd('\','/') -ne $expectedParent -or
      -not [System.IO.Path]::GetFileName($resolvedRoot).StartsWith("objc3c-module-cache-id-", [StringComparison]::Ordinal)) {
    throw "Refusing cleanup outside the task-owned temporary test directory"
  }
  Remove-Item -LiteralPath $root -Recurse -Force -ErrorAction SilentlyContinue
}
