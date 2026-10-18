[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)][ValidateSet('match', 'transfer')][string]$Suite,
  [string]$ClangBin = 'out/toolchains/objc3-development/build/bin',
  [ValidateSet('clang', 'clang-cl', 'objc3c-clang')][string]$ClientDriver = 'clang',
  [string]$NativeDriver = 'out/toolchains/objc3-facade/build/src/clang_foundation/objc3c-clang.exe'
)

$ErrorActionPreference = 'Stop'
$objc3Repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$objc3Python = 'python'
$objc3Results = $null
$objc3Records = @()
$objc3Identity = $null

function Write-Objc3FixtureResults([string]$Status, [string]$Failure = '') {
  $objc3Report = @{} + $objc3Identity
  $objc3Report.status = $Status
  $objc3Report.fixtures = @($objc3Records)
  if ($Status -ne 'running') { $objc3Report.finished_utc = [DateTime]::UtcNow.ToString('o') }
  if ($Failure) { $objc3Report.error = $Failure }
  $objc3Report | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $objc3Results -Encoding utf8
}

Push-Location -LiteralPath $objc3Repo
try {
  $objc3Bin = (Resolve-Path -LiteralPath $ClangBin).Path
  $objc3ExpectedBin = (Resolve-Path -LiteralPath 'out/toolchains/objc3-development/build/bin').Path
  if ($objc3Bin -ne $objc3ExpectedBin) {
    throw 'Native match/transfer fixtures require the sealed development compiler.'
  }
  if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir) {
    throw 'Load the admitted Visual Studio developer environment before running native match/transfer fixtures.'
  }
  if (-not $env:VCToolsVersion -or -not $env:WindowsSDKVersion -or
      $env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207' -or
      $env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') {
    throw 'The active developer environment differs from the admitted toolchain.'
  }
  $env:PYTHONDONTWRITEBYTECODE = '1'
  $objc3SealJson = & $objc3Python -B -m scripts.objc3c_toolchain verify-development
  if ($LASTEXITCODE -ne 0) { throw 'Native match/transfer source/build identity did not verify.' }
  $objc3Seal = ($objc3SealJson -join "`n") | ConvertFrom-Json
  $objc3RuntimeSource = (Resolve-Path -LiteralPath 'out/toolchains/libobjc2-v2.3/src').Path
  $objc3RobinSource = (Resolve-Path -LiteralPath 'out/toolchains/robin-map-v1.4.0/src').Path
  & $objc3Python -B (Join-Path $PSScriptRoot 'verify_runtime_profile.py') `
    --profile windows-x86_64 --source $objc3RuntimeSource --robin-map-source $objc3RobinSource --bin-dir $objc3Bin
  if ($LASTEXITCODE -ne 0) { throw 'Pinned native compiler/runtime profile validation failed.' }

  $objc3LinkerDriver = Join-Path $objc3Bin 'clang++.exe'
  $objc3LinkerVersion = (& $objc3LinkerDriver --version) -join "`n"
  if ($LASTEXITCODE -ne 0 -or
      -not $objc3LinkerVersion.Contains('ObjC3-patches=' + $objc3Seal.patch_digest)) {
    throw 'The linking driver does not match the sealed patch identity.'
  }
  $objc3LinkerHash = (Get-FileHash -LiteralPath $objc3LinkerDriver -Algorithm SHA256).Hash
  $objc3Lld = Join-Path $objc3Bin 'lld-link.exe'
  $objc3LldHash = (Get-FileHash -LiteralPath $objc3Lld -Algorithm SHA256).Hash
  if ($ClientDriver -eq 'objc3c-clang') {
    if (-not $NativeDriver) { throw '-NativeDriver is required for the objc3c-clang client.' }
    $objc3Compiler = (Resolve-Path -LiteralPath $NativeDriver).Path
    # The native driver has no version command. Its linked Clang version string
    # must carry this same seal; retain its separate binary hash in the evidence.
    $objc3MarkerCheck = 'from pathlib import Path; import sys; sys.exit(0 if ("ObjC3-patches="+sys.argv[2]).encode() in Path(sys.argv[1]).read_bytes() else 1)'
    & $objc3Python -B -c $objc3MarkerCheck $objc3Compiler $objc3Seal.patch_digest
    if ($LASTEXITCODE -ne 0) { throw 'The native driver lacks the sealed linked-Clang patch identity.' }
    $objc3CompilerVersion = 'linked Clang marker: ObjC3-patches=' + $objc3Seal.patch_digest
  } else {
    $objc3CompilerName = if ($ClientDriver -eq 'clang') { 'clang++.exe' } else { 'clang-cl.exe' }
    $objc3Compiler = Join-Path $objc3Bin $objc3CompilerName
    $objc3CompilerVersion = (& $objc3Compiler --version) -join "`n"
    if ($LASTEXITCODE -ne 0 -or
        -not $objc3CompilerVersion.Contains('ObjC3-patches=' + $objc3Seal.patch_digest)) {
      throw 'The compiling driver does not match the sealed patch identity.'
    }
  }
  $objc3CompilerHash = (Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash
  $objc3Cases = if ($Suite -eq 'match') {
    @('match-native-runtime.mm', 'match-guard-cleanup-runtime.mm')
  } else {
    @('carrier-transfer-runtime.cpp', 'carrier-transfer-runtime-v2.cpp', 'carrier-transfer-local-runtime.mm')
  }
  $objc3GeneratedInputs = Join-Path $objc3Repo 'out/toolchains/objc3-development/src/clang/test/ObjC3/CodeGen/Inputs'
  $objc3Witnesses = Join-Path $objc3Repo 'tests/clang_foundation'
  $objc3RunId = [guid]::NewGuid().ToString('N')
  $objc3Output = Join-Path $objc3Repo ('out/toolchains/objc3-development/native-' + $Suite + '-fixtures/' +
    $objc3Seal.build_id + '/' + $ClientDriver + '/' + $objc3RunId)
  New-Item -ItemType Directory -Force -Path $objc3Output | Out-Null
  $objc3Results = Join-Path $objc3Output 'results.json'
  $objc3Identity = @{schema_version=1; suite=$Suite; client_driver=$ClientDriver;
    target_triple='x86_64-pc-windows-msvc'; build_identity=$objc3Seal.build_id;
    patch_digest=$objc3Seal.patch_digest; run_id=$objc3RunId;
    compiler=$objc3Compiler; compiler_sha256=$objc3CompilerHash; compiler_version=$objc3CompilerVersion;
    linker_driver=$objc3LinkerDriver; linker_driver_sha256=$objc3LinkerHash; linker_driver_version=$objc3LinkerVersion;
    linker=$objc3Lld; linker_sha256=$objc3LldHash;
    runner_sha256=(Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash;
    started_utc=[DateTime]::UtcNow.ToString('o')}
  Write-Objc3FixtureResults 'running'
  foreach ($objc3Case in $objc3Cases) {
    $objc3Source = Join-Path $objc3Witnesses $objc3Case
    $objc3GeneratedSource = $null
    $objc3GeneratedHash = $null
    $objc3SourceHash = (Get-FileHash -LiteralPath $objc3Source -Algorithm SHA256).Hash
    if ($objc3Case -ne 'carrier-transfer-local-runtime.mm') {
      $objc3GeneratedSource = Join-Path $objc3GeneratedInputs $objc3Case
      $objc3GeneratedHash = (Get-FileHash -LiteralPath $objc3GeneratedSource -Algorithm SHA256).Hash
      # Git's Windows checkout may use CRLF for the tracked witness while LLVM
      # fixtures retain LF. Check the full source text, retaining both raw hashes.
      $objc3WitnessText = [IO.File]::ReadAllText($objc3Source).Replace("`r`n", "`n")
      $objc3GeneratedText = [IO.File]::ReadAllText($objc3GeneratedSource).Replace("`r`n", "`n")
      if ($objc3WitnessText -cne $objc3GeneratedText) {
        throw "Tracked runtime witness differs from the sealed generated fixture: $objc3Case"
      }
    }
    foreach ($objc3Optimization in @('0', '2')) {
      $objc3Name = [IO.Path]::GetFileNameWithoutExtension($objc3Case) + '-O' + $objc3Optimization
      $objc3Executable = Join-Path $objc3Output ($objc3Name + '.exe')
      $objc3Object = Join-Path $objc3Output ($objc3Name + '.obj')
      $objc3SourceComparison = if ($objc3GeneratedSource) {
        'full-text-with-normalized-line-endings'
      } else { 'standalone-hashed-witness' }
      $objc3Record = @{name=$objc3Name; optimization=('O' + $objc3Optimization);
        source=$objc3Source; source_sha256=$objc3SourceHash;
        source_comparison=$objc3SourceComparison;
        generated_source_required=[bool]$objc3GeneratedSource;
        generated_source=$objc3GeneratedSource; generated_source_sha256=$objc3GeneratedHash;
        status='compiling'}
      $objc3Records += $objc3Record
      if ($ClientDriver -eq 'clang') {
        $objc3CompileArguments = @('--target=x86_64-pc-windows-msvc', '-fobjc-runtime=gnustep-2.0',
          '-fms-runtime-lib=dll', '-x', 'objective-c++', '-std=c++20', '-fexceptions', '-fcxx-exceptions',
          '-fobjc-version=3', ('-O' + $objc3Optimization), '-c', $objc3Source, '-o', $objc3Object)
      } elseif ($ClientDriver -eq 'clang-cl') {
        $objc3CompileArguments = @('/nologo', '/MD', '/clang:--target=x86_64-pc-windows-msvc',
          '/clang:-fobjc-runtime=gnustep-2.0', '/clang:-fms-runtime-lib=dll',
          '-x', 'objective-c++', '/std:c++20', '/EHsc', '/clang:-fobjc-version=3',
          ('/clang:-O' + $objc3Optimization), '/c', $objc3Source, ('/Fo' + $objc3Object))
      } else {
        $objc3CompileArguments = @($objc3Source, '--objc3', '--objcxx', '--target', 'x86_64-pc-windows-msvc',
          '--runtime', 'gnustep-2.0', '--emit-object', $objc3Object, '--',
          '-fms-runtime-lib=dll', '-std=c++20', '-fexceptions', '-fcxx-exceptions', ('-O' + $objc3Optimization))
      }
      $objc3Record.compile_command = @($objc3Compiler) + $objc3CompileArguments
      Write-Objc3FixtureResults 'running'
      & $objc3Compiler @objc3CompileArguments
      $objc3Record.compile_exit_code = $LASTEXITCODE
      if ($LASTEXITCODE -ne 0) { throw "Native $Suite compile failed: $ClientDriver $objc3Name ($LASTEXITCODE)" }
      $objc3Record.object_sha256 = (Get-FileHash -LiteralPath $objc3Object -Algorithm SHA256).Hash
      $objc3Record.status = 'linking'
      $objc3LinkArguments = @('--target=x86_64-pc-windows-msvc', '-fms-runtime-lib=dll', '-fuse-ld=lld',
        $objc3Object, '-o', $objc3Executable)
      $objc3Record.link_command = @($objc3LinkerDriver) + $objc3LinkArguments
      Write-Objc3FixtureResults 'running'
      & $objc3LinkerDriver @objc3LinkArguments
      $objc3Record.link_exit_code = $LASTEXITCODE
      if ($LASTEXITCODE -ne 0) { throw "Native $Suite link failed: $ClientDriver $objc3Name ($LASTEXITCODE)" }
      $objc3Record.executable_sha256 = (Get-FileHash -LiteralPath $objc3Executable -Algorithm SHA256).Hash
      $objc3Record.status = 'running'
      $objc3Record.runtime_command = @($objc3Executable)
      Write-Objc3FixtureResults 'running'
      & $objc3Executable
      $objc3Record.exit_code = $LASTEXITCODE
      if ($LASTEXITCODE -ne 0) { throw "Native $Suite runtime failed: $ClientDriver $objc3Name ($LASTEXITCODE)" }
      if ((Get-FileHash -LiteralPath $objc3Source -Algorithm SHA256).Hash -ne $objc3SourceHash -or
          ($objc3GeneratedSource -and
          (Get-FileHash -LiteralPath $objc3GeneratedSource -Algorithm SHA256).Hash -ne $objc3GeneratedHash)) {
        throw "Runtime witness changed during execution: $objc3Case"
      }
      $objc3Record.status = 'passed'
      Write-Objc3FixtureResults 'running'
      Write-Output "NATIVE $($Suite.ToUpperInvariant()) PASS ($ClientDriver): $objc3Name"
    }
  }
  if ((Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash -ne $objc3CompilerHash -or
      (Get-FileHash -LiteralPath $objc3LinkerDriver -Algorithm SHA256).Hash -ne $objc3LinkerHash -or
      (Get-FileHash -LiteralPath $objc3Lld -Algorithm SHA256).Hash -ne $objc3LldHash) {
    throw 'A compiler/linker executable changed during fixture execution.'
  }
  Write-Objc3FixtureResults 'passed'
  Write-Output "Results: $objc3Results"
} catch {
  if ($objc3Results) { Write-Objc3FixtureResults 'failed' $_.Exception.Message }
  throw
} finally {
  Pop-Location
}
