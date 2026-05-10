$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

. (Join-Path $PSScriptRoot "objc3c_native_compile_provenance.ps1")

function Resolve-RepoBoundPath {
  param(
    [string]$RepoRoot,
    [string]$RelativeOrAbsolutePath,
    [string]$Label
  )

  if ([string]::IsNullOrWhiteSpace($RelativeOrAbsolutePath)) {
    Write-Error "$Label path is empty"
    exit 2
  }

  $candidatePath = $RelativeOrAbsolutePath
  if (-not [System.IO.Path]::IsPathRooted($candidatePath)) {
    $normalizedRelative = $candidatePath.Replace('\', '/')
    foreach ($segment in $normalizedRelative.Split('/')) {
      if ($segment -eq "..") {
        Write-Error "$Label path must not contain '..' relative segments: $RelativeOrAbsolutePath"
        exit 2
      }
    }
    $candidatePath = Join-Path $RepoRoot $candidatePath
  }

  $resolvedRoot = [System.IO.Path]::GetFullPath($RepoRoot).TrimEnd('\', '/')
  $resolvedCandidate = [System.IO.Path]::GetFullPath($candidatePath)
  $rootPrefix = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  if (($resolvedCandidate -ne $resolvedRoot) -and
      (-not $resolvedCandidate.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase))) {
    Write-Error "$Label path escapes repository root: $RelativeOrAbsolutePath"
    exit 2
  }

  return $resolvedCandidate
}

function Get-CacheKey {
  param(
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$CompilerSourcePath,
    [string]$WrapperScriptPath
  )

  if ([string]::IsNullOrWhiteSpace($InputPath)) {
    return $null
  }
  if (!(Test-Path -LiteralPath $InputPath -PathType Leaf)) {
    return $null
  }

  $inputHash = Get-FileSha256Hex -Path $InputPath
  $compilerSourceHash = Get-OptionalFileHash -Path $CompilerSourcePath
  $wrapperScriptHash = Get-OptionalFileHash -Path $WrapperScriptPath
  $payload = [ordered]@{
    version = 2
    input_sha256 = $inputHash
    compiler_source_sha256 = $compilerSourceHash
    wrapper_script_sha256 = $wrapperScriptHash
    args = $ArgsWithoutOutDir
  }
  $payloadJson = $payload | ConvertTo-Json -Compress -Depth 6
  $payloadBytes = [System.Text.Encoding]::UTF8.GetBytes($payloadJson)
  return Get-Sha256HexFromBytes -Bytes $payloadBytes
}

function Get-DirectoryDeterminismDigest {
  param([string]$Path)

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }
  if (!(Test-Path -LiteralPath $Path -PathType Container)) {
    return ""
  }

  $resolvedRoot = (Resolve-Path -LiteralPath $Path).Path
  $files = Get-ChildItem -LiteralPath $Path -Recurse -File | Sort-Object -Property FullName
  $rows = New-Object System.Collections.Generic.List[string]
  foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($resolvedRoot.Length).TrimStart('\', '/').Replace('\', '/')
    $fileHash = Get-FileSha256Hex -Path $file.FullName
    $rows.Add($relativePath + ":" + $fileHash)
  }

  $payloadText = [string]::Join("`n", $rows.ToArray())
  $payloadBytes = [System.Text.Encoding]::UTF8.GetBytes($payloadText)
  return Get-Sha256HexFromBytes -Bytes $payloadBytes
}

function Write-CacheRecoverySignal {
  param(
    [Parameter(Mandatory = $true)]
    [string]$Reason
  )

  Write-Output ("cache_recovery=" + $Reason)
}

