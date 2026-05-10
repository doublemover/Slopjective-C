$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$defaultOutDir = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native"
$compileArgumentsScript = Join-Path $repoRoot "scripts/objc3c_native_compile_arguments.ps1"
$compileIoModule = Join-Path $repoRoot "scripts/objc3c_native_compile_io.psm1"
$compileToolchainModule = Join-Path $repoRoot "scripts/objc3c_native_compile_toolchain.psm1"
$compileFrontendGuardsModule = Join-Path $repoRoot "scripts/objc3c_native_compile_frontend_guards.psm1"
$compileCommandModule = Join-Path $repoRoot "scripts/objc3c_native_compile_command.psm1"
$runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
if (!(Test-Path -LiteralPath $compileArgumentsScript -PathType Leaf)) {
  Write-Error "native compile argument helper missing at $compileArgumentsScript"
  exit 2
}
if (!(Test-Path -LiteralPath $compileIoModule -PathType Leaf)) {
  Write-Error "native compile IO helper missing at $compileIoModule"
  exit 2
}
if (!(Test-Path -LiteralPath $compileToolchainModule -PathType Leaf)) {
  Write-Error "native compile toolchain helper missing at $compileToolchainModule"
  exit 2
}
if (!(Test-Path -LiteralPath $compileFrontendGuardsModule -PathType Leaf)) {
  Write-Error "native compile frontend guard helper missing at $compileFrontendGuardsModule"
  exit 2
}
if (!(Test-Path -LiteralPath $compileCommandModule -PathType Leaf)) {
  Write-Error "native compile command helper missing at $compileCommandModule"
  exit 2
}
if (!(Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
  Write-Error "runtime launch contract helper missing at $runtimeLaunchContractScript"
  exit 2
}
. $compileArgumentsScript
Import-Module $compileIoModule -Force -DisableNameChecking
Import-Module $compileToolchainModule -Force -DisableNameChecking
Import-Module $compileFrontendGuardsModule -Force -DisableNameChecking
Import-Module $compileCommandModule -Force -DisableNameChecking
. $runtimeLaunchContractScript

$parsed = Parse-Objc3cNativeCompileArguments -RawArgs $args -DefaultOutDir $defaultOutDir
$exe = Resolve-NativeCompilerExecutablePath -RepoRoot $repoRoot
$buildResult = $null
$buildResult = Ensure-NativeCompilerAvailable -RepoRoot $repoRoot -BuildResult $buildResult

$frontendGuardResult = Invoke-Objc3cNativeCompileFrontendGuards `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -ParsedArgs $parsed
$effectiveCompileArgs = @($frontendGuardResult.effective_compile_args)
$compileCommand = New-Objc3cNativeCompileCommand `
  -RepoRoot $repoRoot `
  -ParsedArgs $parsed `
  -EffectiveCompileArgs $effectiveCompileArgs `
  -WrapperScriptPath $PSCommandPath
$inputPath = $compileCommand.input_path
$cacheContext = $compileCommand.cache_context

if ($parsed.use_cache -and $null -ne $cacheContext.cache_key) {
  $cacheRestore = Restore-Objc3cNativeCompileCacheEntry `
    -CacheContext $cacheContext `
    -DestinationRoot $parsed.out_dir
  if ($cacheRestore.restored) {
    Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix
    Write-CompileOutputProvenance `
      -RepoRoot $repoRoot `
      -CompileDir $parsed.out_dir `
      -EmitPrefix $parsed.emit_prefix `
      -InputPath $inputPath `
      -CompilerBinaryPath $exe `
      -RuntimeLibraryPath (Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib") `
      -WrapperScriptPath $PSCommandPath
    Write-Output "cache_hit=true"
    exit ([int]$cacheRestore.exit_code)
  }
}

$compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments @($compileCommand.arguments)

if ($compileExit -eq 0) {
  Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix
  Write-CompileOutputProvenance `
    -RepoRoot $repoRoot `
    -CompileDir $parsed.out_dir `
    -EmitPrefix $parsed.emit_prefix `
    -InputPath $inputPath `
    -CompilerBinaryPath $exe `
    -RuntimeLibraryPath (Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib") `
    -WrapperScriptPath $PSCommandPath
}

if ($parsed.use_cache -and $null -ne $cacheContext.cache_key) {
  Save-Objc3cNativeCompileCacheEntry `
    -CacheContext $cacheContext `
    -SourceRoot $parsed.out_dir `
    -CompileExit $compileExit
}

Write-Output "cache_hit=false"
exit $compileExit
