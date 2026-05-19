$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$artifactIoModule = Join-Path $PSScriptRoot "artifact_io.psm1"
$resultParsingModule = Join-Path $PSScriptRoot "result_parsing.psm1"
foreach ($dependencyModule in @($artifactIoModule, $resultParsingModule)) {
  if (!(Test-Path -LiteralPath $dependencyModule -PathType Leaf)) {
    Write-Error "native compile cache IO dependency missing at $dependencyModule"
    exit 2
  }
  Import-Module $dependencyModule -Force -DisableNameChecking
}

function New-Objc3cNativeCompileCacheMiss {
  return [pscustomobject]@{
    restored = $false
    exit_code = 0
  }
}

function Try-RestoreObjc3cNativeCompileCacheEntry {
  param(
    [string]$EntryDir,
    [string]$FilesDir,
    [string]$ExitPath,
    [string]$ReadyPath,
    [string]$DestinationRoot,
    [string]$CacheKey,
    [string]$ExpectedEntryContractId
  )

  if (!(Test-Path -LiteralPath $EntryDir -PathType Container)) {
    return New-Objc3cNativeCompileCacheMiss
  }

  if (!(Test-Path -LiteralPath $ReadyPath -PathType Leaf) -or
      !(Test-Path -LiteralPath $ExitPath -PathType Leaf) -or
      !(Test-Path -LiteralPath $FilesDir -PathType Container)) {
    return New-Objc3cNativeCompileCacheMiss
  }

  $metadataPath = Join-Path $EntryDir "metadata.json"
  if (!(Test-Path -LiteralPath $metadataPath -PathType Leaf)) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_missing"
    return New-Objc3cNativeCompileCacheMiss
  }

  $metadataRead = Read-Objc3cNativeCompileCacheMetadata -Path $metadataPath
  if (-not [bool]$metadataRead.parsed) {
    return New-Objc3cNativeCompileCacheMiss
  }
  $metadata = $metadataRead.metadata

  if ([string]$metadata.entry_contract_id -ne $ExpectedEntryContractId) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_contract_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }

  if ([string]$metadata.cache_key -ne [string]$CacheKey) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_cache_key_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }

  $parsedExit = Read-Objc3cNativeCompileExitCode -Path $ExitPath
  if (-not [bool]$parsedExit.parsed) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_exit_code_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }

  $metadataExit = ConvertTo-Objc3cNativeCompileExitCode -Value ([string]$metadata.compile_exit_code)
  if (-not [bool]$metadataExit.parsed) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_exit_code_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }

  if ([int]$metadataExit.exit_code -ne [int]$parsedExit.exit_code) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_exit_code_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }

  $expectedDigest = [string]$metadata.output_digest_sha256
  $actualDigest = Get-Objc3cNativeCompileDirectoryDigest -Path $FilesDir
  if ([string]::IsNullOrWhiteSpace($expectedDigest) -or
      ($actualDigest -ne $expectedDigest.ToLowerInvariant())) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "metadata_digest_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }

  try {
    Copy-Objc3cNativeCompileDirectoryContents -SourceRoot $FilesDir -DestinationRoot $DestinationRoot
  } catch {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "restore_failed"
    return New-Objc3cNativeCompileCacheMiss
  }

  return [pscustomobject]@{
    restored = $true
    exit_code = [int]$parsedExit.exit_code
  }
}

function Restore-Objc3cNativeCompileCacheEntry {
  param(
    [object]$CacheContext,
    [string]$DestinationRoot
  )

  if ($null -eq $CacheContext -or [string]::IsNullOrWhiteSpace([string]$CacheContext.cache_key)) {
    return New-Objc3cNativeCompileCacheMiss
  }

  $entryDir = Join-Path ([string]$CacheContext.cache_root) ([string]$CacheContext.cache_key)
  return Try-RestoreObjc3cNativeCompileCacheEntry `
    -EntryDir $entryDir `
    -FilesDir (Join-Path $entryDir "files") `
    -ExitPath (Join-Path $entryDir "exit_code.txt") `
    -ReadyPath (Join-Path $entryDir "ready.marker") `
    -DestinationRoot $DestinationRoot `
    -CacheKey ([string]$CacheContext.cache_key) `
    -ExpectedEntryContractId ([string]$CacheContext.entry_contract_id)
}

function Save-Objc3cNativeCompileCacheEntry {
  param(
    [object]$CacheContext,
    [string]$SourceRoot,
    [int]$CompileExit
  )

  if ($null -eq $CacheContext -or [string]::IsNullOrWhiteSpace([string]$CacheContext.cache_key)) {
    return
  }

  try {
    $cacheRoot = [string]$CacheContext.cache_root
    $cacheKey = [string]$CacheContext.cache_key
    New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
    $stagingDir = Join-Path $cacheRoot ("_stage_" + [Guid]::NewGuid().ToString("N"))
    $stageFilesDir = Join-Path $stagingDir "files"
    New-Item -ItemType Directory -Force -Path $stageFilesDir | Out-Null

    Copy-Objc3cNativeCompileDirectoryContents -SourceRoot $SourceRoot -DestinationRoot $stageFilesDir
    $outputDigest = Get-Objc3cNativeCompileDirectoryDigest -Path $stageFilesDir
    Set-Content -LiteralPath (Join-Path $stagingDir "exit_code.txt") -Value "$CompileExit" -Encoding ascii
    $metadataPayload = [ordered]@{
      entry_contract_id = [string]$CacheContext.entry_contract_id
      schema_version = 1
      cache_key = $cacheKey
      compile_exit_code = [int]$CompileExit
      output_digest_sha256 = $outputDigest
      required_entry_files = @("files", "exit_code.txt", "ready.marker", "metadata.json")
    }
    Set-Content -LiteralPath (Join-Path $stagingDir "metadata.json") -Value ($metadataPayload | ConvertTo-Json -Depth 8) -Encoding utf8
    Set-Content -LiteralPath (Join-Path $stagingDir "ready.marker") -Value "ready" -Encoding ascii

    $entryDir = Join-Path $cacheRoot $cacheKey
    if (Test-Path -LiteralPath $entryDir -PathType Container) {
      # Preserve existing entry and retain this write as a traceable collision artifact.
      $collisionDir = Join-Path $cacheRoot ("_collision_" + $cacheKey + "_" + [Guid]::NewGuid().ToString("N"))
      Move-Item -LiteralPath $stagingDir -Destination $collisionDir -Force
    } else {
      Move-Item -LiteralPath $stagingDir -Destination $entryDir -Force
    }
  } catch {
    # Fail closed: cache population must never block compile wrapper.
  }
}

Export-ModuleMember -Function @(
  "Restore-Objc3cNativeCompileCacheEntry",
  "Save-Objc3cNativeCompileCacheEntry",
  "Try-RestoreObjc3cNativeCompileCacheEntry"
)