function Try-RestoreCacheEntry {
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
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  if (!(Test-Path -LiteralPath $ReadyPath -PathType Leaf) -or
      !(Test-Path -LiteralPath $ExitPath -PathType Leaf) -or
      !(Test-Path -LiteralPath $FilesDir -PathType Container)) {
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  $metadataPath = Join-Path $EntryDir "metadata.json"
  if (!(Test-Path -LiteralPath $metadataPath -PathType Leaf)) {
    Write-CacheRecoverySignal -Reason "metadata_missing"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  try {
    $metadata = Get-Content -LiteralPath $metadataPath -Raw | ConvertFrom-Json
  } catch {
    Write-CacheRecoverySignal -Reason "metadata_invalid"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  if ([string]$metadata.entry_contract_id -ne $ExpectedEntryContractId) {
    Write-CacheRecoverySignal -Reason "metadata_contract_mismatch"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  if ([string]$metadata.cache_key -ne [string]$CacheKey) {
    Write-CacheRecoverySignal -Reason "metadata_cache_key_mismatch"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  $rawExitCode = (Get-Content -LiteralPath $ExitPath -Raw).Trim()
  $parsedExitCode = 0
  if (-not [int]::TryParse($rawExitCode, [ref]$parsedExitCode)) {
    Write-CacheRecoverySignal -Reason "metadata_exit_code_mismatch"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  $metadataExitCode = 0
  if (-not [int]::TryParse([string]$metadata.compile_exit_code, [ref]$metadataExitCode)) {
    Write-CacheRecoverySignal -Reason "metadata_exit_code_mismatch"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  if ($metadataExitCode -ne $parsedExitCode) {
    Write-CacheRecoverySignal -Reason "metadata_exit_code_mismatch"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  $expectedDigest = [string]$metadata.output_digest_sha256
  $actualDigest = Get-DirectoryDeterminismDigest -Path $FilesDir
  if ([string]::IsNullOrWhiteSpace($expectedDigest) -or
      ($actualDigest -ne $expectedDigest.ToLowerInvariant())) {
    Write-CacheRecoverySignal -Reason "metadata_digest_mismatch"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  try {
    Copy-DirectoryContents -SourceRoot $FilesDir -DestinationRoot $DestinationRoot
  } catch {
    Write-CacheRecoverySignal -Reason "restore_failed"
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  return [pscustomobject]@{
    restored = $true
    exit_code = $parsedExitCode
  }
}

function Copy-DirectoryContents {
  param(
    [string]$SourceRoot,
    [string]$DestinationRoot
  )

  if (!(Test-Path -LiteralPath $SourceRoot -PathType Container)) {
    return
  }

  New-Item -ItemType Directory -Force -Path $DestinationRoot | Out-Null
  $resolvedSourceRoot = (Resolve-Path -LiteralPath $SourceRoot).Path
  $files = Get-ChildItem -LiteralPath $SourceRoot -Recurse -File | Sort-Object -Property FullName

  foreach ($file in $files) {
    $relativePath = $file.FullName.Substring($resolvedSourceRoot.Length).TrimStart('\', '/')
    $destination = Join-Path $DestinationRoot $relativePath
    $parent = Split-Path -Parent $destination
    if (![string]::IsNullOrWhiteSpace($parent)) {
      New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }
    Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
  }
}

function Get-Objc3cNativeCompileInputPath {
  param([string[]]$ArgsWithoutOutDir)

  $argsWithoutOutDir = @($ArgsWithoutOutDir)
  if ($argsWithoutOutDir.Count -le 0) {
    return $null
  }

  $inputCandidate = $argsWithoutOutDir[0]
  if ([string]::IsNullOrWhiteSpace($inputCandidate)) {
    return $null
  }

  return [System.IO.Path]::GetFullPath($inputCandidate)
}

function New-Objc3cNativeCompileCacheContext {
  param(
    [string]$RepoRoot,
    [string]$InputPath,
    [string[]]$ArgsWithoutOutDir,
    [string]$WrapperScriptPath
  )

  $cacheRoot = Join-Path $RepoRoot "tmp/artifacts/objc3c-native/cache"
  $compilerSourcePath = Join-Path $RepoRoot "native/objc3c/src/main.cpp"
  $cacheEntryContractId = "objc3c-native-cache-entry/parser_build-recovery-determinism-hardening-v1"
  $cacheKey = Get-CacheKey `
    -InputPath $InputPath `
    -ArgsWithoutOutDir $ArgsWithoutOutDir `
    -CompilerSourcePath $compilerSourcePath `
    -WrapperScriptPath $WrapperScriptPath

  return [pscustomobject]@{
    cache_root = $cacheRoot
    compiler_source_path = $compilerSourcePath
    entry_contract_id = $cacheEntryContractId
    cache_key = $cacheKey
  }
}

function Restore-Objc3cNativeCompileCacheEntry {
  param(
    [object]$CacheContext,
    [string]$DestinationRoot
  )

  if ($null -eq $CacheContext -or [string]::IsNullOrWhiteSpace([string]$CacheContext.cache_key)) {
    return [pscustomobject]@{
      restored = $false
      exit_code = 0
    }
  }

  $entryDir = Join-Path ([string]$CacheContext.cache_root) ([string]$CacheContext.cache_key)
  return Try-RestoreCacheEntry `
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

    Copy-DirectoryContents -SourceRoot $SourceRoot -DestinationRoot $stageFilesDir
    $outputDigest = Get-DirectoryDeterminismDigest -Path $stageFilesDir
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
  "Get-FileSha256Hex",
  "Get-Objc3cNativeCompileInputPath",
  "New-Objc3cNativeCompileCacheContext",
  "Resolve-RepoBoundPath",
  "Restore-Objc3cNativeCompileCacheEntry",
  "Save-Objc3cNativeCompileCacheEntry",
  "Write-CompileOutputProvenance"
)
