[CmdletBinding()]
param(
  [string]$ClangBin = 'out/toolchains/objc3-development/build/bin',
  [string]$NativeDriver = 'out/toolchains/objc3-facade/build/src/clang_foundation/objc3c-clang.exe'
)

$ErrorActionPreference = 'Stop'
$objc3Repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$objc3Python = 'python'
$objc3Results = $null
$objc3Identity = $null
$objc3Records = @()

function Write-Objc3ImportResults([string]$Status, [string]$Failure = '') {
  $objc3Report = @{} + $script:objc3Identity
  $objc3Report.status = $Status
  $objc3Report.fixtures = @($script:objc3Records)
  if ($Status -ne 'running') { $objc3Report.finished_utc = [DateTime]::UtcNow.ToString('o') }
  if ($Failure) { $objc3Report.error = $Failure }
  $objc3Report | ConvertTo-Json -Depth 15 | Set-Content -LiteralPath $script:objc3Results -Encoding utf8
}

function Invoke-Objc3ImportStage([hashtable]$Record, [string]$Stage, [string]$Executable,
    [string[]]$Arguments, [int]$ExpectedExit = 0) {
  $objc3Stage = @{stage=$Stage; command=@($Executable) + $Arguments; expected_exit_code=$ExpectedExit}
  $Record.commands += $objc3Stage
  Write-Objc3ImportResults 'running'
  # Expected rejection must remain reportable if the caller enabled native-error propagation.
  $PSNativeCommandUseErrorActionPreference = $false
  $objc3Output = & $Executable @Arguments 2>&1
  $objc3Stage.exit_code = $LASTEXITCODE
  $objc3Stage.output = ($objc3Output | Out-String)
  Write-Objc3ImportResults 'running'
  if ($objc3Stage.exit_code -ne $ExpectedExit) {
    throw "$($Record.name) $Stage returned $($objc3Stage.exit_code), expected $ExpectedExit. $($objc3Stage.output)"
  }
}

