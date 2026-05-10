Set-StrictMode -Version Latest

function Ensure-NativeCompilerExecutable {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath
  )

  if (Test-Path -LiteralPath $NativeExePath -PathType Leaf) {
    return
  }
  if ($NativeExeExplicit) {
    throw "execution smoke FAIL: configured native compiler missing at $NativeExePath"
  }
  if (!(Test-Path -LiteralPath $BuildScriptPath -PathType Leaf)) {
    throw "execution smoke FAIL: native build script missing at $BuildScriptPath"
  }

  & $BuildScriptPath -ExecutionMode binaries-only | Out-Null
  if ($LASTEXITCODE -ne 0) {
    throw "execution smoke FAIL: native compiler build failed with exit code $LASTEXITCODE"
  }
  if (!(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    throw "execution smoke FAIL: native compiler executable missing at $NativeExePath"
  }
}

Export-ModuleMember -Function @(
  "Ensure-NativeCompilerExecutable"
)
