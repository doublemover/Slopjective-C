$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-Sha256HexFromBytes {
  param([byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Get-FileSha256Hex {
  param([string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Get-OptionalFileHash {
  param($Path)

  $pathText = if ($null -eq $Path) { "" } else { [string]$Path }

  if ([string]::IsNullOrWhiteSpace($pathText)) {
    return ""
  }
  if (!(Test-Path -LiteralPath $pathText -PathType Leaf)) {
    return ""
  }
  return Get-FileSha256Hex -Path $pathText
}

Export-ModuleMember -Function @(
  "Get-Sha256HexFromBytes",
  "Get-FileSha256Hex",
  "Get-OptionalFileHash"
)
