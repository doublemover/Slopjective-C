[CmdletBinding()]
param(
  [string]$RuntimeSource = 'out/toolchains/libobjc2-v2.3/src',
  [string]$RuntimeBuild = 'out/toolchains/libobjc2-v2.3/build/stage',
  [string]$ClangBin = 'C:\Program Files\LLVM\bin',
  [ValidateSet('clang', 'clang-cl')][string]$ClientDriver = 'clang'
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$runtimeSourcePath = (Resolve-Path (Join-Path $repoRoot $RuntimeSource)).Path
$runtimeBuildPath = (Resolve-Path (Join-Path $repoRoot $RuntimeBuild)).Path
$clangBinPath = (Resolve-Path $ClangBin).Path
$profileCheck = Join-Path $PSScriptRoot 'verify_runtime_profile.py'
$robinMapPath = (Resolve-Path (Join-Path $repoRoot 'out/toolchains/robin-map-v1.4.0/src')).Path

if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir) {
  throw 'Load the Visual Studio 2022 v143 developer environment and Windows SDK before running fixtures.'
}
if ($env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207') { throw 'The active MSVC toolset is not the pinned v143 14.44.35207.' }
if ($env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') { throw 'The active Windows SDK is not 10.0.26100.0.' }

python $profileCheck --profile windows-x86_64 --source $runtimeSourcePath --robin-map-source $robinMapPath --bin-dir $clangBinPath
if ($LASTEXITCODE -ne 0) { throw 'Pinned runtime/toolchain profile validation failed.' }

$runtimeDll = Join-Path $runtimeBuildPath 'objc.dll'
$runtimeImport = Join-Path $runtimeBuildPath 'objc.lib'
$generatedInclude = Join-Path $runtimeBuildPath 'objc'
if (-not (Test-Path $runtimeDll) -or -not (Test-Path $runtimeImport) -or -not (Test-Path (Join-Path $generatedInclude 'objc-config.h'))) {
  throw 'The pinned libobjc2 build products are unavailable; runtime fixtures were not compiled.'
}

$clang = Join-Path $clangBinPath 'clang.exe'
$clangxx = Join-Path $clangBinPath 'clang++.exe'
$clangCl = Join-Path $clangBinPath 'clang-cl.exe'
$fixturesRoot = Join-Path $repoRoot 'tests/tooling/fixtures/clang_foundation/runtime'
$adapter = Join-Path $repoRoot 'native/objc3c/src/runtime/platform/objc3_native_object_adapter.mm'
$adapterObject = Join-Path $runtimeBuildPath 'objc3_native_object_adapter.obj'
$fixtureOutput = Join-Path $runtimeBuildPath 'h3-fixtures'
New-Item -ItemType Directory -Force -Path $fixtureOutput | Out-Null
$runtimeIncludes = @(
  '-I', $runtimeSourcePath,
  '-I', $generatedInclude,
  '-I', (Join-Path $repoRoot 'native/objc3c/src')
)
$targetFlags = @('--target=x86_64-pc-windows-msvc', '-fobjc-runtime=gnustep-2.0', '-fms-runtime-lib=dll', '-fuse-ld=lld')

$adapterCompile = @('--target=x86_64-pc-windows-msvc', '-fms-runtime-lib=dll', '-x', 'c++',
  '-DOBJC3_NATIVE_RUNTIME_GNUSTEP') + $runtimeIncludes + @('-c', $adapter, '-o', $adapterObject)
& $clangxx @adapterCompile
if ($LASTEXITCODE -ne 0) { throw "Native ownership adapter compile failed: $LASTEXITCODE" }

$cases = @(
  @{ Name = 'root_class_arc_weak_blocks'; Source = 'root_class_arc_weak_blocks.m'; Driver = $clang; Flags = @('-fobjc-arc', '-fblocks'); Adapter = $false },
  @{ Name = 'root_class_arc_consumed'; Source = 'root_class_arc_consumed.m'; Driver = $clang; Flags = @('-fobjc-arc'); Adapter = $false },
  @{ Name = 'root_class_mrc'; Source = 'root_class_mrc.m'; Driver = $clang; Flags = @('-fno-objc-arc'); Adapter = $false },
  @{ Name = 'root_class_objcxx_aggregate_cleanup'; Source = 'root_class_objcxx_aggregate_cleanup.mm'; Driver = $clangxx; Flags = @('-std=c++17', '-fobjc-arc'); Adapter = $true },
  @{ Name = 'native_weak_slot_adapter'; Source = 'native_weak_slot_adapter.mm'; Driver = $clangxx; Flags = @('-std=c++17', '-fobjc-arc'); Adapter = $true }
)

foreach ($case in $cases) {
  $source = Join-Path $fixturesRoot $case.Source
  $executable = Join-Path $fixtureOutput ($case.Name + '.exe')
  if ($ClientDriver -eq 'clang-cl') {
    $clientFlags = @('/nologo', '/MD', '/EHsc', '/clang:--target=x86_64-pc-windows-msvc', '/clang:-fobjc-runtime=gnustep-2.0', '/clang:-fms-runtime-lib=dll', '/clang:-fuse-ld=lld')
    foreach ($flag in $case.Flags) {
      switch ($flag) {
        '-fobjc-arc' { $clientFlags += '/clang:-fobjc-arc' }
        '-fno-objc-arc' { $clientFlags += '/clang:-fno-objc-arc' }
        '-fblocks' { $clientFlags += '/clang:-fblocks' }
        '-std=c++17' { $clientFlags += '/std:c++17' }
        default { throw "No clang-cl spelling is defined for fixture flag '$flag'." }
      }
    }
    $clientFlags += @('-I', $runtimeSourcePath, '-I', $generatedInclude, '-I', (Join-Path $repoRoot 'native/objc3c/src'))
    $clientFlags += @($source, '-o', $executable)
    if ($case.Adapter) { $clientFlags += $adapterObject }
    $clientFlags += @('/link', ('/LIBPATH:' + $runtimeBuildPath), 'objc.lib')
    & $clangCl @clientFlags
    if ($LASTEXITCODE -ne 0) { throw "clang-cl client fixture compile/link failed: $($case.Name) ($LASTEXITCODE)" }
    continue
  }
  $arguments = @($targetFlags) + $case.Flags + $runtimeIncludes + @('-L', $runtimeBuildPath, '-lobjc')
  if ($case.Adapter) { $arguments += $adapterObject }
  $arguments += @($source, '-o', $executable)
  & $case.Driver @arguments
  if ($LASTEXITCODE -ne 0) { throw "Fixture compile/link failed: $($case.Name) ($LASTEXITCODE)" }
}

# This changes PATH only for this script and its child fixture processes.
$env:PATH = "$runtimeBuildPath;$env:PATH"
foreach ($case in $cases) {
  $executable = Join-Path $fixtureOutput ($case.Name + '.exe')
  & $executable
  if ($LASTEXITCODE -ne 0) { throw "$ClientDriver client runtime fixture failed: $($case.Name) ($LASTEXITCODE)" }
  Write-Output "RUNTIME FIXTURE PASS ($ClientDriver client): $($case.Name)"
}
