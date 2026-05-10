Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "path_config.psm1") -Force -DisableNameChecking

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

Export-ModuleMember -Function @(
  "Get-CompileArtifactSurface"
)
