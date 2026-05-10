Set-StrictMode -Version Latest

function Read-JsonObject {
  param([string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "perf-budget FAIL: missing JSON artifact at $Path"
  }

  try {
    if ($PSVersionTable.PSVersion.Major -ge 6) {
      return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json -AsHashtable)
    }
    return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json)
  } catch {
    throw "perf-budget FAIL: invalid JSON artifact at $Path"
  }
}

function Get-FileSha256Hex {
  param([string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    throw "perf-budget FAIL: missing file for hashing at $Path"
  }

  return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

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

function Get-CompileArtifactSurface {
  param(
    [string]$OutputDirectory,
    [string]$RepoRoot
  )

  $manifestPath = Join-Path $OutputDirectory "module.manifest.json"
  if (!(Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
    return [ordered]@{
      manifest_present = $false
    }
  }

  $manifest = Read-JsonObject -Path $manifestPath
  $frontend = $manifest["frontend"]
  $pipeline = if ($null -ne $frontend) { $frontend["pipeline"] } else { $null }
  $stages = if ($null -ne $pipeline) { $pipeline["stages"] } else { $null }
  $parserStage = if ($null -ne $stages) { $stages["parser"] } else { $null }
  $semanticStage = if ($null -ne $stages) { $stages["semantic"] } else { $null }
  $semanticSurface = if ($null -ne $pipeline) { $pipeline["semantic_surface"] } else { $null }
  $loweringSurface = $manifest["lowering_incremental_module_cache_invalidation"]

  return [ordered]@{
    manifest_present = $true
    manifest_path = (Get-RepoRelativePath -Path $manifestPath -Root $RepoRoot)
    manifest_sha256 = (Get-FileSha256Hex -Path $manifestPath)
    parser_diagnostics = if ($null -ne $parserStage -and $null -ne $parserStage["diagnostics"]) { [int]$parserStage["diagnostics"] } else { 0 }
    semantic_diagnostics = if ($null -ne $semanticStage -and $null -ne $semanticStage["diagnostics"]) { [int]$semanticStage["diagnostics"] } else { 0 }
    semantic_skipped = if ($null -ne $pipeline -and $null -ne $pipeline["semantic_skipped"]) { [bool]$pipeline["semantic_skipped"] } else { $false }
    declared_globals = if ($null -ne $semanticSurface -and $null -ne $semanticSurface["declared_globals"]) { [int]$semanticSurface["declared_globals"] } else { 0 }
    declared_functions = if ($null -ne $semanticSurface -and $null -ne $semanticSurface["declared_functions"]) { [int]$semanticSurface["declared_functions"] } else { 0 }
    lowering_replay_key = if ($null -ne $loweringSurface -and $null -ne $loweringSurface["replay_key"]) { [string]$loweringSurface["replay_key"] } else { "" }
  }
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

  return $fixtures
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

function Invoke-TimedNativeCommand {
  param(
    [string]$Command,
    [string[]]$Arguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    $exitCode = $LASTEXITCODE
  } finally {
    $stopwatch.Stop()
    $ErrorActionPreference = $previousErrorAction
  }

  return [pscustomobject]@{
    exit_code = $exitCode
    elapsed_ms = [Math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
  }
}

function Invoke-TimedWrapperCommand {
  param(
    [string]$ScriptPath,
    [string[]]$ScriptArguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  $outputLines = @()
  $stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
  try {
    $ErrorActionPreference = "Continue"
    $outputLines = & $ScriptPath @ScriptArguments 2>&1
    $exitCode = $LASTEXITCODE
  } finally {
    $stopwatch.Stop()
    $ErrorActionPreference = $previousErrorAction
  }

  $outputText = ""
  if ($null -ne $outputLines) {
    $outputText = (($outputLines | ForEach-Object { $_.ToString() }) -join [Environment]::NewLine)
  }
  Set-Content -LiteralPath $LogPath -Value $outputText -Encoding utf8

  return [pscustomobject]@{
    exit_code = $exitCode
    elapsed_ms = [Math]::Round($stopwatch.Elapsed.TotalMilliseconds, 3)
    output_text = $outputText
  }
}

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
  "Get-CompileArtifactSurface",
  "Get-Fixtures",
  "Get-PerfFixtureDirectories",
  "Get-RepoRelativePath",
  "Get-ShortHash",
  "Invoke-TimedNativeCommand",
  "Invoke-TimedWrapperCommand",
  "Parse-CacheHitFlag"
)
