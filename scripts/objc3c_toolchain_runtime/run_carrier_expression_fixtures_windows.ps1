[CmdletBinding()]
param(
  [string]$ClangBin = 'out/toolchains/objc3-development/build/bin',
  [ValidateSet('clang', 'clang-cl')][string]$ClientDriver = 'clang'
)

$ErrorActionPreference = 'Stop'
$objc3Repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$objc3Bin = (Resolve-Path $ClangBin).Path
if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir) {
  throw 'Load the admitted Visual Studio developer environment before running carrier fixtures.'
}
if ($env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207' -or
    $env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') {
  throw 'The active developer environment differs from the admitted toolchain.'
}
$env:PYTHONDONTWRITEBYTECODE = '1'
Push-Location -LiteralPath $objc3Repo
try {
  $objc3SealJson = & 'python' -B -m scripts.objc3c_toolchain verify-development
  if ($LASTEXITCODE -ne 0) { throw 'Carrier fixture source/build identity did not verify.' }
  $objc3Seal = ($objc3SealJson -join "`n") | ConvertFrom-Json
  $objc3ExpectedBin = (Resolve-Path 'out/toolchains/objc3-development/build/bin').Path
  if ($objc3Bin -ne $objc3ExpectedBin) {
    throw 'These private native expression fixtures require the sealed development compiler.'
  }
  $objc3RuntimeSource = (Resolve-Path 'out/toolchains/libobjc2-v2.3/src').Path
  $objc3RobinSource = (Resolve-Path 'out/toolchains/robin-map-v1.4.0/src').Path
  & 'python' -B (Join-Path $PSScriptRoot 'verify_runtime_profile.py') --profile windows-x86_64 --source $objc3RuntimeSource --robin-map-source $objc3RobinSource --bin-dir $objc3Bin
  if ($LASTEXITCODE -ne 0) { throw 'Pinned native compiler/runtime profile validation failed.' }

  $objc3Inputs = Join-Path $objc3Repo 'out/toolchains/objc3-development/src/clang/test/ObjC3/CodeGen/Inputs'
  $objc3Output = Join-Path $objc3Repo ('out/toolchains/objc3-development/carrier-expression-fixtures/' + $objc3Seal.build_id + '/' + $ClientDriver)
  New-Item -ItemType Directory -Force -Path $objc3Output | Out-Null
  $objc3Compiler = Join-Path $objc3Bin ($ClientDriver + '.exe')
  $objc3CompilerVersion = (& $objc3Compiler --version) -join "`n"
  if ($LASTEXITCODE -ne 0 -or
      -not $objc3CompilerVersion.Contains('ObjC3-patches=' + $objc3Seal.patch_digest)) {
    throw 'The actual compiler executable does not match the sealed patch identity.'
  }
  $objc3CompilerHash = (Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash
  $objc3Started = [DateTime]::UtcNow.ToString('o')
  $objc3Records = @()
  $objc3Results = Join-Path $objc3Output 'results.json'
  @{status='running'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
    compiler_sha256=$objc3CompilerHash; started_utc=$objc3Started} |
    ConvertTo-Json | Set-Content -LiteralPath $objc3Results -Encoding utf8
  $objc3Cases = @('optional-expression-runtime', 'optional-copy-runtime',
    'result-expression-runtime', 'carrier-copy-adversarial-runtime',
    'carrier-expression-adversarial-runtime', 'defer-public-runtime')
  foreach ($objc3Case in $objc3Cases) {
    $objc3Source = Join-Path $objc3Inputs ($objc3Case + '.c')
    $objc3SourceHash = (Get-FileHash -LiteralPath $objc3Source -Algorithm SHA256).Hash
    foreach ($objc3Optimization in @('0', '2')) {
      $objc3Name = $objc3Case + '-O' + $objc3Optimization
      $objc3Executable = Join-Path $objc3Output ($objc3Name + '.exe')
      $objc3Object = Join-Path $objc3Output ($objc3Name + '.obj')
      if ($ClientDriver -eq 'clang') {
        $objc3Arguments = @('--target=x86_64-pc-windows-msvc', '-fobjc-runtime=gnustep-2.0',
          '-fms-runtime-lib=dll', '-fuse-ld=lld', '-xobjective-c', '-std=c17',
          '-fobjc-version=3', ('-O' + $objc3Optimization), $objc3Source,
          '-o', $objc3Executable)
      } else {
        $objc3Arguments = @('/nologo', '/MD', '/clang:--target=x86_64-pc-windows-msvc',
          '/clang:-fobjc-runtime=gnustep-2.0', '/clang:-fms-runtime-lib=dll',
          '/clang:-fuse-ld=lld', '-x', 'objective-c', '/clang:-std=c17',
          '/clang:-fobjc-version=3', ('/clang:-O' + $objc3Optimization),
          $objc3Source, ('/Fo' + $objc3Object), ('/Fe' + $objc3Executable))
      }
      & $objc3Compiler @objc3Arguments
      if ($LASTEXITCODE -ne 0) { throw "Carrier compile/link failed: $ClientDriver $objc3Name ($LASTEXITCODE)" }
      & $objc3Executable
      if ($LASTEXITCODE -ne 0) { throw "Carrier runtime failed: $ClientDriver $objc3Name ($LASTEXITCODE)" }
      $objc3Records += @{name=$objc3Name; source=$objc3Source; source_sha256=$objc3SourceHash;
        command=@($objc3Compiler) + $objc3Arguments; exit_code=$LASTEXITCODE;
        executable_sha256=(Get-FileHash -LiteralPath $objc3Executable -Algorithm SHA256).Hash}
      Write-Output "CARRIER EXPRESSION PASS ($ClientDriver): $objc3Name"
    }
  }
  @{status='passed'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
    compiler_sha256=$objc3CompilerHash; compiler_version=$objc3CompilerVersion; started_utc=$objc3Started;
    finished_utc=[DateTime]::UtcNow.ToString('o'); fixtures=$objc3Records} |
    ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $objc3Results -Encoding utf8
} catch {
  if ($objc3Results) {
    @{status='failed'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
      compiler_sha256=$objc3CompilerHash; started_utc=$objc3Started;
      finished_utc=[DateTime]::UtcNow.ToString('o'); fixtures=$objc3Records;
      error=$_.Exception.Message} | ConvertTo-Json -Depth 8 |
      Set-Content -LiteralPath $objc3Results -Encoding utf8
  }
  throw
} finally {
  Pop-Location
}
