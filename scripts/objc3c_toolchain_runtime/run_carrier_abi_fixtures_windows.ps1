[CmdletBinding()]
param(
  [string]$ClangBin = 'out/toolchains/objc3-development/build/bin',
  [ValidateSet('clang', 'clang-cl')][string]$ClientDriver = 'clang'
)

$ErrorActionPreference = 'Stop'
$objc3Repo = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$objc3Bin = (Resolve-Path $ClangBin).Path
if (-not $env:VCToolsInstallDir -or -not $env:WindowsSdkDir -or
    $env:VCToolsVersion.TrimEnd('\') -ne '14.44.35207' -or
    $env:WindowsSDKVersion.TrimEnd('\') -ne '10.0.26100.0') {
  throw 'Load the admitted Visual Studio developer environment before running carrier ABI fixtures.'
}
$env:PYTHONDONTWRITEBYTECODE = '1'
Push-Location -LiteralPath $objc3Repo
try {
  $objc3SealJson = & 'python' -B -m scripts.objc3c_toolchain verify-development
  if ($LASTEXITCODE -ne 0) { throw 'Carrier ABI source/build identity did not verify.' }
  $objc3Seal = ($objc3SealJson -join "`n") | ConvertFrom-Json
  if ($objc3Bin -ne (Resolve-Path 'out/toolchains/objc3-development/build/bin').Path) {
    throw 'These private ABI fixtures require the sealed development compiler.'
  }
  $objc3Compiler = Join-Path $objc3Bin ($ClientDriver + '.exe')
  $objc3Version = (& $objc3Compiler --version) -join "`n"
  if ($LASTEXITCODE -ne 0 -or
      -not $objc3Version.Contains('ObjC3-patches=' + $objc3Seal.patch_digest)) {
    throw 'The actual compiler does not match the sealed patch identity.'
  }
  $objc3CompilerHash = (Get-FileHash -LiteralPath $objc3Compiler -Algorithm SHA256).Hash
  $objc3LinkerHash = (Get-FileHash -LiteralPath (Join-Path $objc3Bin 'lld-link.exe') -Algorithm SHA256).Hash
  $objc3Inputs = Join-Path $objc3Repo 'out/toolchains/objc3-development/src/clang/test/ObjC3/CodeGen/Inputs'
  $objc3Output = Join-Path $objc3Repo ('out/toolchains/objc3-development/carrier-abi-fixtures/' + $objc3Seal.build_id + '/' + $ClientDriver)
  New-Item -ItemType Directory -Force -Path $objc3Output | Out-Null
  $objc3Results = Join-Path $objc3Output 'results.json'
  $objc3Started = [DateTime]::UtcNow.ToString('o')
  $objc3Records = @()
  @{status='running'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
    compiler_sha256=$objc3CompilerHash; linker_sha256=$objc3LinkerHash; started_utc=$objc3Started} |
    ConvertTo-Json | Set-Content -LiteralPath $objc3Results -Encoding utf8
  $objc3Cases = @(
    @{name='extension-provider-consumer'; header='carrier-native-abi.h';
      units=@(@{file='carrier-native-abi-runtime-provider.c'; language='objective-c'},
              @{file='carrier-native-abi-runtime-main.c'; language='objective-c'})},
    @{name='ordinary-c-active-payload'; header='carrier-native-abi-c-mirror.h';
      units=@(@{file='carrier-native-abi-c-provider.c'; language='objective-c'},
              @{file='carrier-native-abi-c-client.c'; language='c'})}
  )
  foreach ($objc3Case in $objc3Cases) {
    foreach ($objc3Optimization in @('0', '2')) {
      $objc3Name = $objc3Case.name + '-O' + $objc3Optimization
      $objc3Executable = Join-Path $objc3Output ($objc3Name + '.exe')
      $objc3Commands = @()
      $objc3Objects = @()
      $objc3Sources = @()
      foreach ($objc3Unit in $objc3Case.units) {
        $objc3Source = Join-Path $objc3Inputs $objc3Unit.file
        $objc3Object = Join-Path $objc3Output ($objc3Name + '-' + $objc3Unit.file + '.obj')
        $objc3Sources += @{file=$objc3Source; language=$objc3Unit.language;
          sha256=(Get-FileHash -LiteralPath $objc3Source -Algorithm SHA256).Hash}
        if ($ClientDriver -eq 'clang') {
          $objc3Arguments = @('--target=x86_64-pc-windows-msvc', '-fms-runtime-lib=dll', '-x', $objc3Unit.language,
            '-std=c17', ('-O' + $objc3Optimization), '-I', $objc3Inputs, '-c',
            $objc3Source, '-o', $objc3Object)
          if ($objc3Unit.language -eq 'objective-c') {
            $objc3Arguments = @('-fobjc-version=3', '-fobjc-runtime=gnustep-2.0') + $objc3Arguments
          }
        } else {
          $objc3Arguments = @('/nologo', '/MD', '--target=x86_64-pc-windows-msvc', '-x', $objc3Unit.language,
            '/clang:-std=c17', ('/clang:-O' + $objc3Optimization), '/I', $objc3Inputs,
            '/c', $objc3Source, ('/Fo' + $objc3Object))
          if ($objc3Unit.language -eq 'objective-c') {
            $objc3Arguments = @('/clang:-fobjc-version=3', '/clang:-fobjc-runtime=gnustep-2.0') + $objc3Arguments
          }
        }
        & $objc3Compiler @objc3Arguments
        $objc3CompileExit = $LASTEXITCODE
        $objc3Commands += @{argv=@($objc3Compiler) + $objc3Arguments; exit_code=$objc3CompileExit}
        if ($objc3CompileExit -ne 0) { throw "Carrier ABI compile failed: $ClientDriver $objc3Name $($objc3Unit.file) ($objc3CompileExit)" }
        $objc3Objects += $objc3Object
      }
      if ($ClientDriver -eq 'clang') {
        $objc3LinkArguments = @('--target=x86_64-pc-windows-msvc', '-fuse-ld=lld', '-fms-runtime-lib=dll') +
          $objc3Objects + @('-o', $objc3Executable)
      } else {
        $objc3LinkArguments = @('/nologo', '/MD', '--target=x86_64-pc-windows-msvc', '-fuse-ld=lld',
          ('/Fe' + $objc3Executable)) + $objc3Objects
      }
      & $objc3Compiler @objc3LinkArguments
      $objc3LinkExit = $LASTEXITCODE
      $objc3Commands += @{argv=@($objc3Compiler) + $objc3LinkArguments; exit_code=$objc3LinkExit}
      if ($objc3LinkExit -ne 0) { throw "Carrier ABI link failed: $ClientDriver $objc3Name ($objc3LinkExit)" }
      & $objc3Executable
      $objc3RunExit = $LASTEXITCODE
      $objc3Record = @{name=$objc3Name; sources=$objc3Sources; commands=$objc3Commands;
        header_sha256=(Get-FileHash -LiteralPath (Join-Path $objc3Inputs $objc3Case.header) -Algorithm SHA256).Hash;
        objects=@($objc3Objects | ForEach-Object { @{file=$_; sha256=(Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash} });
        executable=$objc3Executable; executable_sha256=(Get-FileHash -LiteralPath $objc3Executable -Algorithm SHA256).Hash;
        exit_code=$objc3RunExit}
      $objc3Records += $objc3Record
      if ($objc3RunExit -ne 0) { throw "Carrier ABI runtime failed: $ClientDriver $objc3Name ($objc3RunExit)" }
      Write-Output "CARRIER ABI PASS ($ClientDriver): $objc3Name"
    }
  }
  @{status='passed'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
    compiler_sha256=$objc3CompilerHash; compiler_version=$objc3Version; linker_sha256=$objc3LinkerHash;
    started_utc=$objc3Started; finished_utc=[DateTime]::UtcNow.ToString('o'); fixtures=$objc3Records} |
    ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $objc3Results -Encoding utf8
} catch {
  if ($objc3Results) {
    @{status='failed'; client_driver=$ClientDriver; build_identity=$objc3Seal.build_id;
      compiler_sha256=$objc3CompilerHash; linker_sha256=$objc3LinkerHash; started_utc=$objc3Started;
      finished_utc=[DateTime]::UtcNow.ToString('o'); fixtures=$objc3Records; error=$_.Exception.Message} |
      ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $objc3Results -Encoding utf8
  }
  throw
} finally {
  Pop-Location
}
