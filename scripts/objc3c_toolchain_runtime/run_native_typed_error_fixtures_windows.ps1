[CmdletBinding()]
param(
  [string]$ClangBin = 'out/toolchains/objc3-development/build/bin',
  [ValidateSet('clang', 'clang-cl')][string]$ClientDriver = 'clang'
)

$ErrorActionPreference = 'Stop'
$objc3Repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$objc3Bin = (Resolve-Path $ClangBin).Path
$objc3SavedPath = $env:PATH
$objc3Records = @()
if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir -or
    $env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207' -or
    $env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') {
  throw 'Load the admitted Visual Studio 2022 developer environment first.'
}
$env:PYTHONDONTWRITEBYTECODE = '1'
Push-Location -LiteralPath $objc3Repo
try {
  $objc3SealJson = & 'C:\Python313\python.exe' -B -m scripts.objc3c_toolchain verify-development
  if ($LASTEXITCODE -ne 0) { throw 'Typed-error fixture compiler source identity did not verify.' }
  $objc3Seal = ($objc3SealJson -join "`n") | ConvertFrom-Json
  if ($objc3Bin -ne (Resolve-Path 'out/toolchains/objc3-development/build/bin').Path) {
    throw 'Typed-error fixtures require the sealed development compiler.'
  }
  $objc3RuntimeSource = (Resolve-Path 'out/toolchains/libobjc2-v2.3/src').Path
  $objc3RobinSource = (Resolve-Path 'out/toolchains/robin-map-v1.4.0/src').Path
  & 'C:\Python313\python.exe' -B (Join-Path $PSScriptRoot 'verify_runtime_profile.py') --profile windows-x86_64 --source $objc3RuntimeSource --robin-map-source $objc3RobinSource --bin-dir $objc3Bin
  if ($LASTEXITCODE -ne 0) { throw 'Pinned compiler/runtime profile validation failed.' }
  $objc3Runtime = (Resolve-Path 'out/toolchains/libobjc2-v2.3/build/stage').Path
  $objc3RuntimeDll = Join-Path $objc3Runtime 'objc.dll'
  $objc3RuntimeImport = Join-Path $objc3Runtime 'objc.lib'
  $objc3Includes = @('-I', $objc3RuntimeSource, '-I', (Join-Path $objc3Runtime 'objc'))
  foreach ($objc3Required in @($objc3RuntimeDll, $objc3RuntimeImport,
      (Join-Path $objc3Runtime 'objc/objc-config.h'))) {
    if (-not (Test-Path -LiteralPath $objc3Required)) { throw "Missing native runtime input: $objc3Required" }
  }
  $objc3InputRoot = Join-Path $objc3Repo 'out/toolchains/objc3-development/src/clang/test/ObjC3/CodeGen/Inputs'
  $objc3Source = Join-Path $objc3InputRoot 'typed-error-out-runtime.mm'
  $objc3Support = Join-Path $objc3InputRoot 'typed-error-native-runtime-support.m'
  $objc3Output = Join-Path $objc3Repo ('out/toolchains/objc3-development/native-typed-error-fixtures/' + $objc3Seal.build_id + '/' + $ClientDriver)
  New-Item -ItemType Directory -Force -Path $objc3Output | Out-Null
  $objc3Compiler = Join-Path $objc3Bin $(if ($ClientDriver -eq 'clang') { 'clang++.exe' } else { 'clang-cl.exe' })
  $objc3SupportCompiler = Join-Path $objc3Bin $(if ($ClientDriver -eq 'clang') { 'clang.exe' } else { 'clang-cl.exe' })
  $objc3CompilerVersion = (& $objc3Compiler --version) -join "`n"
  if ($LASTEXITCODE -ne 0 -or -not $objc3CompilerVersion.Contains('ObjC3-patches=' + $objc3Seal.patch_digest)) {
    throw 'Actual typed-error compiler does not match the source seal.'
  }
  $objc3SupportVersion = (& $objc3SupportCompiler --version) -join "`n"
  if ($LASTEXITCODE -ne 0 -or -not $objc3SupportVersion.Contains('ObjC3-patches=' + $objc3Seal.patch_digest)) {
    throw 'Actual MRC compiler does not match the source seal.'
  }
  $objc3CompilerHash = (Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash
  $objc3SupportCompilerHash = (Get-FileHash -LiteralPath $objc3SupportCompiler -Algorithm SHA256).Hash
  $objc3RuntimeDllHash = (Get-FileHash -LiteralPath $objc3RuntimeDll -Algorithm SHA256).Hash
  $objc3RuntimeImportHash = (Get-FileHash -LiteralPath $objc3RuntimeImport -Algorithm SHA256).Hash
  $objc3Started = [DateTime]::UtcNow.ToString('o')
  $objc3Results = Join-Path $objc3Output 'results.json'
  @{status='running'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
    compiler_sha256=$objc3CompilerHash; started_utc=$objc3Started} |
    ConvertTo-Json | Set-Content -LiteralPath $objc3Results -Encoding utf8
  $env:PATH = "$objc3Runtime;$objc3SavedPath"
  foreach ($objc3Optimization in @('0', '2')) {
    $objc3Name = 'typed-error-out-runtime-O' + $objc3Optimization
    $objc3Executable = Join-Path $objc3Output ($objc3Name + '.exe')
    $objc3Object = Join-Path $objc3Output ($objc3Name + '.obj')
    $objc3SupportObject = Join-Path $objc3Output ($objc3Name + '-support.obj')
    if ($ClientDriver -eq 'clang') {
      $objc3Common = @('--target=x86_64-pc-windows-msvc', '-fobjc-runtime=gnustep-2.0',
        '-fms-runtime-lib=dll', ('-O' + $objc3Optimization)) + $objc3Includes
      $objc3SupportArguments = $objc3Common + @('-x', 'objective-c', '-fno-objc-arc',
        '-c', $objc3Support, '-o', $objc3SupportObject)
      $objc3Arguments = $objc3Common + @('-x', 'objective-c++', '-std=c++20',
        '-fexceptions', '-fcxx-exceptions', '-fobjc-arc', '-fobjc-arc-exceptions',
        '-fobjc-version=3', '-c', $objc3Source, '-o', $objc3Object)
      $objc3LinkArguments = @('--target=x86_64-pc-windows-msvc', '-fms-runtime-lib=dll',
        '-fuse-ld=lld', $objc3SupportObject, $objc3Object, '-L', $objc3Runtime,
        '-lobjc', '-o', $objc3Executable)
    } else {
      $objc3Common = @('/nologo', '/MD', '/clang:--target=x86_64-pc-windows-msvc',
        '/clang:-fobjc-runtime=gnustep-2.0', ('/clang:-O' + $objc3Optimization)) + $objc3Includes
      $objc3SupportArguments = $objc3Common + @('-x', 'objective-c', '/clang:-fno-objc-arc',
        '/c', $objc3Support, ('/Fo' + $objc3SupportObject))
      $objc3Arguments = $objc3Common + @('-x', 'objective-c++', '/std:c++20', '/EHsc',
        '/clang:-fobjc-arc', '/clang:-fobjc-arc-exceptions', '/clang:-fobjc-version=3',
        '/c', $objc3Source, ('/Fo' + $objc3Object))
      $objc3LinkArguments = @('/nologo', '/MD', '/clang:--target=x86_64-pc-windows-msvc',
        '/clang:-fuse-ld=lld', $objc3SupportObject, $objc3Object, ('/Fe' + $objc3Executable),
        '/link', ('/LIBPATH:' + $objc3Runtime), 'objc.lib')
    }
    $objc3Sources = @($objc3Source, $objc3Support,
      (Join-Path $objc3InputRoot 'typed-error-runtime-interface.h'),
      (Join-Path $objc3InputRoot 'typed-error-do-catch-runtime.inc')) | ForEach-Object {
        @{file=$_; sha256=(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash}
      }
    $objc3Attempt = @{name=$objc3Name; source=$objc3Source; sources=@($objc3Sources);
      executable=$objc3Executable; status='running';
      source_sha256=(Get-FileHash -LiteralPath $objc3Source -Algorithm SHA256).Hash;
      command=@($objc3Compiler) + $objc3Arguments;
      support_command=@($objc3SupportCompiler) + $objc3SupportArguments;
      link_command=@($objc3Compiler) + $objc3LinkArguments}
    $objc3Records += $objc3Attempt
    $objc3Attempt.phase = 'mrc-companion-compile'
    & $objc3SupportCompiler @objc3SupportArguments
    $objc3Attempt.exit_code = $LASTEXITCODE
    if ($LASTEXITCODE -ne 0) { throw "MRC companion compile failed: $objc3Name ($LASTEXITCODE)" }
    $objc3Attempt.support_object_sha256 = (Get-FileHash -LiteralPath $objc3SupportObject -Algorithm SHA256).Hash
    $objc3Attempt.phase = 'native-typed-error-compile'
    & $objc3Compiler @objc3Arguments
    $objc3Attempt.exit_code = $LASTEXITCODE
    if ($LASTEXITCODE -ne 0) { throw "Typed-error compile failed: $objc3Name ($LASTEXITCODE)" }
    $objc3Attempt.object_sha256 = (Get-FileHash -LiteralPath $objc3Object -Algorithm SHA256).Hash
    $objc3Attempt.phase = 'link'
    & $objc3Compiler @objc3LinkArguments
    $objc3Attempt.exit_code = $LASTEXITCODE
    if ($LASTEXITCODE -ne 0) { throw "Typed-error link failed: $objc3Name ($LASTEXITCODE)" }
    $objc3Attempt.executable_sha256 = (Get-FileHash -LiteralPath $objc3Executable -Algorithm SHA256).Hash
    $objc3Attempt.phase = 'execute'
    & $objc3Executable
    $objc3Attempt.exit_code = $LASTEXITCODE
    if ($LASTEXITCODE -ne 0) { throw "Typed-error runtime failed: $objc3Name ($LASTEXITCODE)" }
    $objc3Attempt.status = 'passed'
    Write-Output "NATIVE TYPED ERROR PASS ($ClientDriver): $objc3Name"
    foreach ($objc3TrapCase in @('dynamic-nil', 'force-error')) {
      $objc3TrapName = 'typed-error-' + $objc3TrapCase + '-trap-O' + $objc3Optimization
      $objc3TrapJson = & 'C:\Python313\python.exe' -B (Join-Path $PSScriptRoot 'run_expected_trap_windows.py') $objc3Executable $objc3TrapCase
      if ($LASTEXITCODE -ne 0) { throw "Expected native trap failed: $objc3TrapName ($objc3TrapJson)" }
      $objc3TrapResult = ($objc3TrapJson -join "`n") | ConvertFrom-Json
      $objc3TrapAttempt = $objc3Attempt.Clone()
      $objc3TrapAttempt.name = $objc3TrapName
      $objc3TrapAttempt.phase = 'execute-expected-trap'
      $objc3TrapAttempt.exit_code = $objc3TrapResult.exit_code
      $objc3TrapAttempt.expected_exit_code = $objc3TrapResult.expected_exit_code
      $objc3TrapAttempt.runtime_command = @($objc3TrapResult.command)
      $objc3TrapAttempt.status = $objc3TrapResult.status
      $objc3Records += $objc3TrapAttempt
      Write-Output "NATIVE TYPED ERROR TRAP PASS ($ClientDriver): $objc3TrapName"
    }
  }
  @{status='passed'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
    compiler_sha256=$objc3CompilerHash; compiler_version=$objc3CompilerVersion;
    support_compiler_sha256=$objc3SupportCompilerHash; support_compiler_version=$objc3SupportVersion;
    runtime_dll_sha256=$objc3RuntimeDllHash;
    runtime_import_sha256=$objc3RuntimeImportHash; started_utc=$objc3Started;
    finished_utc=[DateTime]::UtcNow.ToString('o'); fixtures=$objc3Records} |
    ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $objc3Results -Encoding utf8
} catch {
  if ($objc3Attempt -and $objc3Attempt.status -eq 'running') { $objc3Attempt.status = 'failed' }
  if ($objc3Results) {
    @{status='failed'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
      compiler_sha256=$objc3CompilerHash; started_utc=$objc3Started;
      finished_utc=[DateTime]::UtcNow.ToString('o'); fixtures=$objc3Records;
      error=$_.Exception.Message} | ConvertTo-Json -Depth 8 |
      Set-Content -LiteralPath $objc3Results -Encoding utf8
  }
  throw
} finally {
  $env:PATH = $objc3SavedPath
  Pop-Location
}
