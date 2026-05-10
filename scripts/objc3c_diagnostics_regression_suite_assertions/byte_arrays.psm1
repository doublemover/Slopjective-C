$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-Objc3cDiagnosticsByteArraySha256HexCore {
  param([byte[]]$Bytes)

  if ($null -eq $Bytes) {
    return ""
  }

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Test-Objc3cDiagnosticsByteArrayEqualCore {
  param(
    [byte[]]$Left,
    [byte[]]$Right
  )

  if ($null -eq $Left -or $null -eq $Right) {
    return $false
  }
  if ($Left.Length -ne $Right.Length) {
    return $false
  }
  for ($i = 0; $i -lt $Left.Length; $i++) {
    if ($Left[$i] -ne $Right[$i]) {
      return $false
    }
  }
  return $true
}

Export-ModuleMember -Function @(
  "Get-Objc3cDiagnosticsByteArraySha256HexCore",
  "Test-Objc3cDiagnosticsByteArrayEqualCore"
)
