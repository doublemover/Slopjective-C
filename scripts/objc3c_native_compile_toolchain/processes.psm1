$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$commandsModule = Join-Path $PSScriptRoot "commands.psm1"
$resultsModule = Join-Path $PSScriptRoot "results.psm1"
foreach ($dependencyModule in @($commandsModule, $resultsModule)) {
  if (!(Test-Path -LiteralPath $dependencyModule -PathType Leaf)) {
    Write-Error "native compile toolchain dependency missing at $dependencyModule"
    exit 2
  }
  Import-Module $dependencyModule -Force -DisableNameChecking
}

function Invoke-BuildNativeCompiler {
  param([string]$RepoRoot)

  $command = New-NativeCompilerBuildCommand -RepoRoot $RepoRoot
  $buildArguments = @($command.arguments)
  if ($buildArguments.Count -gt 0) {
    $buildOutput = @(& $command.file_path @buildArguments)
  } else {
    $buildOutput = @(& $command.file_path)
  }

  return ConvertTo-NativeCompilerBuildResult `
    -BuildOutput $buildOutput `
    -ExitCode ([int]$LASTEXITCODE)
}

function Invoke-NativeCompiler {
  param(
    [string]$ExePath,
    [string[]]$Arguments
  )

  $command = New-NativeCompilerProcessCommand -ExePath $ExePath -Arguments $Arguments
  $process = Start-Process -FilePath $command.file_path -ArgumentList $command.arguments -NoNewWindow -Wait -PassThru
  return [int]$process.ExitCode
}

Export-ModuleMember -Function @(
  "Invoke-BuildNativeCompiler",
  "Invoke-NativeCompiler"
)
