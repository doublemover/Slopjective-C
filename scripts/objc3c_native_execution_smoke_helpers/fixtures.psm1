Set-StrictMode -Version Latest

function Get-Fixtures {
  param(
    [Parameter(Mandatory = $true)][string]$Directory,
    [Parameter(Mandatory = $true)][string]$FixtureKind
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "execution smoke FAIL: missing $FixtureKind fixture directory $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File -Filter "*.objc3" |
      Sort-Object -Property FullName
  )
  if ($fixtures.Count -eq 0) {
    throw "execution smoke FAIL: no $FixtureKind fixtures found in $Directory"
  }
  return $fixtures
}

function Get-RequestedRelativePaths {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureListPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $resolvedFixtureList = if ([System.IO.Path]::IsPathRooted($FixtureListPath)) {
    $FixtureListPath
  } else {
    Join-Path $RepoRoot $FixtureListPath
  }
  if (!(Test-Path -LiteralPath $resolvedFixtureList -PathType Leaf)) {
    throw "execution smoke FAIL: missing fixture list at $resolvedFixtureList"
  }

  $requested = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
  foreach ($rawLine in @(Get-Content -LiteralPath $resolvedFixtureList)) {
    $candidate = "$rawLine".Trim()
    if ([string]::IsNullOrWhiteSpace($candidate) -or $candidate.StartsWith("#")) {
      continue
    }
    $normalized = $candidate.Replace('\', '/')
    if ($normalized.StartsWith("./")) {
      $normalized = $normalized.Substring(2)
    }
    $null = $requested.Add($normalized)
  }
  return $requested
}

function Select-ExecutionFixtureEntries {
  param(
    [Parameter(Mandatory = $true)][object[]]$Entries,
    [string]$FixtureListPath,
    [string]$FixtureGlobPattern,
    [Parameter(Mandatory = $true)][int]$ShardIndexValue,
    [Parameter(Mandatory = $true)][int]$ShardCountValue,
    [Parameter(Mandatory = $true)][int]$LimitValue,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  if ($LimitValue -lt 0) {
    throw "execution smoke FAIL: limit must be non-negative"
  }
  if ($ShardCountValue -lt 0) {
    throw "execution smoke FAIL: shard-count must be non-negative"
  }
  if (($ShardIndexValue -ge 0) -and ($ShardCountValue -le 0)) {
    throw "execution smoke FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCountValue -gt 0) -and (($ShardIndexValue -lt 0) -or ($ShardIndexValue -ge $ShardCountValue))) {
    throw "execution smoke FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
  }

  $selected = @($Entries)

  if (-not [string]::IsNullOrWhiteSpace($FixtureListPath)) {
    $requested = Get-RequestedRelativePaths -FixtureListPath $FixtureListPath -RepoRoot $RepoRoot
    $selected = @($selected | Where-Object { $requested.Contains($_.relative_path) })
    $matched = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
    foreach ($entry in $selected) {
      $null = $matched.Add([string]$entry.relative_path)
    }
    $missing = @()
    foreach ($requestedPath in $requested) {
      if (-not $matched.Contains($requestedPath)) {
        $missing += $requestedPath
      }
    }
    if ($missing.Count -gt 0) {
      throw "execution smoke FAIL: fixture-list entries did not match execution fixtures ($($missing -join ', '))"
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($FixtureGlobPattern)) {
    $pattern = [System.Management.Automation.WildcardPattern]::new(
      $FixtureGlobPattern.Replace('\', '/'),
      [System.Management.Automation.WildcardOptions]::IgnoreCase
    )
    $selected = @($selected | Where-Object { $pattern.IsMatch($_.relative_path) })
  }

  if ($ShardCountValue -gt 0) {
    $sharded = New-Object System.Collections.Generic.List[object]
    for ($index = 0; $index -lt $selected.Count; $index++) {
      if (($index % $ShardCountValue) -eq $ShardIndexValue) {
        $sharded.Add($selected[$index]) | Out-Null
      }
    }
    $selected = @($sharded)
  }

  if (($LimitValue -gt 0) -and ($selected.Count -gt $LimitValue)) {
    $selected = @($selected | Select-Object -First $LimitValue)
  }

  if ($selected.Count -eq 0) {
    throw "execution smoke FAIL: no execution fixtures matched the requested selection"
  }

  return $selected
}

Export-ModuleMember -Function @(
  "Get-Fixtures",
  "Select-ExecutionFixtureEntries"
)
