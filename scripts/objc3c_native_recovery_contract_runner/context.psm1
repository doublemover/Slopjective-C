$ErrorActionPreference = "Stop"

$script:RecoveryContractContext = $null

function Set-RecoveryContractContext {
  param(
    [string]$RepoRoot,
    [string]$OutDir,
    [string]$CompilerPath,
    [string]$CompileWrapperScript,
    [string]$PowerShellExecutable
  )

  $script:RecoveryContractContext = [pscustomobject]@{
    RepoRoot = $RepoRoot
    OutDir = $OutDir
    CompilerPath = $CompilerPath
    CompileWrapperScript = $CompileWrapperScript
    PowerShellExecutable = $PowerShellExecutable
  }
}

function Get-RecoveryContractContext {
  if ($null -eq $script:RecoveryContractContext) {
    throw "contract FAIL: recovery contract runner context was not initialized"
  }

  return $script:RecoveryContractContext
}

function Invoke-Objc3cNativeWithRecovery {
  param(
    [string[]]$Arguments,
    [switch]$UseCompileWrapper
  )

  $context = Get-RecoveryContractContext
  $exe = $context.CompilerPath
  $compileWrapperScript = $context.CompileWrapperScript
  $pwsh = $context.PowerShellExecutable

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "contract FAIL: native compiler executable missing at $exe"
  }

  if ($UseCompileWrapper) {
    if (!(Test-Path -LiteralPath $compileWrapperScript -PathType Leaf)) {
      throw "contract FAIL: compile wrapper missing at $compileWrapperScript"
    }
    $null = & $pwsh -NoProfile -ExecutionPolicy Bypass -File $compileWrapperScript @Arguments
    return [int]$LASTEXITCODE
  }

  $null = & $exe @Arguments
  return [int]$LASTEXITCODE
}

Export-ModuleMember -Function @(
  "Get-RecoveryContractContext",
  "Invoke-Objc3cNativeWithRecovery",
  "Set-RecoveryContractContext"
)
