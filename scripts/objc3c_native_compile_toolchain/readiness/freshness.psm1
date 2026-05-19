$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Test-AnyPathNewerThanTarget {
  param(
    [Parameter(Mandatory = $true)][string]$TargetPath,
    [Parameter(Mandatory = $true)][string[]]$InputPaths
  )

  if (!(Test-Path -LiteralPath $TargetPath -PathType Leaf)) {
    return $true
  }

  $targetTimestamp = (Get-Item -LiteralPath $TargetPath).LastWriteTimeUtc
  foreach ($inputPath in $InputPaths) {
    if (!(Test-Path -LiteralPath $inputPath -PathType Leaf)) {
      continue
    }

    $inputTimestamp = (Get-Item -LiteralPath $inputPath).LastWriteTimeUtc
    if ($inputTimestamp -gt $targetTimestamp) {
      return $true
    }
  }

  return $false
}
