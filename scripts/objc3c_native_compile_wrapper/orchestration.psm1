$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-Objc3cNativeCompileWrapper {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$DefaultOutDir,
    [string[]]$RawArgs,
    [Parameter(Mandatory = $true)][string]$WrapperScriptPath
  )

  $parsed = Parse-Objc3cNativeCompileArguments -RawArgs $RawArgs -DefaultOutDir $DefaultOutDir
  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $RepoRoot
  $buildResult = Ensure-NativeCompilerAvailable -RepoRoot $RepoRoot -BuildResult $null

  $frontendGuardResult = Invoke-Objc3cNativeCompileFrontendGuards `
    -RepoRoot $RepoRoot `
    -BuildResult $buildResult `
    -ParsedArgs $parsed
  $compileCommand = New-Objc3cNativeCompileCommand `
    -RepoRoot $RepoRoot `
    -ParsedArgs $parsed `
    -EffectiveCompileArgs @($frontendGuardResult.effective_compile_args) `
    -WrapperScriptPath $WrapperScriptPath

  Invoke-Objc3cNativeCompileCacheRestore `
    -RepoRoot $RepoRoot `
    -ParsedArgs $parsed `
    -CacheContext $compileCommand.cache_context `
    -InputPath $compileCommand.input_path `
    -CompilerBinaryPath $exe `
    -WrapperScriptPath $WrapperScriptPath | Out-Null

  $compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments @($compileCommand.arguments)
  if ($compileExit -eq 0) {
    Write-Objc3cNativeCompileSuccessArtifacts `
      -RepoRoot $RepoRoot `
      -ParsedArgs $parsed `
      -InputPath $compileCommand.input_path `
      -CompilerBinaryPath $exe `
      -WrapperScriptPath $WrapperScriptPath
  }

  Save-Objc3cNativeCompileCacheIfEnabled `
    -ParsedArgs $parsed `
    -CacheContext $compileCommand.cache_context `
    -CompileExit $compileExit

  Write-Output "cache_hit=false"
  exit $compileExit
}
