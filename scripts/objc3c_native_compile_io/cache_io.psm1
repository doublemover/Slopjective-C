$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$hashModule = Join-Path (Split-Path -Parent $PSScriptRoot) "objc3c_native_compile_provenance/hash_io.psm1"
$artifactIoModule = Join-Path $PSScriptRoot "artifact_io.psm1"
$resultParsingModule = Join-Path $PSScriptRoot "result_parsing.psm1"
foreach ($dependencyModule in @($hashModule, $artifactIoModule, $resultParsingModule)) {
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
    [string]$ExpectedEntryContractId,
    [string]$AllowedDestinationRoot
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

  $manifest = @($metadata.output_files)
  if ($null -eq $metadata.output_files -or $manifest.Count -eq 0) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "manifest_missing"
    return New-Objc3cNativeCompileCacheMiss
  }
  $resolvedFilesRoot = (Resolve-Path -LiteralPath $FilesDir).Path
  $actualFiles = @(Get-ChildItem -LiteralPath $FilesDir -Recurse -File | ForEach-Object { [System.IO.Path]::GetRelativePath($resolvedFilesRoot, $_.FullName).Replace('\','/') } | Sort-Object)
  $manifestPaths = @($manifest | ForEach-Object { [string]$_.path } | Sort-Object)
  if (($actualFiles -join "`n") -cne ($manifestPaths -join "`n")) {
    Write-Objc3cNativeCompileCacheRecoverySignal -Reason "manifest_file_set_mismatch"
    return New-Objc3cNativeCompileCacheMiss
  }
  foreach ($row in $manifest) {
    $relative = [string]$row.path
    $normalizedRelative = $relative.Replace('\','/')
    if ([System.IO.Path]::IsPathRooted($relative) -or (($normalizedRelative.Split('/') -contains '..'))) { return New-Objc3cNativeCompileCacheMiss }
    $source = Join-Path $FilesDir $relative
    if (!(Test-Path -LiteralPath $source -PathType Leaf) -or (Get-FileSha256Hex -Path $source) -ne [string]$row.sha256) {
      Write-Objc3cNativeCompileCacheRecoverySignal -Reason "manifest_hash_mismatch"
      return New-Objc3cNativeCompileCacheMiss
    }
  }
  if ([string]::IsNullOrWhiteSpace($AllowedDestinationRoot)) { return New-Objc3cNativeCompileCacheMiss }
  $allowedFull = [System.IO.Path]::GetFullPath($AllowedDestinationRoot).TrimEnd('\','/')
  $destinationFull = [System.IO.Path]::GetFullPath($DestinationRoot).TrimEnd('\','/')
  $relativeDestination = [System.IO.Path]::GetRelativePath($allowedFull, $destinationFull)
  if ($relativeDestination -eq '.' -or [System.IO.Path]::IsPathRooted($relativeDestination) -or
      $relativeDestination.Replace('\','/').Split('/') -contains '..') { return New-Objc3cNativeCompileCacheMiss }
  # Reparse-point destinations can resolve outside the declared workspace.
  $probe = $destinationFull
  while ($probe -and $probe -ne $allowedFull) {
    if ((Test-Path -LiteralPath $probe) -and
        ((Get-Item -LiteralPath $probe -Force).Attributes -band [System.IO.FileAttributes]::ReparsePoint)) {
      return New-Objc3cNativeCompileCacheMiss
    }
    $probe = Split-Path -Parent $probe
  }
  $restoreStage = $null
  $previousRoot = $null
  try {
    $destinationParent = Split-Path -Parent $destinationFull
    New-Item -ItemType Directory -Force -Path $destinationParent | Out-Null
    $restoreStage = Join-Path $destinationParent (".cache-restore-" + [Guid]::NewGuid().ToString("N"))
    Copy-Objc3cNativeCompileDirectoryContents -SourceRoot $FilesDir -DestinationRoot $restoreStage
    if ((Get-Objc3cNativeCompileDirectoryDigest -Path $restoreStage) -ne $actualDigest) {
      throw "staged cache bytes changed during restore"
    }
    # Keep the previous output recoverable. Stage and validate before replacing
    # the visible directory, and never recursively delete a caller's output.
    if (Test-Path -LiteralPath $destinationFull) {
      $previousRoot = Join-Path $destinationParent (".cache-previous-" + [Guid]::NewGuid().ToString("N"))
      [System.IO.Directory]::Move($destinationFull, $previousRoot)
    }
    [System.IO.Directory]::Move($restoreStage, $destinationFull)
  } catch {
    if ($previousRoot -and !(Test-Path -LiteralPath $destinationFull)) {
      [System.IO.Directory]::Move($previousRoot, $destinationFull)
    }
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
  $allowedRoot = if ($CacheContext.PSObject.Properties.Name -contains 'allowed_destination_root') { [string]$CacheContext.allowed_destination_root } else { '' }
  return Try-RestoreObjc3cNativeCompileCacheEntry `
    -EntryDir $entryDir `
    -FilesDir (Join-Path $entryDir "files") `
    -ExitPath (Join-Path $entryDir "exit_code.txt") `
    -ReadyPath (Join-Path $entryDir "ready.marker") `
    -DestinationRoot $DestinationRoot `
    -CacheKey ([string]$CacheContext.cache_key) `
    -ExpectedEntryContractId ([string]$CacheContext.entry_contract_id) `
    -AllowedDestinationRoot $allowedRoot
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
    $sourceFull = [System.IO.Path]::GetFullPath($SourceRoot)
    $cacheFull = [System.IO.Path]::GetFullPath($cacheRoot)
    $cacheRelativeToSource = [System.IO.Path]::GetRelativePath($sourceFull, $cacheFull).Replace('\','/')
    # An output root containing its cache would recursively copy the stage
    # into itself. Reject it before creating any directories.
    if (-not [System.IO.Path]::IsPathRooted($cacheRelativeToSource) -and
        -not ($cacheRelativeToSource.Split('/') -contains '..')) { return }
    if ((Get-Item -LiteralPath $sourceFull -Force).Attributes -band [System.IO.FileAttributes]::ReparsePoint) { return }
    if (@(Get-ChildItem -LiteralPath $sourceFull -Recurse -Force |
          Where-Object { $_.Attributes -band [System.IO.FileAttributes]::ReparsePoint }).Count) { return }
    New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
    $stagingDir = Join-Path $cacheRoot ("_stage_" + [Guid]::NewGuid().ToString("N"))
    $stageFilesDir = Join-Path $stagingDir "files"
    New-Item -ItemType Directory -Force -Path $stageFilesDir | Out-Null

    Copy-Objc3cNativeCompileDirectoryContents -SourceRoot $SourceRoot -DestinationRoot $stageFilesDir
    $outputDigest = Get-Objc3cNativeCompileDirectoryDigest -Path $stageFilesDir
    $outputFiles = @(Get-ChildItem -LiteralPath $stageFilesDir -Recurse -File | Sort-Object FullName | ForEach-Object { [ordered]@{ path = [System.IO.Path]::GetRelativePath((Resolve-Path -LiteralPath $stageFilesDir).Path, $_.FullName).Replace('\', '/'); sha256 = Get-FileSha256Hex -Path $_.FullName; byte_count = [long]$_.Length } })
    Set-Content -LiteralPath (Join-Path $stagingDir "exit_code.txt") -Value "$CompileExit" -Encoding ascii
    $metadataPayload = [ordered]@{
      entry_contract_id = [string]$CacheContext.entry_contract_id
      schema_version = 2
      output_files = $outputFiles
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
      [System.IO.Directory]::Move($stagingDir, $collisionDir)
    } else {
      try { [System.IO.Directory]::Move($stagingDir, $entryDir) } catch {
        if (!(Test-Path -LiteralPath $entryDir -PathType Container)) { throw }
        $collisionDir = Join-Path $cacheRoot ("_collision_" + $cacheKey + "_" + [Guid]::NewGuid().ToString("N"))
        [System.IO.Directory]::Move($stagingDir, $collisionDir)
      }
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
