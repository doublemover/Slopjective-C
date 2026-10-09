[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)][string]$Source,
  [Parameter(Mandatory = $true)][string]$ClangBin,
  [Parameter(Mandatory = $true)][string]$BuildDirectory,
  [switch]$Build
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$verifier = Join-Path $PSScriptRoot 'verify_runtime_profile.py'
$sourcePath = (Resolve-Path $Source).Path
$clangBinPath = (Resolve-Path $ClangBin).Path
$buildPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $BuildDirectory))
if (-not $buildPath.StartsWith($repoRoot.TrimEnd('\', '/') + [System.IO.Path]::DirectorySeparatorChar,
    [System.StringComparison]::OrdinalIgnoreCase)) {
  throw "Build directory must be inside the project: $buildPath"
}

if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir) {
  throw 'Load the Visual Studio 2022 v143 developer environment and Windows SDK before continuing.'
}
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
  throw 'The Visual Studio v143 C/C++ toolset (cl.exe) is unavailable in this environment.'
}
$expectedVCTools = '14.44.35207'
$expectedSdk = '10.0.26100.0'
if ($env:VCToolsVersion.TrimEnd('\') -ne $expectedVCTools) {
  throw "Expected v143 toolset $expectedVCTools, got $($env:VCToolsVersion)."
}
if ($env:WindowsSDKVersion.TrimEnd('\') -ne $expectedSdk) {
  throw "Expected Windows SDK $expectedSdk, got $($env:WindowsSDKVersion)."
}
$cmakeCommand = Get-Command cmake.exe -ErrorAction Stop
$ninjaCommand = Get-Command ninja.exe -ErrorAction Stop
$cmakeVersion = (& $cmakeCommand.Source --version | Select-Object -First 1).Trim()
$ninjaVersion = (& $ninjaCommand.Source --version | Select-Object -First 1).Trim()
if ($cmakeVersion -ne 'cmake version 4.2.3') { throw "Expected CMake 4.2.3, got $cmakeVersion." }
if ($ninjaVersion -ne '1.13.2') { throw "Expected Ninja 1.13.2, got $ninjaVersion." }

$robinMapPath = (Resolve-Path (Join-Path $repoRoot 'out/toolchains/robin-map-v1.4.0/src')).Path
python $verifier --profile windows-x86_64 --source $sourcePath --robin-map-source $robinMapPath --bin-dir $clangBinPath
if ($LASTEXITCODE -ne 0) { throw 'Runtime profile verification failed; no build was started.' }

if (-not $Build) {
  Write-Output 'Pinned runtime and toolchain checks passed. Pass -Build to configure and compile libobjc2.'
  return
}

$clang = (Join-Path $clangBinPath 'clang.exe').Replace('\', '/')
$clangxx = (Join-Path $clangBinPath 'clang++.exe').Replace('\', '/')
$lld = (Join-Path $clangBinPath 'lld-link.exe').Replace('\', '/')
$robinMapSource = $robinMapPath.Replace('\', '/')
$configure = @(
  '--fresh',
  '-S', $sourcePath,
  '-B', $buildPath,
  '-G', 'Ninja',
  "-DCMAKE_C_COMPILER=$clang",
  "-DCMAKE_CXX_COMPILER=$clangxx",
  "-DCMAKE_OBJC_COMPILER=$clang",
  "-DCMAKE_OBJCXX_COMPILER=$clangxx",
  "-DCMAKE_LINKER=$lld",
  '-DCMAKE_C_FLAGS=--target=x86_64-pc-windows-msvc -fuse-ld=lld',
  '-DCMAKE_CXX_FLAGS=--target=x86_64-pc-windows-msvc -fuse-ld=lld',
  '-DCMAKE_OBJC_FLAGS=--target=x86_64-pc-windows-msvc -fuse-ld=lld -fobjc-runtime=gnustep-2.0',
  '-DCMAKE_OBJCXX_FLAGS=--target=x86_64-pc-windows-msvc -fuse-ld=lld -fobjc-runtime=gnustep-2.0',
  '-DCMAKE_ASM_FLAGS=--target=x86_64-pc-windows-msvc',
  '-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL',
  '-DCMAKE_BUILD_TYPE=Release',
  '-DCMAKE_INSTALL_PREFIX=NONE',
  '-DTESTS=OFF',
  '-DEMBEDDED_BLOCKS_RUNTIME=ON',
  '-DENABLE_OBJCXX=ON',
  '-DOLDABI_COMPAT=OFF',
  '-DCMAKE_DISABLE_FIND_PACKAGE_tsl-robin-map=TRUE',
  "-DFETCHCONTENT_SOURCE_DIR_ROBINMAP=$robinMapSource",
  '-DFETCHCONTENT_FULLY_DISCONNECTED=ON'
)
& cmake @configure
if ($LASTEXITCODE -ne 0) { throw "libobjc2 CMake configure failed with exit code $LASTEXITCODE" }
& cmake --build $buildPath --config Release
if ($LASTEXITCODE -ne 0) { throw "libobjc2 build failed with exit code $LASTEXITCODE" }
Write-Output "PINNED RUNTIME BUILT: $buildPath"
