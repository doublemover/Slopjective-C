$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Show-Objc3cNativeCompileUsageAndExit {
  Write-Error "usage: objc3c_native_compile.ps1 <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--use-cache]"
  exit 2
}

function Stop-Objc3cNativeCompileArgumentError {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Message
  )

  Write-Error $Message
  exit 2
}
