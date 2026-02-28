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

$sourceFiles = @(
  "native/objc3c/src/main.cpp"
  "native/objc3c/src/driver/objc3_cli_options.cpp"
  "native/objc3c/src/driver/objc3_driver_shell.cpp"
  "native/objc3c/src/driver/objc3_frontend_options.cpp"
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

& $clangxx `
  -std=c++20 `
  -Wall `
  -Wextra `
  -pedantic `
  -DOBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION=1 `
  "-I$includeDir" `
  "-Inative/objc3c/src" `
  @sourceFiles `
  $libclang `
  -o $outExe

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Output "built=$outExe"
