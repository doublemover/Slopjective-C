[CmdletBinding()]
param(
  [string]$RuntimeSource = 'out/toolchains/libobjc2-v2.3/src',
  [string]$RuntimeBuild = 'out/toolchains/libobjc2-v2.3/build/stage',
  [string]$ClangBin = 'C:\Program Files\LLVM\bin',
  [string]$BuildDirectory = 'out/toolchains/libobjc2-v2.3/build/stage/cmake-adapter-test',
  [string]$InstallDirectory = 'out/toolchains/libobjc2-v2.3/build/stage/cmake-adapter-install'
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$runtimeSourcePath = (Resolve-Path (Join-Path $repoRoot $RuntimeSource)).Path
$runtimeBuildPath = (Resolve-Path (Join-Path $repoRoot $RuntimeBuild)).Path
$clangBinPath = (Resolve-Path $ClangBin).Path
$robinMapPath = (Resolve-Path (Join-Path $repoRoot 'out/toolchains/robin-map-v1.4.0/src')).Path
$buildPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $BuildDirectory))
$installPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $InstallDirectory))
foreach ($path in @($buildPath, $installPath)) {
  if (-not $path.StartsWith($repoRoot.TrimEnd('\', '/') + [System.IO.Path]::DirectorySeparatorChar,
      [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "CMake adapter test output must remain inside the checkout: $path"
  }
}

if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir) {
  throw 'Load the Visual Studio 2022 v143 developer environment and Windows SDK before continuing.'
}
if ($env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207') { throw 'Expected MSVC v143 14.44.35207.' }
if ($env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') { throw 'Expected Windows SDK 10.0.26100.0.' }

$clangCl = Join-Path $clangBinPath 'clang-cl.exe'
if (-not (Test-Path $clangCl)) { throw "Pinned clang-cl client driver is unavailable: $clangCl" }
$clangClCmake = $clangCl.Replace('\', '/')
$fixture = Join-Path $repoRoot 'tests/tooling/fixtures/native_runtime_cmake'
$module = Join-Path $repoRoot 'native/objc3c/cmake/Objc3NativeRuntime.cmake'
$cmake = (Get-Command cmake.exe -ErrorAction Stop).Source

& $cmake --fresh -S $fixture -B $buildPath -G Ninja `
  "-DCMAKE_CXX_COMPILER=$clangClCmake" `
  "-DCMAKE_INSTALL_PREFIX=$installPath" `
  "-DOBJC3_NATIVE_RUNTIME_MODULE=$module" `
  "-DOBJC3_TEST_RUNTIME_SOURCE=$runtimeSourcePath" `
  "-DOBJC3_TEST_RUNTIME_BUILD=$runtimeBuildPath" `
  "-DOBJC3_TEST_ROBIN_MAP_SOURCE=$robinMapPath"
if ($LASTEXITCODE -ne 0) { throw "Native runtime CMake fixture configure failed: $LASTEXITCODE" }
& $cmake --build $buildPath --parallel 2
if ($LASTEXITCODE -ne 0) { throw "Native runtime CMake fixture build failed: $LASTEXITCODE" }
& $cmake --install $buildPath --component Runtime
if ($LASTEXITCODE -ne 0) { throw "Native runtime CMake fixture install failed: $LASTEXITCODE" }

$packageRoot = Join-Path $installPath 'h3-runtime'
$requiredPackageFiles = @(
  (Join-Path $packageRoot 'bin/objc.dll'),
  (Join-Path $packageRoot 'lib/objc.lib'),
  (Join-Path $packageRoot 'lib/objc3_test_native_adapter.lib'),
  (Join-Path $packageRoot 'include/runtime/platform/objc3_native_object_adapter.h'),
  (Join-Path $packageRoot 'include/objc/objc-arc.h'),
  (Join-Path $packageRoot 'include/objc/objc-config.h'),
  (Join-Path $packageRoot 'share/objc3c/runtime/runtime-profile.json'),
  (Join-Path $packageRoot 'share/licenses/libobjc2/COPYING')
)
foreach ($path in $requiredPackageFiles) {
  if (-not (Test-Path $path)) { throw "Expected runtime package artifact is missing: $path" }
}

$env:PATH = "$(Join-Path $packageRoot 'bin');$env:PATH"
$consumer = Join-Path $packageRoot 'bin/objc3_native_runtime_cmake_consumer.exe'
if (-not (Test-Path $consumer)) { throw "Installed CMake consumer was not packaged: $consumer" }
& $consumer
if ($LASTEXITCODE -ne 0) { throw "Installed native runtime consumer failed: $LASTEXITCODE" }
Write-Output 'NATIVE RUNTIME CMAKE PASS: adapter target, imported objc link, package contents, installed weak-slot consumer'
