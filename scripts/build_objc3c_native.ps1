$ErrorActionPreference = "Stop"

$llvmRoot = if ($env:LLVM_ROOT) { $env:LLVM_ROOT } else { "C:\Program Files\LLVM" }
$clangxx = Join-Path $llvmRoot "bin\clang++.exe"
$libclang = Join-Path $llvmRoot "lib\libclang.lib"
$includeDir = Join-Path $llvmRoot "include"

if (!(Test-Path $clangxx)) { throw "clang++ not found at $clangxx" }
if (!(Test-Path $libclang)) { throw "libclang not found at $libclang" }
if (!(Test-Path $includeDir)) { throw "LLVM include dir not found at $includeDir" }

$outDir = "artifacts/bin"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outExe = Join-Path $outDir "objc3c-native.exe"

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  "-I$includeDir" `
  native/objc3c/src/main.cpp `
  $libclang `
  -o $outExe

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Output "built=$outExe"
