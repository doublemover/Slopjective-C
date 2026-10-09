$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Test-AnyPathNewerThanTarget {
  param([Parameter(Mandatory=$true)][string]$TargetPath,[Parameter(Mandatory=$true)][string[]]$InputPaths)
  if (!(Test-Path -LiteralPath $TargetPath -PathType Leaf)) { return $true }
  foreach ($inputPath in $InputPaths) {
    if (!(Test-Path -LiteralPath $inputPath -PathType Leaf)) { return $true }
    if ((Get-Item -LiteralPath $inputPath).LastWriteTimeUtc -gt (Get-Item -LiteralPath $TargetPath).LastWriteTimeUtc) { return $true }
  }
  return $false
}

function Test-CMakeGraphHasPendingBuild {
  param([Parameter(Mandatory=$true)][string]$RepoRoot)
  $buildDir = Join-Path $RepoRoot "tmp/build-objc3c-native"
  $ninja = Join-Path $RepoRoot "toolchains/llvm/bin/ninja.exe"
  if (!(Test-Path -LiteralPath (Join-Path $buildDir "build.ninja") -PathType Leaf)) { return $true }
  if (!(Test-Path -LiteralPath $ninja -PathType Leaf)) {
    $found = Get-Command ninja -ErrorAction SilentlyContinue
    if ($null -eq $found) { return $true }
    $ninja = $found.Source
  }
  foreach ($target in @("objc3c-native", "objc3_runtime")) {
    $probe = & $ninja -C $buildDir -n $target 2>&1
    if ($LASTEXITCODE -ne 0) { return $true }
    if (@($probe | Where-Object { $_ -match '^(?!ninja: no work to do\.).+\.(o|obj|a|lib|dll|so|dylib|exe)?$' }).Count -gt 0) { return $true }
    if (($probe -join "`n") -notmatch "no work to do") { return $true }
  }
  return $false
}
