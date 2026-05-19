$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Write-Objc3cNativeCompileSuccessArtifacts {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$ParsedArgs,
    [Parameter(Mandatory = $true)][string]$InputPath,
    [Parameter(Mandatory = $true)][string]$CompilerBinaryPath,
    [Parameter(Mandatory = $true)][string]$WrapperScriptPath
  )

  Assert-Objc3cRuntimeLaunchContract `
    -CompileDir $ParsedArgs.out_dir `
    -RepoRoot $RepoRoot `
    -EmitPrefix $ParsedArgs.emit_prefix
  Write-CompileOutputProvenance `
    -RepoRoot $RepoRoot `
    -CompileDir $ParsedArgs.out_dir `
    -EmitPrefix $ParsedArgs.emit_prefix `
    -InputPath $InputPath `
    -CompilerBinaryPath $CompilerBinaryPath `
    -RuntimeLibraryPath (Join-Path $RepoRoot "artifacts/lib/objc3_runtime.lib") `
    -WrapperScriptPath $WrapperScriptPath
}
