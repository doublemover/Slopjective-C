Set-StrictMode -Version Latest

function Get-FixtureCaseName {
  param(
    [string]$Prefix,
    [string]$FixturePath
  )

  $leaf = [System.IO.Path]::GetFileNameWithoutExtension($FixturePath)
  $leaf = $leaf -replace "[^A-Za-z0-9_-]", "_"
  if ($leaf.Length -gt 48) {
    $leaf = $leaf.Substring(0, 48)
  }

  $fullPath = [System.IO.Path]::GetFullPath($FixturePath)
  $bytes = [System.Text.Encoding]::UTF8.GetBytes($fullPath.Replace("\", "/"))
  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($bytes)
  } finally {
    $sha256.Dispose()
  }
  $hash = [System.BitConverter]::ToString($hashBytes).Replace("-", "").ToLowerInvariant().Substring(0, 12)

  return "$Prefix`_$leaf`_$hash"
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

function Read-JsonHashtable {
  param([string]$Path)

  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json -AsHashtable)
}

Export-ModuleMember -Function @(
  "Get-FileSha256Hex",
  "Get-FixtureCaseName",
  "Get-Sha256HexFromBytes",
  "Read-JsonHashtable"
)
