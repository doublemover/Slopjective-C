$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "paths.psm1") -Force -DisableNameChecking

function Get-Objc3cNativeFixtureMatrixFixtures {
  param(
    [Parameter(Mandatory = $true)][string]$Directory,
    [Parameter(Mandatory = $true)][string]$FixtureKind
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "matrix FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File |
      Where-Object { $_.Extension -in @(".objc3", ".m") } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "matrix FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return $fixtures
}

function Get-Objc3cNativeFixtureMatrixRequestedRelativePaths {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureListPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $resolvedFixtureList = if ([System.IO.Path]::IsPathRooted($FixtureListPath)) { $FixtureListPath } else { Join-Path $RepoRoot $FixtureListPath }
  if (!(Test-Path -LiteralPath $resolvedFixtureList -PathType Leaf)) {
    throw "matrix FAIL: missing fixture list at $resolvedFixtureList"
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

function Select-Objc3cNativeFixtureMatrixFixtures {
  param(
    [Parameter(Mandatory = $true)][object[]]$Fixtures,
    [string]$FixtureListPath = "",
    [string]$FixtureGlobPattern = "",
    [int]$ShardIndexValue = -1,
    [int]$ShardCountValue = 0,
    [int]$LimitValue = 0,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  if ($LimitValue -lt 0) {
    throw "matrix FAIL: limit must be non-negative"
  }
  if ($ShardCountValue -lt 0) {
    throw "matrix FAIL: shard-count must be non-negative"
  }
  if (($ShardIndexValue -ge 0) -and ($ShardCountValue -le 0)) {
    throw "matrix FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCountValue -gt 0) -and (($ShardIndexValue -lt 0) -or ($ShardIndexValue -ge $ShardCountValue))) {
    throw "matrix FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
  }

  $selected = @($Fixtures)

  if (-not [string]::IsNullOrWhiteSpace($FixtureListPath)) {
    $requested = Get-Objc3cNativeFixtureMatrixRequestedRelativePaths `
      -FixtureListPath $FixtureListPath `
      -RepoRoot $RepoRoot
    $selected = @($selected | Where-Object { $requested.Contains((Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $_.FullName -Root $RepoRoot)) })
    $matched = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
    foreach ($fixture in $selected) {
      $null = $matched.Add((Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $fixture.FullName -Root $RepoRoot))
    }
    $missing = @()
    foreach ($requestedPath in $requested) {
      if (-not $matched.Contains($requestedPath)) {
        $missing += $requestedPath
      }
    }
    if ($missing.Count -gt 0) {
      throw "matrix FAIL: fixture-list entries did not match matrix fixtures ($($missing -join ', '))"
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($FixtureGlobPattern)) {
    $pattern = [System.Management.Automation.WildcardPattern]::new(
      $FixtureGlobPattern.Replace('\', '/'),
      [System.Management.Automation.WildcardOptions]::IgnoreCase
    )
    $selected = @($selected | Where-Object { $pattern.IsMatch((Get-Objc3cNativeFixtureMatrixRepoRelativePath -Path $_.FullName -Root $RepoRoot)) })
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
    throw "matrix FAIL: no fixture-matrix positive fixtures matched the requested selection"
  }

  return $selected
}

function Get-Objc3cNativeFixtureMatrixCompileArgs {
  param([Parameter(Mandatory = $true)][object]$Fixture)

  $metaPath = Join-Path $Fixture.DirectoryName ("{0}.meta.json" -f $Fixture.BaseName)
  if (!(Test-Path -LiteralPath $metaPath -PathType Leaf)) {
    return @()
  }

  $meta = Get-Content -LiteralPath $metaPath -Raw | ConvertFrom-Json
  if (!($meta.PSObject.Properties.Name -contains "execution")) {
    return @()
  }
  $execution = $meta.execution
  if (!($execution.PSObject.Properties.Name -contains "native_compile_args")) {
    return @()
  }

  $args = @()
  foreach ($arg in @($execution.native_compile_args)) {
    $args += [string]$arg
  }
  return $args
}

Export-ModuleMember -Function @(
  "Get-Objc3cNativeFixtureMatrixFixtures",
  "Select-Objc3cNativeFixtureMatrixFixtures",
  "Get-Objc3cNativeFixtureMatrixCompileArgs"
)
