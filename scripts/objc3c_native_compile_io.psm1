$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Get-Sha256HexFromBytes {
  param([byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Get-RegexMatchCount {
  param(
    [string]$Text,
    [string]$Pattern,
    [System.Text.RegularExpressions.RegexOptions]$Options = [System.Text.RegularExpressions.RegexOptions]::Multiline
  )

  if ($null -eq $Text) {
    return 0
  }
  return ([regex]::Matches($Text, $Pattern, $Options)).Count
}

function Get-ReplayKeyCounter {
  param(
    [string]$ReplayKey,
    [string]$CounterName
  )

  if ([string]::IsNullOrWhiteSpace($ReplayKey) -or [string]::IsNullOrWhiteSpace($CounterName)) {
    return 0
  }

  $match = [regex]::Match($ReplayKey, ([regex]::Escape($CounterName) + "=([0-9]+)"))
  if (-not $match.Success) {
    return 0
  }
  return [int]$match.Groups[1].Value
}

function Get-FileSha256Hex {
  param([string]$Path)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  $stream = [System.IO.File]::OpenRead($Path)
  try {
    $hashBytes = $sha256.ComputeHash($stream)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $stream.Dispose()
    $sha256.Dispose()
  }
}

function Get-OptionalFileHash {
  param($Path)

  $pathText = if ($null -eq $Path) { "" } else { [string]$Path }

  if ([string]::IsNullOrWhiteSpace($pathText)) {
    return ""
  }
  if (!(Test-Path -LiteralPath $pathText -PathType Leaf)) {
    return ""
  }
  return Get-FileSha256Hex -Path $pathText
}

function Get-RepoRelativeDisplayPath {
  param(
    [string]$RepoRoot,
    [string]$Path
  )

  if ([string]::IsNullOrWhiteSpace($Path)) {
    return ""
  }

  $resolvedRoot = [System.IO.Path]::GetFullPath($RepoRoot).TrimEnd('\', '/')
  $resolvedPath = [System.IO.Path]::GetFullPath($Path)
  $rootPrefix = $resolvedRoot + [System.IO.Path]::DirectorySeparatorChar
  if ($resolvedPath -eq $resolvedRoot) {
    return "."
  }
  if ($resolvedPath.StartsWith($rootPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    return [System.IO.Path]::GetRelativePath($resolvedRoot, $resolvedPath).Replace('\', '/')
  }
  return $resolvedPath.Replace('\', '/')
}

function Get-CompileOutputTruthfulness {
  param(
    [string]$CompileDir,
    [string]$EmitPrefix
  )

  if ([string]::IsNullOrWhiteSpace($CompileDir) -or !(Test-Path -LiteralPath $CompileDir -PathType Container)) {
    throw "compile output truthfulness check requires an existing compile directory"
  }
  if ([string]::IsNullOrWhiteSpace($EmitPrefix)) {
    $EmitPrefix = "module"
  }

  $manifestPath = Join-Path $CompileDir ($EmitPrefix + ".manifest.json")
  $registrationManifestPath = Join-Path $CompileDir ($EmitPrefix + ".runtime-registration-manifest.json")
  $llvmIrPath = Join-Path $CompileDir ($EmitPrefix + ".ll")
  foreach ($requiredPath in @($manifestPath, $registrationManifestPath, $llvmIrPath)) {
    if (!(Test-Path -LiteralPath $requiredPath -PathType Leaf)) {
      throw "compile output truthfulness check missing required artifact '$requiredPath'"
    }
  }

  $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json -AsHashtable
  $registrationManifest = Get-Content -LiteralPath $registrationManifestPath -Raw | ConvertFrom-Json -AsHashtable
  $llvmIrText = Get-Content -LiteralPath $llvmIrPath -Raw

  $lowering = $manifest["lowering"]
  $propertySynthesis = $manifest["lowering_property_synthesis_ivar_binding"]
  $runtimeDispatchSymbol = ""
  if ($lowering -is [System.Collections.IDictionary]) {
    $runtimeDispatchSymbol = [string]$lowering["runtime_dispatch_symbol"]
  }
  if ([string]::IsNullOrWhiteSpace($runtimeDispatchSymbol)) {
    $runtimeDispatchSymbol = [string]$manifest["runtime_support_library_link_wiring_runtime_dispatch_symbol"]
  }
  if ([string]::IsNullOrWhiteSpace($runtimeDispatchSymbol)) {
    $runtimeDispatchSymbol = [string]$manifest["runtime_link_host_link_runtime_dispatch_symbol"]
  }
  if ([string]::IsNullOrWhiteSpace($runtimeDispatchSymbol)) {
    throw "compile output truthfulness check could not resolve the runtime dispatch symbol from the compile manifest"
  }

  $propertyDescriptorCountExpected = [int]$registrationManifest["property_descriptor_count"]
  $ivarDescriptorCountExpected = [int]$registrationManifest["ivar_descriptor_count"]
  $propertySynthesisSitesExpected = 0
  if ($propertySynthesis -is [System.Collections.IDictionary]) {
    $propertySynthesisSitesExpected = Get-ReplayKeyCounter -ReplayKey ([string]$propertySynthesis["replay_key"]) -CounterName "property_synthesis_sites"
  }

  $dispatchDeclarationCount = Get-RegexMatchCount -Text $llvmIrText -Pattern ("(?m)declare i32 @" + [regex]::Escape($runtimeDispatchSymbol) + "\(")
  $dispatchCallCount = Get-RegexMatchCount -Text $llvmIrText -Pattern ("(?m)call i32 @" + [regex]::Escape($runtimeDispatchSymbol) + "\(")
  $propertyDescriptorDefinitionCount = Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_meta_property_[0-9]+ = "
  $ivarDescriptorDefinitionCount = Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_meta_ivar_[0-9]+ = "
  $propertyDescriptorSectionPresent = (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_sec_property_descriptors = ") -ge 1
  $ivarDescriptorSectionPresent = (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^@__objc3_sec_ivar_descriptors = ") -ge 1
  $currentPropertyHelperCallCount =
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)call i32 @objc3_runtime_read_current_property_i32\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)call void @objc3_runtime_write_current_property_i32\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)call i32 @objc3_runtime_exchange_current_property_i32\(")
  $synthesizedAccessorDefinitionCount =
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^define i32 @objc3_method_.*_instance_.*\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^define i1 @objc3_method_.*_instance_.*\(") +
    (Get-RegexMatchCount -Text $llvmIrText -Pattern "(?m)^define void @objc3_method_.*_instance_.*\(")

  $propertyDescriptorCountsMatch = ($propertyDescriptorDefinitionCount -eq $propertyDescriptorCountExpected)
  $ivarDescriptorCountsMatch = ($ivarDescriptorDefinitionCount -eq $ivarDescriptorCountExpected)
  $synthesizedPropertySurfaceMatches = ($propertySynthesisSitesExpected -eq 0) -or (
    $propertyDescriptorCountExpected -gt 0 -and (
      (
        $currentPropertyHelperCallCount -gt 0 -and
        $synthesizedAccessorDefinitionCount -ge $propertySynthesisSitesExpected
      ) -or (
        $currentPropertyHelperCallCount -eq 0
      )
    )
  )
  $truthful = $dispatchDeclarationCount -ge 1 -and
    $propertyDescriptorSectionPresent -and
    $ivarDescriptorSectionPresent -and
    $propertyDescriptorCountsMatch -and
    $ivarDescriptorCountsMatch -and
    $synthesizedPropertySurfaceMatches

  $failures = New-Object System.Collections.Generic.List[string]
  if ($dispatchDeclarationCount -lt 1) {
    $failures.Add("missing LLVM declaration for runtime dispatch symbol '$runtimeDispatchSymbol'")
  }
  if (-not $propertyDescriptorSectionPresent) {
    $failures.Add("missing property descriptor aggregate section in emitted LLVM IR")
  }
  if (-not $ivarDescriptorSectionPresent) {
    $failures.Add("missing ivar descriptor aggregate section in emitted LLVM IR")
  }
  if (-not $propertyDescriptorCountsMatch) {
    $failures.Add("property descriptor count mismatch: registration manifest=$propertyDescriptorCountExpected emitted LLVM IR=$propertyDescriptorDefinitionCount")
  }
  if (-not $ivarDescriptorCountsMatch) {
    $failures.Add("ivar descriptor count mismatch: registration manifest=$ivarDescriptorCountExpected emitted LLVM IR=$ivarDescriptorDefinitionCount")
  }
  if (-not $synthesizedPropertySurfaceMatches) {
    $failures.Add("synthesized property lowering replay claims do not match emitted runtime-backed accessor/helper surface")
  }

  return [ordered]@{
    contract_id = "objc3c.native.compile.output.truthfulness.v1"
    llvm_ir_artifact = ($EmitPrefix + ".ll")
    manifest_artifact = ($EmitPrefix + ".manifest.json")
    registration_manifest_artifact = ($EmitPrefix + ".runtime-registration-manifest.json")
    verification_model = "compile-wrapper-cross-checks-manifest-and-runtime-registration-claims-against-emitted-llvm-ir"
    runtime_dispatch_symbol = $runtimeDispatchSymbol
    runtime_dispatch_declaration_count = $dispatchDeclarationCount
    runtime_dispatch_call_count = $dispatchCallCount
    property_descriptor_count_expected = $propertyDescriptorCountExpected
    property_descriptor_definition_count = $propertyDescriptorDefinitionCount
    property_descriptor_section_present = $propertyDescriptorSectionPresent
    ivar_descriptor_count_expected = $ivarDescriptorCountExpected
    ivar_descriptor_definition_count = $ivarDescriptorDefinitionCount
    ivar_descriptor_section_present = $ivarDescriptorSectionPresent
    property_synthesis_sites_expected = $propertySynthesisSitesExpected
    synthesized_accessor_definition_count = $synthesizedAccessorDefinitionCount
    current_property_helper_call_count = $currentPropertyHelperCallCount
    property_descriptor_counts_match = $propertyDescriptorCountsMatch
    ivar_descriptor_counts_match = $ivarDescriptorCountsMatch
    synthesized_property_surface_matches = $synthesizedPropertySurfaceMatches
    truthful = $truthful
    failures = @($failures.ToArray())
  }
}

function Write-CompileOutputProvenance {
  param(
    [string]$RepoRoot,
    [string]$CompileDir,
    [string]$EmitPrefix,
    $InputPath,
    $CompilerBinaryPath,
    $RuntimeLibraryPath,
    $WrapperScriptPath
  )

  if ([string]::IsNullOrWhiteSpace($CompileDir) -or !(Test-Path -LiteralPath $CompileDir -PathType Container)) {
    return
  }
  if ([string]::IsNullOrWhiteSpace($EmitPrefix)) {
    $EmitPrefix = "module"
  }
  $inputPathText = if ($null -eq $InputPath) { "" } else { [string]$InputPath }
  $compilerBinaryPathText = if ($null -eq $CompilerBinaryPath) { "" } else { [string]$CompilerBinaryPath }
  $runtimeLibraryPathText = if ($null -eq $RuntimeLibraryPath) { "" } else { [string]$RuntimeLibraryPath }
  $wrapperScriptPathText = if ($null -eq $WrapperScriptPath) { "" } else { [string]$WrapperScriptPath }
  $inputSourceDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $inputPathText
  $inputSourceHash = Get-OptionalFileHash -Path $inputPathText
  $compilerBinaryDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $compilerBinaryPathText
  $compilerBinaryHash = Get-OptionalFileHash -Path $compilerBinaryPathText
  $runtimeLibraryDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $runtimeLibraryPathText
  $runtimeLibraryHash = Get-OptionalFileHash -Path $runtimeLibraryPathText
  $wrapperScriptDisplay = Get-RepoRelativeDisplayPath -RepoRoot $RepoRoot -Path $wrapperScriptPathText
  $wrapperScriptHash = Get-OptionalFileHash -Path $wrapperScriptPathText
  $truthfulness = Get-CompileOutputTruthfulness -CompileDir $CompileDir -EmitPrefix $EmitPrefix
  if (-not [bool]$truthfulness["truthful"]) {
    $truthfulnessFailures = @($truthfulness["failures"])
    $failureSummary = if ($truthfulnessFailures.Count -gt 0) { $truthfulnessFailures -join "; " } else { "unknown cross-check failure" }
    throw "compile output truthfulness check failed: $failureSummary"
  }

  $provenanceFileName = "$EmitPrefix.compile-provenance.json"
  $provenancePath = Join-Path $CompileDir $provenanceFileName
  $artifactFiles = @(
    Get-ChildItem -LiteralPath $CompileDir -File |
      Where-Object {
        $_.Name -ne $provenanceFileName -and
        $_.Name -ne ($EmitPrefix + ".runtime-registration-manifest.json") -and
        (
          $_.Name.Equals($EmitPrefix, [System.StringComparison]::OrdinalIgnoreCase) -or
          $_.Name.StartsWith($EmitPrefix + ".", [System.StringComparison]::OrdinalIgnoreCase) -or
          $_.Name.StartsWith($EmitPrefix + "-", [System.StringComparison]::OrdinalIgnoreCase)
        )
      } |
      Sort-Object Name
  )

  $artifactEntries = New-Object System.Collections.Generic.List[object]
  foreach ($artifact in $artifactFiles) {
    $artifactEntries.Add([ordered]@{
      path = $artifact.Name
      byte_count = [long]$artifact.Length
      sha256 = Get-FileSha256Hex -Path $artifact.FullName
    })
  }

  $digestLines = New-Object System.Collections.Generic.List[string]
  foreach ($entry in $artifactEntries) {
    $digestLines.Add(("{0}|{1}|{2}" -f [string]$entry.path, [string]$entry.byte_count, [string]$entry.sha256))
  }
  $artifactSetDigest = Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($digestLines -join "`n")))

  $artifactEntryArray = @($artifactEntries.ToArray())
  $payload = [ordered]@{}
  $payload["contract_id"] = "objc3c.native.compile.output.provenance.v1"
  $payload["provenance_artifact"] = $provenanceFileName
  $payload["manifest_artifact"] = "$EmitPrefix.manifest.json"
  $payload["registration_manifest_artifact"] = "$EmitPrefix.runtime-registration-manifest.json"
  $payload["input_source"] = $inputSourceDisplay
  $payload["input_source_sha256"] = $inputSourceHash
  $payload["compiler_binary"] = $compilerBinaryDisplay
  $payload["compiler_binary_sha256"] = $compilerBinaryHash
  $payload["runtime_support_library"] = $runtimeLibraryDisplay
  $payload["runtime_support_library_sha256"] = $runtimeLibraryHash
  $payload["compile_wrapper_script"] = $wrapperScriptDisplay
  $payload["compile_wrapper_script_sha256"] = $wrapperScriptHash
  $payload["replay_verification_model"] = "artifact-set-digest-plus-per-file-sha256-over-real-emitted-compile-outputs"
  $payload["compile_output_truthfulness"] = $truthfulness
  $payload["artifact_count"] = $artifactEntryArray.Count
  $payload["artifact_set_digest_sha256"] = $artifactSetDigest
  $payload["emitted_artifacts"] = $artifactEntryArray
  Set-Content -LiteralPath $provenancePath -Value ($payload | ConvertTo-Json -Depth 8) -Encoding utf8

  $registrationManifestPath = Join-Path $CompileDir ($EmitPrefix + ".runtime-registration-manifest.json")
  if (Test-Path -LiteralPath $registrationManifestPath -PathType Leaf) {
    $registrationManifest =
      Get-Content -LiteralPath $registrationManifestPath -Raw |
      ConvertFrom-Json -AsHashtable
    $registrationManifest["compile_output_provenance_contract_id"] = "objc3c.native.compile.output.provenance.v1"
    $registrationManifest["compile_output_provenance_artifact"] = $provenanceFileName
    $registrationManifest["compile_output_truthfulness_contract_id"] = [string]$truthfulness["contract_id"]
    $registrationManifest["compile_output_truthful"] = [bool]$truthfulness["truthful"]
    $registrationManifest["compile_output_truthfulness_runtime_dispatch_symbol"] = [string]$truthfulness["runtime_dispatch_symbol"]
    $registrationManifest["compile_output_truthfulness_property_descriptor_count"] = [int]$truthfulness["property_descriptor_definition_count"]
    $registrationManifest["compile_output_truthfulness_ivar_descriptor_count"] = [int]$truthfulness["ivar_descriptor_definition_count"]
    $registrationManifest["compile_output_artifact_count"] = $artifactEntries.Count
    $registrationManifest["compile_output_artifact_set_digest_sha256"] = $artifactSetDigest
    Set-Content -LiteralPath $registrationManifestPath -Value ($registrationManifest | ConvertTo-Json -Depth 64) -Encoding utf8
  }
}

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
