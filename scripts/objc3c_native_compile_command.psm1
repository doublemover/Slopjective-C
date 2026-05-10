$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-Objc3cNativeCompileCommand {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)]$ParsedArgs,
    [Parameter(Mandatory = $true)][string[]]$EffectiveCompileArgs,
    [Parameter(Mandatory = $true)][string]$WrapperScriptPath
  )

  $compileArgs = @($EffectiveCompileArgs)
  $argsWithoutOutDir = @(Get-Objc3cNativeCompileArgsWithoutOutDir -CompileArgs $compileArgs)
  $inputPath = Get-Objc3cNativeCompileInputPath -ArgsWithoutOutDir $argsWithoutOutDir
  $cacheContext = New-Objc3cNativeCompileCacheContext `
    -RepoRoot $RepoRoot `
    -InputPath $inputPath `
    -ArgsWithoutOutDir $argsWithoutOutDir `
    -WrapperScriptPath $WrapperScriptPath

  return [pscustomobject]@{
    arguments = $compileArgs
    args_without_out_dir = $argsWithoutOutDir
    input_path = $inputPath
    cache_context = $cacheContext
    out_dir = [string]$ParsedArgs.out_dir
    emit_prefix = [string]$ParsedArgs.emit_prefix
    use_cache = [bool]$ParsedArgs.use_cache
  }
}

Export-ModuleMember -Function "New-Objc3cNativeCompileCommand"
