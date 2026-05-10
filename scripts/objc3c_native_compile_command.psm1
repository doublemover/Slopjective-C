$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$compileArgumentsScript = Join-Path $PSScriptRoot "objc3c_native_compile_arguments.ps1"
$compileIoModule = Join-Path $PSScriptRoot "objc3c_native_compile_io.psm1"
if (!(Test-Path -LiteralPath $compileArgumentsScript -PathType Leaf)) {
  Write-Error "native compile argument helper missing at $compileArgumentsScript"
  exit 2
}
if (!(Test-Path -LiteralPath $compileIoModule -PathType Leaf)) {
  Write-Error "native compile IO helper missing at $compileIoModule"
  exit 2
}
. $compileArgumentsScript
Import-Module $compileIoModule -Force -DisableNameChecking

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
  }
}

Export-ModuleMember -Function "New-Objc3cNativeCompileCommand"
