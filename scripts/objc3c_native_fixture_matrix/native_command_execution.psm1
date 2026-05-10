$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

function Invoke-Objc3cNativeFixtureMatrixLoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return $LASTEXITCODE
  } finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Resolve-Objc3cNativeFixtureMatrixCompiler {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [Parameter(Mandatory = $true)][string]$RunDir
  )

  $exe = Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe"
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    $buildScript = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"
    $buildLog = Join-Path $RunDir "build.log"
    $buildExit = Invoke-Objc3cNativeFixtureMatrixLoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript, "-ExecutionMode", "binaries-only") `
      -LogPath $buildLog
    if ($buildExit -ne 0) {
      throw "matrix FAIL: native compiler build failed with exit code $buildExit"
    }
  }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "matrix FAIL: native compiler executable missing at $exe"
  }

  return $exe
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cNativeFixtureMatrixLoggedCommand",
  "Resolve-Objc3cNativeFixtureMatrixCompiler"
)
