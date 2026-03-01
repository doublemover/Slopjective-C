$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$llvmRoot = if ($env:LLVM_ROOT) { $env:LLVM_ROOT } else { "C:\Program Files\LLVM" }
$clangxx = Join-Path $llvmRoot "bin\clang++.exe"

if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
  $clangCommand = Get-Command clang++ -ErrorAction SilentlyContinue
  if ($null -ne $clangCommand -and (Test-Path -LiteralPath $clangCommand.Source -PathType Leaf)) {
    $clangxx = $clangCommand.Source
    $clangBinDir = Split-Path -Parent $clangxx
    $llvmRoot = Split-Path -Parent $clangBinDir
  }
}

$libclangCandidates = @(
  (Join-Path $llvmRoot "lib\libclang.lib"),
  (Join-Path $llvmRoot "lib\clang.lib")
)
$libclang = $null
foreach ($candidate in $libclangCandidates) {
  if (Test-Path -LiteralPath $candidate -PathType Leaf) {
    $libclang = $candidate
    break
  }
}

$includeDir = Join-Path $llvmRoot "include"
$nativeSourceRoot = Join-Path $repoRoot "native/objc3c/src"

if (!(Test-Path -LiteralPath $clangxx -PathType Leaf)) {
  throw ("clang++ not found. set LLVM_ROOT or ensure clang++ is on PATH (attempted: " + $clangxx + ")")
}
if ($null -eq $libclang) {
  $attempted = [string]::Join(", ", $libclangCandidates)
  throw ("LLVM import library not found. set LLVM_ROOT to a full LLVM install (attempted: " + $attempted + ")")
}
if (!(Test-Path -LiteralPath $includeDir -PathType Container)) { throw "LLVM include dir not found at $includeDir" }
if (!(Test-Path -LiteralPath $nativeSourceRoot -PathType Container)) { throw "native source root not found at $nativeSourceRoot" }

$outDir = Join-Path $repoRoot "artifacts/bin"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$outExe = Join-Path $outDir "objc3c-native.exe"
$outCapiExe = Join-Path $outDir "objc3c-frontend-c-api-runner.exe"
$tmpOutDir = Join-Path $repoRoot "tmp/build-objc3c-native"
New-Item -ItemType Directory -Force -Path $tmpOutDir | Out-Null
$runSuffix = "{0}_{1}" -f (Get-Date -Format "yyyyMMdd_HHmmss_fff"), $PID
$stagedOutExe = Join-Path $tmpOutDir ("objc3c-native.{0}.exe" -f $runSuffix)
$stagedOutCapiExe = Join-Path $tmpOutDir ("objc3c-frontend-c-api-runner.{0}.exe" -f $runSuffix)

function Publish-ArtifactWithRetry {
  param(
    [Parameter(Mandatory = $true)]
    [string]$StagedPath,
    [Parameter(Mandatory = $true)]
    [string]$FinalPath,
    [int]$MaxAttempts = 40,
    [int]$SleepMilliseconds = 250
  )

  for ($attempt = 1; $attempt -le $MaxAttempts; $attempt++) {
    try {
      Move-Item -LiteralPath $StagedPath -Destination $FinalPath -Force
      return
    } catch {
      if ($attempt -eq $MaxAttempts) {
        throw "failed to publish $FinalPath after $MaxAttempts attempt(s): $($_.Exception.Message)"
      }
      Start-Sleep -Milliseconds $SleepMilliseconds
    }
  }
}

$sharedSources = @(
  "native/objc3c/src/driver/objc3_cli_options.cpp"
  "native/objc3c/src/driver/objc3_driver_main.cpp"
  "native/objc3c/src/driver/objc3_driver_shell.cpp"
  "native/objc3c/src/driver/objc3_frontend_options.cpp"
  "native/objc3c/src/driver/objc3_llvm_capability_routing.cpp"
  "native/objc3c/src/driver/objc3_objc3_path.cpp"
  "native/objc3c/src/driver/objc3_objectivec_path.cpp"
  "native/objc3c/src/driver/objc3_compilation_driver.cpp"
  "native/objc3c/src/diag/objc3_diag_utils.cpp"
  "native/objc3c/src/io/objc3_diagnostics_artifacts.cpp"
  "native/objc3c/src/io/objc3_file_io.cpp"
  "native/objc3c/src/io/objc3_manifest_artifacts.cpp"
  "native/objc3c/src/io/objc3_process.cpp"
  "native/objc3c/src/ir/objc3_ir_emitter.cpp"
  "native/objc3c/src/lex/objc3_lexer.cpp"
  "native/objc3c/src/libobjc3c_frontend/c_api.cpp"
  "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp"
  "native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp"
  "native/objc3c/src/lower/objc3_lowering_contract.cpp"
  "native/objc3c/src/parse/objc3_ast_builder.cpp"
  "native/objc3c/src/parse/objc3_ast_builder_contract.cpp"
  "native/objc3c/src/parse/objc3_parser.cpp"
  "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp"
  "native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp"
  "native/objc3c/src/sema/objc3_sema_diagnostics_bus.cpp"
  "native/objc3c/src/sema/objc3_sema_pass_manager.cpp"
  "native/objc3c/src/sema/objc3_semantic_passes.cpp"
  "native/objc3c/src/sema/objc3_static_analysis.cpp"
  "native/objc3c/src/sema/objc3_pure_contract.cpp"
)

$nativeSources = @(
  "native/objc3c/src/main.cpp"
) + $sharedSources
$nativeSourcePaths = @($nativeSources | ForEach-Object { Join-Path $repoRoot $_ })

$capiRunnerSources = @(
  "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp"
) + $sharedSources
$capiRunnerSourcePaths = @($capiRunnerSources | ForEach-Object { Join-Path $repoRoot $_ })

foreach ($sourcePath in @($nativeSourcePaths + $capiRunnerSourcePaths)) {
  if (!(Test-Path -LiteralPath $sourcePath -PathType Leaf)) {
    throw "native source file missing: $sourcePath"
  }
}

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  -DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1 `
  "-I$includeDir" `
  "-I$nativeSourceRoot" `
  @nativeSourcePaths `
  $libclang `
  -o $stagedOutExe

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Publish-ArtifactWithRetry -StagedPath $stagedOutExe -FinalPath $outExe

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  -DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1 `
  "-I$includeDir" `
  "-I$nativeSourceRoot" `
  @capiRunnerSourcePaths `
  $libclang `
  -o $stagedOutCapiExe

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Publish-ArtifactWithRetry -StagedPath $stagedOutCapiExe -FinalPath $outCapiExe
Write-Output ("built=" + [System.IO.Path]::GetRelativePath($repoRoot, $outExe).Replace("\", "/"))
Write-Output ("built=" + [System.IO.Path]::GetRelativePath($repoRoot, $outCapiExe).Replace("\", "/"))