function Assert-Objc3OwnedImportPath([string]$Path) {
  $objc3Resolved = [IO.Path]::GetFullPath($Path)
  $objc3OwnedPrefix = [IO.Path]::GetFullPath($script:objc3Output).TrimEnd('\') + '\'
  if (-not $objc3Resolved.StartsWith($objc3OwnedPrefix, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Generated fixture path escapes this unique run directory: $objc3Resolved"
  }
}

Push-Location -LiteralPath $objc3Repo
try {
  $objc3Bin = (Resolve-Path -LiteralPath $ClangBin).Path
  if ($objc3Bin -ne (Resolve-Path -LiteralPath 'out/toolchains/objc3-development/build/bin').Path) {
    throw 'Source-import fixtures require the sealed development compiler.'
  }
  if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir -or
      -not $env:VCToolsVersion -or -not $env:WindowsSDKVersion -or
      $env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207' -or
      $env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') {
    throw 'Load the admitted Visual Studio v143 / Windows SDK developer environment.'
  }
  $env:PYTHONDONTWRITEBYTECODE = '1'
  $objc3SealJson = & $objc3Python -B -m scripts.objc3c_toolchain verify-development
  if ($LASTEXITCODE -ne 0) { throw 'Source-import source/build identity did not verify.' }
  $objc3Seal = ($objc3SealJson -join "`n") | ConvertFrom-Json
  & $objc3Python -B (Join-Path $PSScriptRoot 'verify_runtime_profile.py') --profile windows-x86_64 `
    --source (Resolve-Path -LiteralPath 'out/toolchains/libobjc2-v2.3/src').Path `
    --robin-map-source (Resolve-Path -LiteralPath 'out/toolchains/robin-map-v1.4.0/src').Path --bin-dir $objc3Bin
  if ($LASTEXITCODE -ne 0) { throw 'Pinned native compiler/runtime profile validation failed.' }
  $objc3Compiler = (Resolve-Path -LiteralPath $NativeDriver).Path
  $objc3Marker = 'ObjC3-patches=' + $objc3Seal.patch_digest
  $objc3MarkerCheck = 'from pathlib import Path; import sys; sys.exit(0 if sys.argv[2].encode() in Path(sys.argv[1]).read_bytes() else 1)'
  & $objc3Python -B -c $objc3MarkerCheck $objc3Compiler $objc3Marker
  if ($LASTEXITCODE -ne 0) { throw 'Native facade lacks the sealed linked-Clang binary marker.' }
  $objc3LinkDriver = Join-Path $objc3Bin 'clang++.exe'
  $objc3LinkVersion = (& $objc3LinkDriver --version) -join "`n"
  if ($LASTEXITCODE -ne 0 -or -not $objc3LinkVersion.Contains($objc3Marker)) {
    throw 'Actual linking driver differs from the sealed patch identity.'
  }
  $objc3Lld = Join-Path $objc3Bin 'lld-link.exe'
  $objc3CompilerHash = (Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash
  $objc3LinkHash = (Get-FileHash -LiteralPath $objc3LinkDriver -Algorithm SHA256).Hash
  $objc3LldHash = (Get-FileHash -LiteralPath $objc3Lld -Algorithm SHA256).Hash
  $objc3RunId = [guid]::NewGuid().ToString('N')
  $objc3Output = Join-Path $objc3Repo ('out/toolchains/objc3-development/native-imports/' + $objc3RunId)
  New-Item -ItemType Directory -Path $objc3Output | Out-Null
  $objc3Results = Join-Path $objc3Output 'results.json'
  $objc3Identity = @{schema_version=1; suite='native-source-imports'; client_driver='objc3c-clang';
    target_triple='x86_64-pc-windows-msvc'; build_identity=$objc3Seal.build_id; patch_digest=$objc3Seal.patch_digest;
    run_id=$objc3RunId; compiler=$objc3Compiler; compiler_sha256=$objc3CompilerHash;
    compiler_version=('linked Clang binary marker: ' + $objc3Marker);
    linker_driver=$objc3LinkDriver; linker_driver_sha256=$objc3LinkHash; linker_driver_version=$objc3LinkVersion;
    linker=$objc3Lld; linker_sha256=$objc3LldHash; vc_tools_version=$env:VCToolsVersion;
    windows_sdk_version=$env:WindowsSDKVersion;
    runner_sha256=(Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash;
    started_utc=[DateTime]::UtcNow.ToString('o')}
  Write-Objc3ImportResults 'running'
  foreach ($objc3Subdirectory in @('sources', 'interfaces', 'archive', 'eager', 'source-directed')) {
    New-Item -ItemType Directory -Path (Join-Path $objc3Output $objc3Subdirectory) | Out-Null
  }
  $objc3Source = Join-Path $objc3Output 'sources/provider.m'
  $objc3ArchivedSource = Join-Path $objc3Output 'archive/provider.m.txt'
  $objc3Interfaces = Join-Path $objc3Output 'interfaces'
  $objc3Interface = Join-Path $objc3Interfaces 'Runtime.objc3i'
  $objc3ProviderObject = Join-Path $objc3Output 'provider.obj'
  $objc3Utf8 = [Text.UTF8Encoding]::new($false)
  [IO.File]::WriteAllText($objc3Source, "typedef int Answer;`nAnswer answer(void) { return 42; }`n", $objc3Utf8)
  $objc3Provider = @{name='provider'; status='compiling'; commands=@(); source=$objc3Source;
    source_sha256=(Get-FileHash -LiteralPath $objc3Source -Algorithm SHA256).Hash}
  $objc3Records += $objc3Provider
  $objc3ModeArguments = @('--objc3', '--target', 'x86_64-pc-windows-msvc', '--runtime', 'gnustep-2.0')
  Invoke-Objc3ImportStage $objc3Provider 'compile' $objc3Compiler (@($objc3Source) + $objc3ModeArguments +
    @('--emit-object', $objc3ProviderObject, '--emit-interface', $objc3Interface,
      '--module', 'fixture.Runtime', '--package', 'source-import-runtime-fixture', '--', '-fms-runtime-lib=dll', '-O2'))
  $objc3Sidecar = (Get-Content -LiteralPath ($objc3Interface + '.json') -Raw) | ConvertFrom-Json
  $objc3Module = $objc3Sidecar.module
  if ($objc3Module.canonical_name -ne 'fixture.Runtime' -or
      $objc3Module.package_identity -ne 'source-import-runtime-fixture' -or
      $objc3Module.semantic_fingerprint -notmatch '^[0-9a-f]{64}$') {
    throw 'Provider emission did not produce the requested complete module lock.'
  }
  $objc3Provider.object_sha256 = (Get-FileHash -LiteralPath $objc3ProviderObject -Algorithm SHA256).Hash
  $objc3Provider.interface_sha256 = (Get-FileHash -LiteralPath $objc3Interface -Algorithm SHA256).Hash
  $objc3Provider.sidecar_sha256 = (Get-FileHash -LiteralPath ($objc3Interface + '.json') -Algorithm SHA256).Hash
  $objc3Provider.module_lock = $objc3Module
  # Move only this runner's own generated source, after checking both absolute paths.
  Assert-Objc3OwnedImportPath $objc3Source
  Assert-Objc3OwnedImportPath $objc3ArchivedSource
  Move-Item -LiteralPath $objc3Source -Destination $objc3ArchivedSource
  if ((Test-Path -LiteralPath $objc3Source) -or
      (Get-FileHash -LiteralPath $objc3ArchivedSource -Algorithm SHA256).Hash -ne $objc3Provider.source_sha256) {
    throw 'Provider source was not isolated intact from consumer compilation.'
  }
  $objc3Provider.archived_source = $objc3ArchivedSource
  $objc3Provider.status = 'passed'
  foreach ($objc3Mode in @('eager', 'source-directed')) {
    $objc3ModeOutput = Join-Path $objc3Output $objc3Mode
    $objc3Consumer = Join-Path $objc3Output ('sources/consumer-' + $objc3Mode + '.m')
    $objc3Text = if ($objc3Mode -eq 'source-directed') {
      "@import fixture.Runtime;`n@fixture.Runtime.Answer global_answer;`n" +
        "int main(void) { @fixture.Runtime.Answer value = @fixture.Runtime.answer(); " +
        "return value == 42 && global_answer == 0 ? 0 : 1; }`n"
    } else { "int main(void) { Answer value = answer(); return value == 42 ? 0 : 1; }`n" }
    [IO.File]::WriteAllText($objc3Consumer, $objc3Text, $objc3Utf8)
    $objc3Object = Join-Path $objc3ModeOutput 'consumer.obj'
    $objc3Dependency = Join-Path $objc3ModeOutput 'dependencies.json'
    $objc3Executable = Join-Path $objc3ModeOutput 'consumer.exe'
    $objc3Record = @{name=$objc3Mode; status='compiling'; commands=@(); source=$objc3Consumer;
      source_sha256=(Get-FileHash -LiteralPath $objc3Consumer -Algorithm SHA256).Hash;
      provider_source_absent_at_consumer_compile=(-not (Test-Path -LiteralPath $objc3Source))}
    $objc3Records += $objc3Record
    if (-not $objc3Record.provider_source_absent_at_consumer_compile) { throw 'Provider source reappeared before consumer compile.' }
    $objc3ImportArguments = @($objc3Consumer) + $objc3ModeArguments + @('--module-search-path', $objc3Interfaces,
      '--import-module', $objc3Module.canonical_name, $objc3Module.semantic_fingerprint, $objc3Module.package_identity)
    if ($objc3Mode -eq 'source-directed') { $objc3ImportArguments += '--source-imports' }
    Invoke-Objc3ImportStage $objc3Record 'compile' $objc3Compiler ($objc3ImportArguments +
      @('--emit-object', $objc3Object, '--emit-dependency-record', $objc3Dependency, '--', '-fms-runtime-lib=dll', '-O2'))
    $objc3Dependencies = (Get-Content -LiteralPath $objc3Dependency -Raw) | ConvertFrom-Json
    $objc3Imported = @($objc3Dependencies.modules | Where-Object { $_.name -eq $objc3Module.canonical_name })
    $objc3SealedDriver = Join-Path $objc3Bin 'clang.exe'
    if ($objc3Dependencies.schema -ne 'objc3c.dependency-record.v2' -or $objc3Imported.Count -ne 1 -or
        $objc3Imported[0].fingerprint -ne $objc3Module.semantic_fingerprint -or
        $objc3Imported[0].package -ne $objc3Module.package_identity -or
        $objc3Dependencies.toolchain.patch_digest -ne $objc3Seal.patch_digest -or
        (Resolve-Path -LiteralPath $objc3Dependencies.toolchain.driver_path).Path -ne $objc3SealedDriver -or
        $objc3Dependencies.toolchain.driver_sha256 -ne (Get-FileHash -LiteralPath $objc3SealedDriver -Algorithm SHA256).Hash) {
      throw 'Consumer dependency record differs from the selected provider lock or sealed compiler.'
    }
    $objc3Record.object_sha256 = (Get-FileHash -LiteralPath $objc3Object -Algorithm SHA256).Hash
    $objc3Record.dependency_record = $objc3Dependency
    $objc3Record.dependency_record_sha256 = (Get-FileHash -LiteralPath $objc3Dependency -Algorithm SHA256).Hash
    Invoke-Objc3ImportStage $objc3Record 'link' $objc3LinkDriver @('--target=x86_64-pc-windows-msvc',
      '-fms-runtime-lib=dll', '-fuse-ld=lld', $objc3ProviderObject, $objc3Object, '-o', $objc3Executable)
    $objc3Record.executable_sha256 = (Get-FileHash -LiteralPath $objc3Executable -Algorithm SHA256).Hash
    Invoke-Objc3ImportStage $objc3Record 'run' $objc3Executable @()
    $objc3BadFingerprint = $(if ($objc3Module.semantic_fingerprint[0] -eq '0') { '1' } else { '0' }) +
      $objc3Module.semantic_fingerprint.Substring(1)
    $objc3BadArguments = @($objc3Consumer) + $objc3ModeArguments + @('--module-search-path', $objc3Interfaces,
      '--import-module', $objc3Module.canonical_name, $objc3BadFingerprint, $objc3Module.package_identity)
    if ($objc3Mode -eq 'source-directed') { $objc3BadArguments += '--source-imports' }
    $objc3BadObject = Join-Path $objc3ModeOutput 'wrong-lock.obj'
    $objc3BadDependency = Join-Path $objc3ModeOutput 'wrong-lock.json'
    [IO.File]::WriteAllText($objc3BadObject, 'preserved object', $objc3Utf8)
    [IO.File]::WriteAllText($objc3BadDependency, 'preserved dependency', $objc3Utf8)
    Invoke-Objc3ImportStage $objc3Record 'reject-wrong-lock' $objc3Compiler ($objc3BadArguments +
      @('--emit-object', $objc3BadObject, '--emit-dependency-record', $objc3BadDependency, '--', '-fms-runtime-lib=dll')) 1
    if ([IO.File]::ReadAllText($objc3BadObject) -ne 'preserved object' -or
        [IO.File]::ReadAllText($objc3BadDependency) -ne 'preserved dependency') {
      throw 'Rejected wrong-lock compile changed previously published outputs.'
    }
    $objc3Record.wrong_lock_outputs_preserved = $true
    $objc3Record.status = 'passed'
    Write-Objc3ImportResults 'running'
    Write-Output "NATIVE SOURCE IMPORT PASS (objc3c-clang): $objc3Mode"
  }
  if ((Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash -ne $objc3CompilerHash -or
      (Get-FileHash -LiteralPath $objc3LinkDriver -Algorithm SHA256).Hash -ne $objc3LinkHash -or
      (Get-FileHash -LiteralPath $objc3Lld -Algorithm SHA256).Hash -ne $objc3LldHash -or
      (Get-FileHash -LiteralPath $objc3ProviderObject -Algorithm SHA256).Hash -ne $objc3Provider.object_sha256 -or
      (Get-FileHash -LiteralPath $objc3Interface -Algorithm SHA256).Hash -ne $objc3Provider.interface_sha256 -or
      (Get-FileHash -LiteralPath ($objc3Interface + '.json') -Algorithm SHA256).Hash -ne $objc3Provider.sidecar_sha256) {
    throw 'A compiler/linker executable or provider artifact changed during the fixture run.'
  }
  Write-Objc3ImportResults 'passed'
  Write-Output "Results: $objc3Results"
} catch {
  if ($objc3Results) { Write-Objc3ImportResults 'failed' $_.Exception.Message }
  throw
} finally {
  Pop-Location
}
