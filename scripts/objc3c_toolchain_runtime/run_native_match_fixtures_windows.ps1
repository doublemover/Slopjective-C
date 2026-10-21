[CmdletBinding()]
param(
  [string]$ClangBin = 'out/toolchains/objc3-development/build/bin',
  [ValidateSet('clang', 'clang-cl', 'objc3c-clang')][string]$ClientDriver = 'clang',
  [string]$NativeDriver = 'out/toolchains/objc3-facade/build/src/clang_foundation/objc3c-clang.exe'
)

& (Join-Path $PSScriptRoot 'run_native_match_transfer_fixtures_windows.ps1') `
  -Suite match -ClangBin $ClangBin -ClientDriver $ClientDriver -NativeDriver $NativeDriver
