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
  $compilerItem = Get-Item -LiteralPath $command.file_path -Force
  if (($compilerItem.Attributes -band [System.IO.FileAttributes]::ReparsePoint) -ne 0) {
    throw "native compiler executable cannot be a reparse point: $($command.file_path)"
  }
  $isWindowsPlatform = [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform(
    [System.Runtime.InteropServices.OSPlatform]::Windows
  )
  if ($isWindowsPlatform -and [System.IO.Path]::GetExtension($compilerItem.FullName) -ne ".exe") {
    throw "native compiler executable must be a Windows .exe path: $($command.file_path)"
  }

  $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
  $startInfo.FileName = $compilerItem.FullName
  $startInfo.UseShellExecute = $false
  $startInfo.CreateNoWindow = $true
  foreach ($argument in @($command.arguments)) {
    [void]$startInfo.ArgumentList.Add([string]$argument)
  }

  $process = [System.Diagnostics.Process]::Start($startInfo)
  $process.WaitForExit()
  return [int]$process.ExitCode
}

Export-ModuleMember -Function @(
  "Invoke-BuildNativeCompiler",
  "Invoke-NativeCompiler"
)
