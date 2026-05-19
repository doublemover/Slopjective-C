Set-StrictMode -Version Latest

function Get-Objc3cParserReplayRepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  if ($fullPath.StartsWith($RepoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($RepoRoot.Length).TrimStart([char[]]@([char]92, [char]47)).Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Assert-Objc3cParserReplayPreconditions {
  param(
    [Parameter(Mandatory = $true)][string]$BuildScript,
    [Parameter(Mandatory = $true)][string]$FixtureDir
  )

  if (!(Test-Path -LiteralPath $BuildScript -PathType Leaf)) {
    Write-Output "error: parser replay proof FAIL: missing build script at $BuildScript"
    exit 1
  }
  if (!(Test-Path -LiteralPath $FixtureDir -PathType Container)) {
    Write-Output "error: parser replay proof FAIL: missing fixture directory at $FixtureDir"
    exit 1
  }
}

function Ensure-Objc3cParserReplayNativeCompiler {
  param(
    [Parameter(Mandatory = $true)][string]$BuildScript,
    [Parameter(Mandatory = $true)][string]$NativeExe,
    [Parameter(Mandatory = $true)][string]$BuildLogPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  if (!(Test-Path -LiteralPath $NativeExe -PathType Leaf)) {
    & $BuildScript *> $BuildLogPath
    $buildExitCode = [int]$LASTEXITCODE
    if ($buildExitCode -ne 0) {
      Write-Output ("error: parser replay proof FAIL: native build failed with exit {0}" -f $buildExitCode)
      Write-Output ("build_log: {0}" -f (Get-Objc3cParserReplayRepoRelativePath -Path $BuildLogPath -RepoRoot $RepoRoot))
      exit 1
    }
  }

  if (!(Test-Path -LiteralPath $NativeExe -PathType Leaf)) {
    Write-Output "error: parser replay proof FAIL: missing native compiler executable after build"
    exit 1
  }
}

function Invoke-Objc3cParserReplayCompile {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExe,
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$OutDir,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $false
  }

  New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
  & $NativeExe $FixturePath --out-dir $OutDir --emit-prefix module *> $LogPath
  return [int]$LASTEXITCODE
}

Export-ModuleMember -Function @(
  "Get-Objc3cParserReplayRepoRelativePath",
  "Assert-Objc3cParserReplayPreconditions",
  "Ensure-Objc3cParserReplayNativeCompiler",
  "Invoke-Objc3cParserReplayCompile"
)
