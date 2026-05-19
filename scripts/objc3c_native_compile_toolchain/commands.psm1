$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function New-NativeCompilerBuildCommand {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return [pscustomobject]@{
    file_path = (Join-Path $RepoRoot "scripts/build_objc3c_native.ps1")
    arguments = @()
  }
}

function New-NativeCompilerProcessCommand {
  param(
    [Parameter(Mandatory = $true)][string]$ExePath,
    [string[]]$Arguments
  )

  return [pscustomobject]@{
    file_path = $ExePath
    arguments = @($Arguments)
  }
}

Export-ModuleMember -Function @(
  "New-NativeCompilerBuildCommand",
  "New-NativeCompilerProcessCommand"
)
