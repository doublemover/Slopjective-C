Set-StrictMode -Version Latest

function Get-Sha256HexFromText {
  param([Parameter(Mandatory = $true)][AllowEmptyString()][string]$Text)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Text)
    $hashBytes = $sha256.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $sha256.Dispose()
  }
}

function Get-Sha256HexFromBytes {
  param([Parameter(Mandatory = $true)][byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $sha256.Dispose()
  }
}

function Get-Sha256HexFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "execution replay proof FAIL: missing file for hashing $Path"
  }
  return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-NormalizedTextFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "execution replay proof FAIL: missing text file $Path"
  }
  return ("$((Get-Content -LiteralPath $Path -Raw))").Replace("`r`n", "`n")
}

function Read-JsonHashtable {
  param([Parameter(Mandatory = $true)][string]$Path)

  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json -AsHashtable)
}
