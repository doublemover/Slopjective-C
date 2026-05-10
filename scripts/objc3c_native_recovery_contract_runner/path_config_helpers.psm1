$ErrorActionPreference = "Stop"

Import-Module (Join-Path $PSScriptRoot "context.psm1") -Force -DisableNameChecking

function Resolve-RecoveryContractSourcePath {
  param(
    [string]$Source,
    [string]$CaseName
  )

  $context = Get-RecoveryContractContext
  $resolvedSource = if ([System.IO.Path]::IsPathRooted($Source)) { $Source } else { Join-Path $context.RepoRoot $Source }
  if (-not (Test-Path -LiteralPath $resolvedSource -PathType Leaf)) {
    throw "contract FAIL: missing source fixture for $CaseName at $resolvedSource"
  }

  return $resolvedSource
}

function New-RecoveryContractReplayRunPaths {
  param(
    [string]$OutDir,
    [string]$CaseName
  )

  $run1 = Join-Path $OutDir ($CaseName + "_run1")
  $run2 = Join-Path $OutDir ($CaseName + "_run2")
  New-Item -ItemType Directory -Force -Path $run1 | Out-Null
  New-Item -ItemType Directory -Force -Path $run2 | Out-Null

  return [pscustomobject]@{
    Run1 = $run1
    Run2 = $run2
  }
}

function New-RecoveryContractCompileArguments {
  param(
    [string]$Source,
    [string]$RunDir,
    [string[]]$ExtraArgs = @()
  )

  return @($Source, "--out-dir", $RunDir, "--emit-prefix", "module") + $ExtraArgs
}

function Get-RecoveryContractArtifactPath {
  param(
    [string]$RunDir,
    [string]$FileName
  )

  return Join-Path $RunDir $FileName
}

function New-InvalidDispatchContractConfig {
  param(
    [string]$OutDir,
    [string]$HelloObjc3Source
  )

  $invalidDispatchOutDir = Join-Path $OutDir "objc3_invalid_dispatch_symbol"
  New-Item -ItemType Directory -Force -Path $invalidDispatchOutDir | Out-Null
  $resolvedInvalidDispatchSource = [System.IO.Path]::GetFullPath($HelloObjc3Source)
  if (-not (Test-Path -LiteralPath $resolvedInvalidDispatchSource -PathType Leaf)) {
    throw "contract FAIL: missing invalid-dispatch source fixture at $resolvedInvalidDispatchSource"
  }

  return [pscustomobject]@{
    OutDir = $invalidDispatchOutDir
    Source = $resolvedInvalidDispatchSource
  }
}

Export-ModuleMember -Function @(
  "Get-RecoveryContractArtifactPath",
  "New-InvalidDispatchContractConfig",
  "New-RecoveryContractCompileArguments",
  "New-RecoveryContractReplayRunPaths",
  "Resolve-RecoveryContractSourcePath"
)
