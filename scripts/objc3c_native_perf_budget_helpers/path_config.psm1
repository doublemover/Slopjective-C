Set-StrictMode -Version Latest

function Get-RepoRelativePath {
  param(
    [string]$Path,
    [string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-ShortHash {
  param([string]$Value)

  $sha1 = [System.Security.Cryptography.SHA1]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
    $hashBytes = $sha1.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant().Substring(0, 10)
  } finally {
    $sha1.Dispose()
  }
}

function Get-Fixtures {
  param(
    [string]$Directory,
    [string]$FixtureKind,
    [string[]]$Extensions = @(".objc3", ".m")
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "perf-budget FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File |
      Where-Object { $_.Extension -in $Extensions } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "perf-budget FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return @(
    $fixtures |
      ForEach-Object {
        [pscustomobject]@{
          FullName = [string]$_.FullName
          Name = [string]$_.Name
          Extension = [string]$_.Extension
        }
      }
  )
}

function Get-PerfFixtureDirectories {
  param(
    [string]$RepoRoot,
    [string]$BaselineDirectory,
    [string]$RequiredDispatchDirectory,
    [string[]]$DispatchCandidateDirectories,
    [string]$ExtraDirectoriesRaw
  )

  $seen = New-Object "System.Collections.Generic.HashSet[string]" ([System.StringComparer]::OrdinalIgnoreCase)
  $directories = New-Object "System.Collections.Generic.List[object]"

  function Add-DirectoryEntry {
    param(
      [string]$RawPath,
      [string]$FixtureKind,
      [string]$SourceLabel,
      [bool]$Required,
      [string[]]$Extensions = @(".objc3", ".m")
    )

    if ([string]::IsNullOrWhiteSpace($RawPath)) {
      return
    }

    $candidate = $RawPath
    if (-not [System.IO.Path]::IsPathRooted($candidate)) {
      $candidate = Join-Path $RepoRoot $candidate
    }
    $fullPath = [System.IO.Path]::GetFullPath($candidate)
    if (!(Test-Path -LiteralPath $fullPath -PathType Container)) {
      if ($Required) {
        throw "perf-budget FAIL: missing $SourceLabel fixture directory at $fullPath"
      }
      return
    }

    if ($seen.Add($fullPath)) {
      $directories.Add([pscustomobject]@{
          directory = $fullPath
          fixture_kind = $FixtureKind
          source = $SourceLabel
          extensions = $Extensions
        }) | Out-Null
    }
  }

  Add-DirectoryEntry -RawPath $BaselineDirectory -FixtureKind "recovery-positive" -SourceLabel "baseline positive recovery" -Required $true -Extensions @(".objc3")
  Add-DirectoryEntry -RawPath $RequiredDispatchDirectory -FixtureKind "dispatch-positive" -SourceLabel "dispatch lowering suite" -Required $true -Extensions @(".m", ".objc3")
  foreach ($candidate in $DispatchCandidateDirectories) {
    Add-DirectoryEntry -RawPath $candidate -FixtureKind "dispatch-positive" -SourceLabel "dispatch positive" -Required $false -Extensions @(".m", ".objc3")
  }

  if (-not [string]::IsNullOrWhiteSpace($ExtraDirectoriesRaw)) {
    $extraDirectories = @(
      $ExtraDirectoriesRaw.Split(";") |
        ForEach-Object { $_.Trim() } |
        Where-Object { -not [string]::IsNullOrWhiteSpace($_) }
    )
    foreach ($extraDirectory in $extraDirectories) {
      Add-DirectoryEntry -RawPath $extraDirectory -FixtureKind "dispatch-positive" -SourceLabel "extra positive" -Required $true -Extensions @(".m", ".objc3")
    }
  }

  if ($directories.Count -eq 0) {
    throw "perf-budget FAIL: no positive fixture directories resolved"
  }

  return $directories.ToArray()
}

Export-ModuleMember -Function @(
  "Get-Fixtures",
  "Get-PerfFixtureDirectories",
  "Get-RepoRelativePath",
  "Get-ShortHash"
)
