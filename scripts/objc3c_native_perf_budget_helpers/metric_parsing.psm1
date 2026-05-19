Set-StrictMode -Version Latest

function Parse-CacheHitFlag {
  param(
    [string]$OutputText,
    [string]$RunLabel
  )

  if ([string]::IsNullOrWhiteSpace($OutputText)) {
    throw "perf-budget FAIL: $RunLabel produced no output (missing cache_hit marker)"
  }

  $matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\s*$")
  if ($matches.Count -ne 1) {
    throw "perf-budget FAIL: $RunLabel expected exactly one cache_hit marker, observed $($matches.Count)"
  }
  return ($matches[0].Groups[1].Value.ToLowerInvariant() -eq "true")
}

function Get-ArtifactHashSet {
  param(
    [string]$Directory,
    [string[]]$ArtifactNames
  )

  $hashes = [ordered]@{}
  foreach ($name in $ArtifactNames) {
    $path = Join-Path $Directory $name
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) {
      throw "perf-budget FAIL: cache-proof missing artifact $path"
    }
    $sha256 = [System.Security.Cryptography.SHA256]::Create()
    $stream = [System.IO.File]::OpenRead($path)
    try {
      $hashBytes = $sha256.ComputeHash($stream)
      $hashes[$name] = ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
    } finally {
      $stream.Dispose()
      $sha256.Dispose()
    }
  }
  return $hashes
}

Export-ModuleMember -Function @(
  "Get-ArtifactHashSet",
  "Parse-CacheHitFlag"
)
