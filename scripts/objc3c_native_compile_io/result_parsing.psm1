$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Write-Objc3cNativeCompileCacheRecoverySignal {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Reason
  )

  Write-Output ("cache_recovery=" + $Reason)
}

function Read-Objc3cNativeCompileCacheMetadata {
  param([Parameter(Mandatory = $true)][string]$Path)

  try {
    $metadata = Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
    return [pscustomobject]@{
      parsed = $true
      metadata = $metadata
    }
  } catch {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_invalid"
    return [pscustomobject]@{
      parsed = $false
      metadata = $null
    }
  }
}

function ConvertTo-Objc3cNativeCompileExitCode {
  param([string]$Value)

  $parsedExitCode = 0
  if (-not [int]::TryParse($Value, [ref]$parsedExitCode)) {
    return [pscustomobject]@{
      parsed = $false
      exit_code = 0
    }
  }

  return [pscustomobject]@{
    parsed = $true
    exit_code = $parsedExitCode
  }
}

function Read-Objc3cNativeCompileExitCode {
  param([Parameter(Mandatory = $true)][string]$Path)

  $rawExitCode = (Get-Content -LiteralPath $Path -Raw).Trim()
  return ConvertTo-Objc3cNativeCompileExitCode -Value $rawExitCode
}

Export-ModuleMember -Function @(
  "ConvertTo-Objc3cNativeCompileExitCode",
  "Read-Objc3cNativeCompileCacheMetadata",
  "Read-Objc3cNativeCompileExitCode",
  "Write-Objc3cNativeCompileCacheRecoverySignal"
)
