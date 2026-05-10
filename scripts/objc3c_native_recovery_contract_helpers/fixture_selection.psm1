Set-StrictMode -Version Latest

function Get-RecoveryFixtures {
  param(
    [string]$Directory,
    [string]$FixtureKind,
    [string[]]$Extensions = @(".objc3")
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "contract FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(Get-ChildItem -LiteralPath $Directory -Recurse -File | Where-Object {
      $_.Extension -in $Extensions
    } | Sort-Object FullName)

  if ($fixtures.Count -eq 0) {
    throw "contract FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return $fixtures
}

function Get-RequestedRelativePaths {
  param(
    [string]$FixtureListPath,
    [string]$RepoRoot
  )

  $resolvedFixtureList = if ([System.IO.Path]::IsPathRooted($FixtureListPath)) {
    $FixtureListPath
  } else {
    Join-Path $RepoRoot $FixtureListPath
  }
  if (!(Test-Path -LiteralPath $resolvedFixtureList -PathType Leaf)) {
    throw "contract FAIL: missing fixture list at $resolvedFixtureList"
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

function Select-RecoveryFixtureEntries {
  param(
    [object[]]$Entries,
    [string]$FixtureListPath,
    [string]$FixtureGlobPattern,
    [int]$ShardIndexValue,
    [int]$ShardCountValue,
    [int]$LimitValue,
    [string]$RepoRoot
  )

  if ($LimitValue -lt 0) {
    throw "contract FAIL: limit must be non-negative"
  }
  if ($ShardCountValue -lt 0) {
    throw "contract FAIL: shard-count must be non-negative"
  }
  if (($ShardIndexValue -ge 0) -and ($ShardCountValue -le 0)) {
    throw "contract FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCountValue -gt 0) -and (($ShardIndexValue -lt 0) -or ($ShardIndexValue -ge $ShardCountValue))) {
    throw "contract FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
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
      throw "contract FAIL: fixture-list entries did not match recovery fixtures ($($missing -join ', '))"
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
    throw "contract FAIL: no recovery fixtures matched the requested selection"
  }

  return $selected
}

function Assert-RecoveryFixtureClass {
  param(
    [object[]]$Fixtures,
    [string]$FixtureKind
  )

  $nonObjc3Fixtures = @($Fixtures | Where-Object { $_.Extension -ine ".objc3" })
  if ($nonObjc3Fixtures.Count -gt 0) {
    $sample = ($nonObjc3Fixtures | Select-Object -First 3 | ForEach-Object { $_.FullName.Replace("\", "/") }) -join ", "
    throw "contract FAIL: $FixtureKind fixture class resolved non-.objc3 entries (sample: $sample)"
  }

  $dispatchFixtures = @($Fixtures | Where-Object { $_.FullName -match "[\\/](lowering_dispatch|message_dispatch|dispatch)[\\/]" })
  if ($dispatchFixtures.Count -gt 0) {
    $sample = ($dispatchFixtures | Select-Object -First 3 | ForEach-Object { $_.FullName.Replace("\", "/") }) -join ", "
    throw "contract FAIL: $FixtureKind fixture class resolved dispatch/lowering fixtures (sample: $sample)"
  }

  Write-Output ("fixture-class: kind={0} expected=recovery-objc3 count={1}" -f $FixtureKind, $Fixtures.Count)
}

Export-ModuleMember -Function @(
  "Assert-RecoveryFixtureClass",
  "Get-RecoveryFixtures",
  "Get-RequestedRelativePaths",
  "Select-RecoveryFixtureEntries"
)
