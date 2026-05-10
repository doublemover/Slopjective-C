Set-StrictMode -Version Latest

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-ShortHash {
  param([Parameter(Mandatory = $true)][string]$Value)

  $sha1 = [System.Security.Cryptography.SHA1]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
    $hashBytes = $sha1.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant().Substring(0, 10)
  }
  finally {
    $sha1.Dispose()
  }
}

function Get-CaseDirectoryName {
  param(
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$Kind,
    [Parameter(Mandatory = $true)][string]$FixtureRelativePath,
    [Parameter(Mandatory = $true)][string]$FixtureBaseName
  )

  $prefix = "${Kind}_$(Get-ShortHash -Value $FixtureRelativePath)"
  $sanitizedBase = [regex]::Replace($FixtureBaseName.ToLowerInvariant(), '[^a-z0-9]+', '_').Trim('_')
  if ([string]::IsNullOrWhiteSpace($sanitizedBase)) {
    return $prefix
  }

  $reservedLeaf = "\compile\module.object-backend.txt"
  $maxPathLength = 220
  $available = $maxPathLength - ((Join-Path $RunDir $prefix).Length + $reservedLeaf.Length + 1)
  if ($available -le 0) {
    return $prefix
  }

  if ($sanitizedBase.Length -gt $available) {
    $sanitizedBase = $sanitizedBase.Substring(0, $available).TrimEnd('_')
  }
  if ([string]::IsNullOrWhiteSpace($sanitizedBase)) {
    return $prefix
  }

  return "${prefix}_$sanitizedBase"
}

Export-ModuleMember -Function @(
  "Get-CaseDirectoryName",
  "Get-RepoRelativePath"
)
