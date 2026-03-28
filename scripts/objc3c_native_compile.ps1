$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$defaultOutDir = Join-Path $repoRoot "tmp/artifacts/compilation/objc3c-native"
$runtimeLaunchContractScript = Join-Path $repoRoot "scripts/objc3c_runtime_launch_contract.ps1"
if (!(Test-Path -LiteralPath $runtimeLaunchContractScript -PathType Leaf)) {
  Write-Error "runtime launch contract helper missing at $runtimeLaunchContractScript"
  exit 2
}
. $runtimeLaunchContractScript

function Show-UsageAndExit {
  Write-Error "usage: objc3c_native_compile.ps1 <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--use-cache]"
  exit 2
}

function Parse-WrapperArguments {
  param([string[]]$RawArgs)

  if ($RawArgs.Count -lt 1) {
    Show-UsageAndExit
  }

  $useCache = $false
  $compileArgs = New-Object System.Collections.Generic.List[string]
  $outDir = $null
  $emitPrefix = "module"
  $wrapperFlagCounts = @{
    "--use-cache" = 0
    "--out-dir" = 0
  }

  for ($i = 0; $i -lt $RawArgs.Count; $i++) {
    $token = $RawArgs[$i]
    if ($token -eq "--use-cache") {
      $wrapperFlagCounts["--use-cache"] = [int]$wrapperFlagCounts["--use-cache"] + 1
      if ([int]$wrapperFlagCounts["--use-cache"] -gt 1) {
        Write-Error "--use-cache can be provided at most once"
        exit 2
      }
      $useCache = $true
      continue
    }
    if ($token.StartsWith("--use-cache=", [System.StringComparison]::OrdinalIgnoreCase)) {
      $wrapperFlagCounts["--use-cache"] = [int]$wrapperFlagCounts["--use-cache"] + 1
      if ([int]$wrapperFlagCounts["--use-cache"] -gt 1) {
        Write-Error "--use-cache can be provided at most once"
        exit 2
      }
      $rawBoolean = $token.Substring("--use-cache=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $rawBoolean) {
        $useCache = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $rawBoolean) {
        $useCache = $false
        continue
      }
      Write-Error "invalid --use-cache value '$rawBoolean' (expected true/false style token)"
      exit 2
    }

    if ($token -eq "--out-dir") {
      $wrapperFlagCounts["--out-dir"] = [int]$wrapperFlagCounts["--out-dir"] + 1
      if ([int]$wrapperFlagCounts["--out-dir"] -gt 1) {
        Write-Error "--out-dir can be provided at most once"
        exit 2
      }
      if (($i + 1) -ge $RawArgs.Count) {
        Write-Error "missing value for --out-dir"
        exit 2
      }
      $i++
      $value = $RawArgs[$i]
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --out-dir"
        exit 2
      }
      $compileArgs.Add("--out-dir")
      $compileArgs.Add($value)
      $outDir = $value
      continue
    }

    if ($token.StartsWith("--out-dir=", [System.StringComparison]::Ordinal)) {
      $wrapperFlagCounts["--out-dir"] = [int]$wrapperFlagCounts["--out-dir"] + 1
      if ([int]$wrapperFlagCounts["--out-dir"] -gt 1) {
        Write-Error "--out-dir can be provided at most once"
        exit 2
      }
      $value = $token.Substring("--out-dir=".Length)
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --out-dir"
        exit 2
      }
      $compileArgs.Add("--out-dir")
      $compileArgs.Add($value)
      $outDir = $value
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $value = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $emitPrefix = $value
      $compileArgs.Add($token)
      continue
    }

    if ($token -eq "--emit-prefix") {
      if (($i + 1) -ge $RawArgs.Count) {
        Write-Error "missing value for --emit-prefix"
        exit 2
      }
      $i++
      $value = $RawArgs[$i]
      if ([string]::IsNullOrWhiteSpace($value)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $emitPrefix = $value
      $compileArgs.Add("--emit-prefix")
      $compileArgs.Add($value)
      continue
    }

    $compileArgs.Add($token)
  }

  if ($compileArgs.Count -lt 1) {
    Show-UsageAndExit
  }

  if ([string]::IsNullOrWhiteSpace($outDir)) {
    $outDir = $defaultOutDir
    $compileArgs.Add("--out-dir")
    $compileArgs.Add($outDir)
  }

  return [pscustomobject]@{
    use_cache = $useCache
    compile_args = $compileArgs.ToArray()
    out_dir = $outDir
    emit_prefix = $emitPrefix
  }
}

function Get-ArgsWithoutOutDir {
  param([string[]]$CompileArgs)

  $result = New-Object System.Collections.Generic.List[string]
  for ($i = 0; $i -lt $CompileArgs.Count; $i++) {
    $token = $CompileArgs[$i]
    if ($token -eq "--out-dir") {
      if (($i + 1) -ge $CompileArgs.Count) {
        break
      }
      $i++
      continue
    }
    $result.Add($token)
  }
  return $result.ToArray()
}

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
    $runtimeDispatchSymbol = [string]$manifest["runtime_shim_host_link_runtime_dispatch_symbol"]
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

function Invoke-BuildNativeCompiler {
  param([string]$RepoRoot)

  $buildScript = Join-Path $RepoRoot "scripts/build_objc3c_native.ps1"
  $buildOutput = @(& $buildScript)
  $buildOutputLines = New-Object System.Collections.Generic.List[string]
  $frontendScaffoldRelativePath = $null
  $frontendInvocationLockRelativePath = $null
  $frontendCoreFeatureExpansionRelativePath = $null
  $frontendEdgeCompatRelativePath = $null
  $frontendEdgeRobustnessRelativePath = $null
  $frontendDiagnosticsHardeningRelativePath = $null
  $frontendRecoveryDeterminismHardeningRelativePath = $null
  $frontendConformanceMatrixRelativePath = $null
  $frontendConformanceCorpusRelativePath = $null
  $frontendIntegrationCloseoutRelativePath = $null
  foreach ($line in $buildOutput) {
    $lineText = [string]$line
    $buildOutputLines.Add($lineText)
    if ($lineText.StartsWith("frontend_scaffold=")) {
      $frontendScaffoldRelativePath = $lineText.Substring("frontend_scaffold=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_invocation_lock=")) {
      $frontendInvocationLockRelativePath = $lineText.Substring("frontend_invocation_lock=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_core_feature_expansion=")) {
      $frontendCoreFeatureExpansionRelativePath = $lineText.Substring("frontend_core_feature_expansion=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_edge_compat=")) {
      $frontendEdgeCompatRelativePath = $lineText.Substring("frontend_edge_compat=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_edge_robustness=")) {
      $frontendEdgeRobustnessRelativePath = $lineText.Substring("frontend_edge_robustness=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_diagnostics_hardening=")) {
      $frontendDiagnosticsHardeningRelativePath = $lineText.Substring("frontend_diagnostics_hardening=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_recovery_determinism_hardening=")) {
      $frontendRecoveryDeterminismHardeningRelativePath = $lineText.Substring("frontend_recovery_determinism_hardening=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_conformance_matrix=")) {
      $frontendConformanceMatrixRelativePath = $lineText.Substring("frontend_conformance_matrix=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_conformance_corpus=")) {
      $frontendConformanceCorpusRelativePath = $lineText.Substring("frontend_conformance_corpus=".Length).Trim()
    }
    if ($lineText.StartsWith("frontend_integration_closeout=")) {
      $frontendIntegrationCloseoutRelativePath = $lineText.Substring("frontend_integration_closeout=".Length).Trim()
    }
  }
  return [pscustomobject]@{
    exit_code = [int]$LASTEXITCODE
    build_output_lines = $buildOutputLines.ToArray()
    frontend_scaffold_relative_path = $frontendScaffoldRelativePath
    frontend_invocation_lock_relative_path = $frontendInvocationLockRelativePath
    frontend_core_feature_expansion_relative_path = $frontendCoreFeatureExpansionRelativePath
    frontend_edge_compat_relative_path = $frontendEdgeCompatRelativePath
    frontend_edge_robustness_relative_path = $frontendEdgeRobustnessRelativePath
    frontend_diagnostics_hardening_relative_path = $frontendDiagnosticsHardeningRelativePath
    frontend_recovery_determinism_hardening_relative_path = $frontendRecoveryDeterminismHardeningRelativePath
    frontend_conformance_matrix_relative_path = $frontendConformanceMatrixRelativePath
    frontend_conformance_corpus_relative_path = $frontendConformanceCorpusRelativePath
    frontend_integration_closeout_relative_path = $frontendIntegrationCloseoutRelativePath
  }
}

function Invoke-NativeCompiler {
  param(
    [string]$ExePath,
    [string[]]$Arguments
  )

  $process = Start-Process -FilePath $ExePath -ArgumentList $Arguments -NoNewWindow -Wait -PassThru
  return [int]$process.ExitCode
}

function Resolve-FrontendScaffoldPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_modular_scaffold.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_scaffold_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend modular scaffold"
}

function Resolve-FrontendInvocationLockPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_invocation_lock.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_invocation_lock_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend invocation lock"
}

function Resolve-FrontendCoreFeatureExpansionPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_core_feature_expansion_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend core feature expansion"
}

function Resolve-FrontendEdgeCompatibilityPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_edge_compat.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_edge_compat_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend edge compatibility"
}

function Resolve-FrontendEdgeRobustnessPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_edge_robustness.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_edge_robustness_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend edge robustness"
}

function Resolve-FrontendDiagnosticsHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_diagnostics_hardening_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend diagnostics hardening"
}

function Resolve-FrontendRecoveryDeterminismHardeningPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_recovery_determinism_hardening_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend recovery determinism hardening"
}

function Resolve-FrontendConformanceMatrixPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_conformance_matrix_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend conformance matrix"
}

function Resolve-FrontendConformanceCorpusPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_conformance_corpus_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend conformance corpus"
}

function Resolve-FrontendIntegrationCloseoutPath {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $defaultRelativePath = "tmp/artifacts/objc3c-native/frontend_integration_closeout.json"
  $relativePath = $defaultRelativePath
  if ($null -ne $BuildResult) {
    $candidatePath = [string]$BuildResult.frontend_integration_closeout_relative_path
    if (-not [string]::IsNullOrWhiteSpace($candidatePath)) {
      $relativePath = $candidatePath
    }
  }

  return Resolve-RepoBoundPath -RepoRoot $RepoRoot -RelativeOrAbsolutePath $relativePath -Label "frontend integration closeout"
}

function Get-NativeCompilerBuildInputPaths {
  param([Parameter(Mandatory = $true)][string]$RepoRoot)

  return @(
    (Join-Path $RepoRoot "native/objc3c/src/main.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/pipeline/objc3_frontend_types.h")
    (Join-Path $RepoRoot "native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_process.h")
    (Join-Path $RepoRoot "native/objc3c/src/io/objc3_process.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/driver/objc3_objc3_path.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/runtime/objc3_runtime.cpp")
    (Join-Path $RepoRoot "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h")
    (Join-Path $RepoRoot "scripts/build_objc3c_native.ps1")
  )
}

function Test-AnyPathNewerThanTarget {
  param(
    [Parameter(Mandatory = $true)][string]$TargetPath,
    [Parameter(Mandatory = $true)][string[]]$InputPaths
  )

  if (!(Test-Path -LiteralPath $TargetPath -PathType Leaf)) {
    return $true
  }

  $targetTimestamp = (Get-Item -LiteralPath $TargetPath).LastWriteTimeUtc
  foreach ($inputPath in $InputPaths) {
    if (!(Test-Path -LiteralPath $inputPath -PathType Leaf)) {
      continue
    }

    $inputTimestamp = (Get-Item -LiteralPath $inputPath).LastWriteTimeUtc
    if ($inputTimestamp -gt $targetTimestamp) {
      return $true
    }
  }

  return $false
}

function Test-NativeCompilerBuildArtifactsReady {
  param(
    [Parameter(Mandatory = $true)][string]$CompilerRepoRoot,
    [object]$ExistingBuildResult
  )

  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $CompilerRepoRoot
  $runtimeLibrary = Join-Path $CompilerRepoRoot "artifacts/lib/objc3_runtime.lib"
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    return $false
  }
  if (!(Test-Path -LiteralPath $runtimeLibrary -PathType Leaf)) {
    return $false
  }

  $requiredArtifacts = @(
    (Resolve-FrontendScaffoldPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendInvocationLockPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendEdgeCompatibilityPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendEdgeRobustnessPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendConformanceMatrixPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendConformanceCorpusPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
    (Resolve-FrontendIntegrationCloseoutPath -RepoRoot $CompilerRepoRoot -BuildResult $ExistingBuildResult)
  )

  foreach ($artifactPath in $requiredArtifacts) {
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      return $false
    }
  }

  $buildInputPaths = Get-NativeCompilerBuildInputPaths -RepoRoot $CompilerRepoRoot
  if (Test-AnyPathNewerThanTarget -TargetPath $exe -InputPaths $buildInputPaths) {
    return $false
  }
  if (Test-AnyPathNewerThanTarget -TargetPath $runtimeLibrary -InputPaths $buildInputPaths) {
    return $false
  }

  return $true
}

function Ensure-NativeCompilerAvailable {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot,
    [object]$BuildResult
  )

  if (Test-NativeCompilerBuildArtifactsReady -CompilerRepoRoot $RepoRoot -ExistingBuildResult $BuildResult) {
    return $BuildResult
  }

  $nextBuildResult = Invoke-BuildNativeCompiler -RepoRoot $RepoRoot
  foreach ($lineText in @($nextBuildResult.build_output_lines)) {
    Write-Host $lineText
  }
  $buildExit = [int]$nextBuildResult.exit_code
  if ($buildExit -ne 0) {
    exit $buildExit
  }

  $exe = Resolve-NativeCompilerExecutablePath -RepoRoot $RepoRoot
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    Write-Error "native compiler executable missing at $exe"
    exit 2
  }

  return $nextBuildResult
}

function Resolve-NativeCompilerExecutablePath {
  param(
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $configuredNativeExe = [string]$env:OBJC3C_NATIVE_EXECUTABLE
  if (-not [string]::IsNullOrWhiteSpace($configuredNativeExe)) {
    return [System.IO.Path]::GetFullPath($configuredNativeExe)
  }
  return (Join-Path $RepoRoot "artifacts/bin/objc3c-native.exe")
}

function Assert-FrontendModuleScaffold {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $scaffoldPath = Resolve-FrontendScaffoldPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend modular scaffold artifact missing at $scaffoldPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $scaffoldPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend modular scaffold artifact is not valid JSON at $scaffoldPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-d002-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend modular scaffold contract id mismatch in $scaffoldPath"
    exit 2
  }

  $modules = @($payload.modules)
  $requiredModules = @("driver","diagnostics-io","ir","lex-parse","frontend-api","lowering","pipeline","sema")
  $presentModules = @{}
  foreach ($module in $modules) {
    $moduleName = [string]$module.name
    if ([string]::IsNullOrWhiteSpace($moduleName)) {
      Write-Error "frontend modular scaffold has module with missing name in $scaffoldPath"
      exit 2
    }
    $moduleSources = @($module.sources)
    if ($moduleSources.Count -eq 0) {
      Write-Error "frontend modular scaffold module '$moduleName' has no sources in $scaffoldPath"
      exit 2
    }
    $presentModules[$moduleName] = $true
  }
  foreach ($moduleName in $requiredModules) {
    if (-not $presentModules.ContainsKey($moduleName)) {
      Write-Error "frontend modular scaffold missing required module '$moduleName' in $scaffoldPath"
      exit 2
    }
  }

  $sharedSources = @($payload.shared_sources)
  if ($sharedSources.Count -eq 0) {
    Write-Error "frontend modular scaffold shared_sources must be non-empty in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.module_count -ne $modules.Count) {
    Write-Error "frontend modular scaffold module_count mismatch in $scaffoldPath"
    exit 2
  }
  if ([int]$payload.shared_source_count -ne $sharedSources.Count) {
    Write-Error "frontend modular scaffold shared_source_count mismatch in $scaffoldPath"
    exit 2
  }
}

function Assert-FrontendInvocationLock {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $lockPath = Resolve-FrontendInvocationLockPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $lockPath -PathType Leaf)) {
    Write-Error "frontend invocation lock artifact missing at $lockPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $lockPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend invocation lock artifact is not valid JSON at $lockPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-manifest-guard/parser_build-d003-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend invocation lock contract id mismatch in $lockPath"
    exit 2
  }

  $expectedScaffoldContractId = "objc3c-frontend-build-invocation-modular-scaffold/parser_build-d002-v1"
  if ([string]$payload.scaffold_contract_id -ne $expectedScaffoldContractId) {
    Write-Error "frontend invocation lock scaffold contract id mismatch in $lockPath"
    exit 2
  }

  $scaffold = $payload.scaffold
  if ($null -eq $scaffold) {
    Write-Error "frontend invocation lock scaffold metadata missing in $lockPath"
    exit 2
  }

  $scaffoldRelativePath = [string]$scaffold.path
  $scaffoldExpectedHash = [string]$scaffold.sha256
  if ([string]::IsNullOrWhiteSpace($scaffoldRelativePath) -or [string]::IsNullOrWhiteSpace($scaffoldExpectedHash)) {
    Write-Error "frontend invocation lock scaffold metadata invalid in $lockPath"
    exit 2
  }

  $scaffoldPath = $scaffoldRelativePath
  if (-not [System.IO.Path]::IsPathRooted($scaffoldPath)) {
    $scaffoldPath = Join-Path $RepoRoot $scaffoldPath
  }
  if (!(Test-Path -LiteralPath $scaffoldPath -PathType Leaf)) {
    Write-Error "frontend invocation lock scaffold path missing at $scaffoldPath"
    exit 2
  }
  $scaffoldActualHash = Get-FileSha256Hex -Path $scaffoldPath
  if ($scaffoldActualHash -ne $scaffoldExpectedHash.ToLowerInvariant()) {
    Write-Error "frontend invocation lock scaffold sha256 mismatch in $lockPath"
    exit 2
  }

  $binaries = @($payload.binaries)
  if ($binaries.Count -lt 2) {
    Write-Error "frontend invocation lock binaries list must include native and c-api runner entries in $lockPath"
    exit 2
  }

  $expectedBinaries = [ordered]@{
    "objc3c-native" = "artifacts/bin/objc3c-native.exe"
    "objc3c-frontend-c-api-runner" = "artifacts/bin/objc3c-frontend-c-api-runner.exe"
  }
  $binaryIndex = @{}
  foreach ($binary in $binaries) {
    $binaryName = [string]$binary.name
    $binaryPath = [string]$binary.path
    $binaryHash = [string]$binary.sha256
    if ([string]::IsNullOrWhiteSpace($binaryName) -or
        [string]::IsNullOrWhiteSpace($binaryPath) -or
        [string]::IsNullOrWhiteSpace($binaryHash)) {
      Write-Error "frontend invocation lock binary entry is invalid in $lockPath"
      exit 2
    }
    if ($binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock contains duplicate binary entry '$binaryName' in $lockPath"
      exit 2
    }
    $binaryIndex[$binaryName] = $binary
  }

  foreach ($binaryName in $expectedBinaries.Keys) {
    if (-not $binaryIndex.ContainsKey($binaryName)) {
      Write-Error "frontend invocation lock missing binary '$binaryName' in $lockPath"
      exit 2
    }
    $binary = $binaryIndex[$binaryName]
    $expectedRelativePath = [string]$expectedBinaries[$binaryName]
    $manifestRelativePath = ([string]$binary.path).Replace('\', '/')
    if ($manifestRelativePath -ne $expectedRelativePath) {
      Write-Error "frontend invocation lock binary path mismatch for '$binaryName' in $lockPath"
      exit 2
    }
    $binaryPath = Join-Path $RepoRoot $expectedRelativePath
    if (!(Test-Path -LiteralPath $binaryPath -PathType Leaf)) {
      Write-Error "frontend invocation lock binary path missing at $binaryPath"
      exit 2
    }
    $binaryActualHash = Get-FileSha256Hex -Path $binaryPath
    if ($binaryActualHash -ne ([string]$binary.sha256).ToLowerInvariant()) {
      Write-Error "frontend invocation lock binary sha256 mismatch for '$binaryName' in $lockPath"
      exit 2
    }
  }
}

function Assert-FrontendCoreFeatureExpansion {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs
  )

  $featurePath = Resolve-FrontendCoreFeatureExpansionPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $featurePath -PathType Leaf)) {
    Write-Error "frontend core feature expansion artifact missing at $featurePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $featurePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend core feature expansion artifact is not valid JSON at $featurePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-d004-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend core feature expansion contract id mismatch in $featurePath"
    exit 2
  }

  $expectedDependencyContracts = @(
    "objc3c-frontend-build-invocation-modular-scaffold/parser_build-d002-v1",
    "objc3c-frontend-build-invocation-manifest-guard/parser_build-d003-v1"
  )
  $presentDependencyContracts = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (![string]::IsNullOrWhiteSpace($contractIdText)) {
      $presentDependencyContracts[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencyContracts) {
    if (-not $presentDependencyContracts.ContainsKey($requiredContractId)) {
      Write-Error "frontend core feature expansion missing dependency contract '$requiredContractId' in $featurePath"
      exit 2
    }
  }

  $requiredModules = @("driver", "diagnostics-io", "ir", "lex-parse", "frontend-api", "lowering", "pipeline", "sema")
  $presentModules = @{}
  foreach ($moduleName in @($payload.module_names)) {
    $moduleText = [string]$moduleName
    if (![string]::IsNullOrWhiteSpace($moduleText)) {
      $presentModules[$moduleText] = $true
    }
  }
  foreach ($requiredModule in $requiredModules) {
    if (-not $presentModules.ContainsKey($requiredModule)) {
      Write-Error "frontend core feature expansion missing required module '$requiredModule' in $featurePath"
      exit 2
    }
  }

  $invocation = $payload.invocation
  if ($null -eq $invocation) {
    Write-Error "frontend core feature expansion invocation metadata missing in $featurePath"
    exit 2
  }
  if ([string]$invocation.default_out_dir -ne "tmp/artifacts/compilation/objc3c-native") {
    Write-Error "frontend core feature expansion default_out_dir mismatch in $featurePath"
    exit 2
  }
  if ([string]$invocation.cache_root -ne "tmp/artifacts/objc3c-native/cache") {
    Write-Error "frontend core feature expansion cache_root mismatch in $featurePath"
    exit 2
  }
  if (-not [bool]$invocation.supports_cache) {
    Write-Error "frontend core feature expansion supports_cache must be true in $featurePath"
    exit 2
  }

  $backendRouting = $payload.backend_routing
  if ($null -eq $backendRouting) {
    Write-Error "frontend core feature expansion backend_routing metadata missing in $featurePath"
    exit 2
  }
  if (-not [bool]$backendRouting.supports_capability_routing) {
    Write-Error "frontend core feature expansion supports_capability_routing must be true in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend core feature expansion capability_summary_flag mismatch in $featurePath"
    exit 2
  }
  if ([string]$backendRouting.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend core feature expansion route_flag mismatch in $featurePath"
    exit 2
  }

  $allowedBackends = @{}
  foreach ($backend in @($backendRouting.allowed_ir_object_backends)) {
    $backendText = ([string]$backend).Trim()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $allowedBackends[$backendText.ToLowerInvariant()] = $backendText
    }
  }
  foreach ($requiredBackend in @("clang", "llvm-direct")) {
    if (-not $allowedBackends.ContainsKey($requiredBackend)) {
      Write-Error "frontend core feature expansion missing backend '$requiredBackend' in $featurePath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $requestedBackend = $null
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]
    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $requestedBackend = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $requestedBackend = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($requestedBackend)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      continue
    }
    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $usesCapabilityRouting = $true
      continue
    }
    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        $usesCapabilityRouting = $false
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $hasCapabilitySummary = $true
      continue
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($requestedBackend)) {
    $normalizedRequestedBackend = $requestedBackend.Trim().ToLowerInvariant().Replace("_", "-")
    if (-not $allowedBackends.ContainsKey($normalizedRequestedBackend)) {
      Write-Error "requested --objc3-ir-object-backend '$requestedBackend' is not allowed by frontend core feature expansion in $featurePath"
      exit 2
    }
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    feature_path = $featurePath
    allowed_ir_object_backends = @($allowedBackends.Keys)
  }
}

function Assert-FrontendEdgeCompatibility {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [object]$CoreFeatureGuard
  )

  $compatPath = Resolve-FrontendEdgeCompatibilityPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $compatPath -PathType Leaf)) {
    Write-Error "frontend edge compatibility artifact missing at $compatPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $compatPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge compatibility artifact is not valid JSON at $compatPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-d005-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge compatibility contract id mismatch in $compatPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-d004-v1",
    "objc3c-frontend-build-invocation-manifest-guard/parser_build-d003-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge compatibility missing dependency contract '$requiredContractId' in $compatPath"
      exit 2
    }
  }

  $edgeCompat = $payload.invocation_edge_compat
  if ($null -eq $edgeCompat) {
    Write-Error "frontend edge compatibility invocation_edge_compat metadata missing in $compatPath"
    exit 2
  }
  if ([int]$edgeCompat.fail_closed_exit_code -ne 2) {
    Write-Error "frontend edge compatibility fail_closed_exit_code must be 2 in $compatPath"
    exit 2
  }
  if (-not [bool]$edgeCompat.disallow_relative_parent_segments) {
    Write-Error "frontend edge compatibility disallow_relative_parent_segments must be true in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.route_flag -ne "--objc3-route-backend-from-capabilities") {
    Write-Error "frontend edge compatibility route_flag mismatch in $compatPath"
    exit 2
  }
  if ([string]$edgeCompat.capability_summary_flag -ne "--llvm-capabilities-summary") {
    Write-Error "frontend edge compatibility capability_summary_flag mismatch in $compatPath"
    exit 2
  }

  $backendCompat = $payload.backend_compat
  if ($null -eq $backendCompat) {
    Write-Error "frontend edge compatibility backend_compat metadata missing in $compatPath"
    exit 2
  }

  $canonicalBackends = @{}
  foreach ($backend in @($backendCompat.canonical_allowed_backends)) {
    $backendText = ([string]$backend).Trim().ToLowerInvariant()
    if (-not [string]::IsNullOrWhiteSpace($backendText)) {
      $canonicalBackends[$backendText] = $true
    }
  }
  if ($canonicalBackends.Count -eq 0) {
    Write-Error "frontend edge compatibility canonical_allowed_backends must be non-empty in $compatPath"
    exit 2
  }
  if ($null -ne $CoreFeatureGuard) {
    foreach ($coreBackend in @($CoreFeatureGuard.allowed_ir_object_backends)) {
      $coreBackendText = ([string]$coreBackend).Trim().ToLowerInvariant()
      if (-not [string]::IsNullOrWhiteSpace($coreBackendText) -and
          -not $canonicalBackends.ContainsKey($coreBackendText)) {
        Write-Error "frontend edge compatibility missing backend '$coreBackendText' declared by frontend core feature expansion"
        exit 2
      }
    }
  }

  $aliasMap = @{}
  $aliasPayload = $backendCompat.alias_to_canonical
  if ($null -eq $aliasPayload) {
    Write-Error "frontend edge compatibility alias_to_canonical mapping missing in $compatPath"
    exit 2
  }
  foreach ($property in $aliasPayload.PSObject.Properties) {
    $alias = ([string]$property.Name).Trim().ToLowerInvariant().Replace("_", "-")
    $canonical = ([string]$property.Value).Trim().ToLowerInvariant().Replace("_", "-")
    if ([string]::IsNullOrWhiteSpace($alias) -or [string]::IsNullOrWhiteSpace($canonical)) {
      Write-Error "frontend edge compatibility alias_to_canonical entries must be non-empty in $compatPath"
      exit 2
    }
    if (-not $canonicalBackends.ContainsKey($canonical)) {
      Write-Error "frontend edge compatibility alias '$alias' maps to unknown canonical backend '$canonical' in $compatPath"
      exit 2
    }
    $aliasMap[$alias] = $canonical
  }
  foreach ($canonicalBackend in $canonicalBackends.Keys) {
    if (-not $aliasMap.ContainsKey($canonicalBackend)) {
      $aliasMap[$canonicalBackend] = $canonicalBackend
    }
  }

  $singleValueFlags = @{}
  foreach ($flag in @($backendCompat.single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $singleValueFlags[$flagText] = 0
    }
  }
  foreach ($requiredSingleValueFlag in @("--objc3-ir-object-backend", "--llvm-capabilities-summary")) {
    if (-not $singleValueFlags.ContainsKey($requiredSingleValueFlag)) {
      Write-Error "frontend edge compatibility missing single-value flag '$requiredSingleValueFlag' in $compatPath"
      exit 2
    }
  }

  $compileArgs = @()
  if ($null -ne $ParsedArgs) {
    $compileArgs = @($ParsedArgs.compile_args)
  }
  $normalizedArgs = New-Object System.Collections.Generic.List[string]
  $usesCapabilityRouting = $false
  $hasCapabilitySummary = $false
  $routeFlagOccurrences = 0

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (-not $aliasMap.ContainsKey($backendKey)) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $singleValueFlags["--objc3-ir-object-backend"] = [int]$singleValueFlags["--objc3-ir-object-backend"] + 1
      $normalizedArgs.Add("--objc3-ir-object-backend")
      $normalizedArgs.Add([string]$aliasMap[$backendKey])
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryPath = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryPath)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      if (-not [System.IO.Path]::IsPathRooted($summaryPath) -and $summaryPath.Replace('\', '/').Split('/') -contains "..") {
        Write-Error "--llvm-capabilities-summary must not contain '..' relative segments"
        exit 2
      }
      $singleValueFlags["--llvm-capabilities-summary"] = [int]$singleValueFlags["--llvm-capabilities-summary"] + 1
      $hasCapabilitySummary = $true
      $normalizedArgs.Add("--llvm-capabilities-summary")
      $normalizedArgs.Add($summaryPath)
      continue
    }

    if ($token.StartsWith("--emit-prefix=", [System.StringComparison]::Ordinal)) {
      $emitPrefix = $token.Substring("--emit-prefix=".Length)
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--emit-prefix") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --emit-prefix"
        exit 2
      }
      $i++
      $emitPrefix = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($emitPrefix)) {
        Write-Error "empty value for --emit-prefix"
        exit 2
      }
      $normalizedArgs.Add("--emit-prefix")
      $normalizedArgs.Add($emitPrefix)
      continue
    }

    if ($token.StartsWith("--clang=", [System.StringComparison]::Ordinal)) {
      $clangPath = $token.Substring("--clang=".Length)
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
      }
      $normalizedArgs.Add($token)
      continue
    }

    if ($token -eq "--clang") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --clang"
        exit 2
      }
      $i++
      $clangPath = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($clangPath)) {
        Write-Error "empty value for --clang"
        exit 2
      }
      $normalizedArgs.Add("--clang")
      $normalizedArgs.Add($clangPath)
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeFlagOccurrences++
      $usesCapabilityRouting = $true
      $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeFlagOccurrences++
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $usesCapabilityRouting = $true
        $normalizedArgs.Add("--objc3-route-backend-from-capabilities")
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }

    $normalizedArgs.Add($token)
  }

  foreach ($flag in $singleValueFlags.Keys) {
    if ([int]$singleValueFlags[$flag] -gt 1) {
      Write-Error "$flag can be provided at most once"
      exit 2
    }
  }
  if ($routeFlagOccurrences -gt 1) {
    Write-Error "--objc3-route-backend-from-capabilities can be provided at most once"
    exit 2
  }
  if ($usesCapabilityRouting -and -not $hasCapabilitySummary) {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  return [pscustomobject]@{
    edge_compat_path = $compatPath
    normalized_compile_args = $normalizedArgs.ToArray()
  }
}

function Assert-FrontendEdgeRobustness {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $robustnessPath = Resolve-FrontendEdgeRobustnessPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $robustnessPath -PathType Leaf)) {
    Write-Error "frontend edge robustness artifact missing at $robustnessPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $robustnessPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend edge robustness artifact is not valid JSON at $robustnessPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-edge-robustness/parser_build-d006-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend edge robustness contract id mismatch in $robustnessPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-d005-v1",
    "objc3c-frontend-build-invocation-core-feature-expansion/parser_build-d004-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend edge robustness missing dependency contract '$requiredContractId' in $robustnessPath"
      exit 2
    }
  }

  $guardrails = $payload.wrapper_guardrails
  if ($null -eq $guardrails) {
    Write-Error "frontend edge robustness wrapper_guardrails metadata missing in $robustnessPath"
    exit 2
  }

  $requiredWrapperSingleFlags = @("--use-cache", "--out-dir")
  $wrapperSingleSet = @{}
  foreach ($flag in @($guardrails.wrapper_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $wrapperSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredWrapperSingleFlags) {
    if (-not $wrapperSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing wrapper_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredCompileSingleFlags = @(
    "--objc3-ir-object-backend",
    "--llvm-capabilities-summary",
    "--objc3-route-backend-from-capabilities"
  )
  $compileSingleSet = @{}
  foreach ($flag in @($guardrails.compile_single_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $compileSingleSet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredCompileSingleFlags) {
    if (-not $compileSingleSet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing compile_single_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  $requiredRejectEmptyFlags = @("--emit-prefix", "--clang", "--use-cache")
  $rejectEmptySet = @{}
  foreach ($flag in @($guardrails.reject_empty_equals_value_flags)) {
    $flagText = [string]$flag
    if (-not [string]::IsNullOrWhiteSpace($flagText)) {
      $rejectEmptySet[$flagText] = $true
    }
  }
  foreach ($requiredFlag in $requiredRejectEmptyFlags) {
    if (-not $rejectEmptySet.ContainsKey($requiredFlag)) {
      Write-Error "frontend edge robustness missing reject_empty_equals_value flag '$requiredFlag' in $robustnessPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    edge_robustness_path = $robustnessPath
  }
}

function Assert-FrontendDiagnosticsHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $diagnosticsPath = Resolve-FrontendDiagnosticsHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $diagnosticsPath -PathType Leaf)) {
    Write-Error "frontend diagnostics hardening artifact missing at $diagnosticsPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $diagnosticsPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend diagnostics hardening artifact is not valid JSON at $diagnosticsPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-d007-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend diagnostics hardening contract id mismatch in $diagnosticsPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-edge-robustness/parser_build-d006-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-d005-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend diagnostics hardening missing dependency contract '$requiredContractId' in $diagnosticsPath"
      exit 2
    }
  }

  $wrapperDiagnostics = $payload.wrapper_diagnostics
  if ($null -eq $wrapperDiagnostics) {
    Write-Error "frontend diagnostics hardening wrapper_diagnostics metadata missing in $diagnosticsPath"
    exit 2
  }
  if ([int]$wrapperDiagnostics.fail_closed_exit_code -ne 2) {
    Write-Error "frontend diagnostics hardening fail_closed_exit_code must be 2 in $diagnosticsPath"
    exit 2
  }

  $requiredMessages = @(
    "--use-cache can be provided at most once",
    "invalid --use-cache value",
    "--out-dir can be provided at most once",
    "missing value for --out-dir",
    "empty value for --out-dir",
    "missing value for --emit-prefix",
    "empty value for --emit-prefix",
    "missing value for --clang",
    "empty value for --clang"
  )
  $messageSet = @{}
  foreach ($message in @($wrapperDiagnostics.required_error_messages)) {
    $messageText = [string]$message
    if (-not [string]::IsNullOrWhiteSpace($messageText)) {
      $messageSet[$messageText] = $true
    }
  }
  foreach ($requiredMessage in $requiredMessages) {
    if (-not $messageSet.ContainsKey($requiredMessage)) {
      Write-Error "frontend diagnostics hardening missing required_error_messages entry '$requiredMessage' in $diagnosticsPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    diagnostics_hardening_path = $diagnosticsPath
  }
}

function Assert-FrontendRecoveryDeterminismHardening {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $recoveryPath = Resolve-FrontendRecoveryDeterminismHardeningPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $recoveryPath -PathType Leaf)) {
    Write-Error "frontend recovery determinism hardening artifact missing at $recoveryPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $recoveryPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend recovery determinism hardening artifact is not valid JSON at $recoveryPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-d008-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend recovery determinism hardening contract id mismatch in $recoveryPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-diagnostics-hardening/parser_build-d007-v1",
    "objc3c-frontend-build-invocation-edge-robustness/parser_build-d006-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend recovery determinism hardening missing dependency contract '$requiredContractId' in $recoveryPath"
      exit 2
    }
  }

  $cacheDeterminism = $payload.cache_determinism
  if ($null -eq $cacheDeterminism) {
    Write-Error "frontend recovery determinism hardening cache_determinism metadata missing in $recoveryPath"
    exit 2
  }
  if ([int]$cacheDeterminism.fail_closed_exit_code -ne 2) {
    Write-Error "frontend recovery determinism hardening fail_closed_exit_code must be 2 in $recoveryPath"
    exit 2
  }
  if ([string]$cacheDeterminism.entry_contract_id -ne "objc3c-native-cache-entry/parser_build-d008-v1") {
    Write-Error "frontend recovery determinism hardening entry_contract_id mismatch in $recoveryPath"
    exit 2
  }

  $requiredStatusTokens = @("cache_hit=true", "cache_hit=false")
  $statusTokenSet = @{}
  foreach ($token in @($cacheDeterminism.cache_status_tokens)) {
    $tokenText = [string]$token
    if (-not [string]::IsNullOrWhiteSpace($tokenText)) {
      $statusTokenSet[$tokenText] = $true
    }
  }
  foreach ($requiredToken in $requiredStatusTokens) {
    if (-not $statusTokenSet.ContainsKey($requiredToken)) {
      Write-Error "frontend recovery determinism hardening missing cache_status_tokens entry '$requiredToken' in $recoveryPath"
      exit 2
    }
  }

  $requiredEntryFiles = @("files", "exit_code.txt", "ready.marker", "metadata.json")
  $entryFileSet = @{}
  foreach ($entryFile in @($cacheDeterminism.required_entry_files)) {
    $entryFileText = [string]$entryFile
    if (-not [string]::IsNullOrWhiteSpace($entryFileText)) {
      $entryFileSet[$entryFileText] = $true
    }
  }
  foreach ($requiredEntryFile in $requiredEntryFiles) {
    if (-not $entryFileSet.ContainsKey($requiredEntryFile)) {
      Write-Error "frontend recovery determinism hardening missing required_entry_files entry '$requiredEntryFile' in $recoveryPath"
      exit 2
    }
  }

  $requiredRecoverySignals = @(
    "cache_recovery=metadata_missing",
    "cache_recovery=metadata_invalid",
    "cache_recovery=metadata_contract_mismatch",
    "cache_recovery=metadata_cache_key_mismatch",
    "cache_recovery=metadata_exit_code_mismatch",
    "cache_recovery=metadata_digest_mismatch",
    "cache_recovery=restore_failed"
  )
  $recoverySignalSet = @{}
  foreach ($signal in @($cacheDeterminism.recovery_signals)) {
    $signalText = [string]$signal
    if (-not [string]::IsNullOrWhiteSpace($signalText)) {
      $recoverySignalSet[$signalText] = $true
    }
  }
  foreach ($requiredSignal in $requiredRecoverySignals) {
    if (-not $recoverySignalSet.ContainsKey($requiredSignal)) {
      Write-Error "frontend recovery determinism hardening missing recovery_signals entry '$requiredSignal' in $recoveryPath"
      exit 2
    }
  }

  return [pscustomobject]@{
    recovery_determinism_hardening_path = $recoveryPath
  }
}

function Assert-FrontendConformanceMatrix {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [object]$ParsedArgs,
    [string[]]$EffectiveCompileArgs
  )

  $conformancePath = Resolve-FrontendConformanceMatrixPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $conformancePath -PathType Leaf)) {
    Write-Error "frontend conformance matrix artifact missing at $conformancePath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $conformancePath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend conformance matrix artifact is not valid JSON at $conformancePath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-conformance-matrix/parser_build-d009-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend conformance matrix contract id mismatch in $conformancePath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-d008-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-d005-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend conformance matrix missing dependency contract '$requiredContractId' in $conformancePath"
      exit 2
    }
  }

  if ([int]$payload.acceptance_profile_count -le 0) {
    Write-Error "frontend conformance matrix acceptance_profile_count must be positive in $conformancePath"
    exit 2
  }
  if ([int]$payload.rejection_profile_count -le 0) {
    Write-Error "frontend conformance matrix rejection_profile_count must be positive in $conformancePath"
    exit 2
  }

  $acceptanceRows = @($payload.acceptance_matrix)
  $acceptanceProfileSet = @{}
  foreach ($row in $acceptanceRows) {
    $caseId = [string]$row.case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Write-Error "frontend conformance matrix acceptance rows must define case_id and profile_key in $conformancePath"
      exit 2
    }
    if ($expectedResult -ne "accept") {
      Write-Error "frontend conformance matrix acceptance row '$caseId' must declare expected_result='accept' in $conformancePath"
      exit 2
    }
    if ($acceptanceProfileSet.ContainsKey($profileKey)) {
      Write-Error "frontend conformance matrix duplicate acceptance profile '$profileKey' in $conformancePath"
      exit 2
    }
    $acceptanceProfileSet[$profileKey] = $caseId
  }
  if ([int]$payload.acceptance_profile_count -ne $acceptanceRows.Count) {
    Write-Error "frontend conformance matrix acceptance_profile_count mismatch in $conformancePath"
    exit 2
  }

  $expectedProfileSet = @{}
  foreach ($cacheMode in @("no-cache", "cache-aware")) {
    foreach ($backendMode in @("default", "clang", "llvm-direct")) {
      foreach ($summaryMode in @("none", "present")) {
        $profileKey = "{0}|{1}|manual|{2}" -f $cacheMode, $backendMode, $summaryMode
        $expectedProfileSet[$profileKey] = $true
      }
      $profileKey = "{0}|{1}|capability-route|present" -f $cacheMode, $backendMode
      $expectedProfileSet[$profileKey] = $true
    }
  }
  foreach ($expectedProfile in $expectedProfileSet.Keys) {
    if (-not $acceptanceProfileSet.ContainsKey($expectedProfile)) {
      Write-Error "frontend conformance matrix missing acceptance profile '$expectedProfile' in $conformancePath"
      exit 2
    }
  }

  $requiredRejectDiagnostics = @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
  $rejectRows = @($payload.rejection_matrix)
  if ([int]$payload.rejection_profile_count -ne $rejectRows.Count) {
    Write-Error "frontend conformance matrix rejection_profile_count mismatch in $conformancePath"
    exit 2
  }
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectRows) {
    $caseId = [string]$row.case_id
    $expectedResult = [string]$row.expected_result
    $requiredDiagnostic = [string]$row.required_diagnostic
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($requiredDiagnostic)) {
      Write-Error "frontend conformance matrix rejection rows must define case_id and required_diagnostic in $conformancePath"
      exit 2
    }
    if ($expectedResult -ne "reject") {
      Write-Error "frontend conformance matrix rejection row '$caseId' must declare expected_result='reject' in $conformancePath"
      exit 2
    }
    $rejectDiagnosticSet[$requiredDiagnostic] = $true
  }
  foreach ($requiredDiagnostic in $requiredRejectDiagnostics) {
    if (-not $rejectDiagnosticSet.ContainsKey($requiredDiagnostic)) {
      Write-Error "frontend conformance matrix missing rejection diagnostic '$requiredDiagnostic' in $conformancePath"
      exit 2
    }
  }

  $compileArgs = @($EffectiveCompileArgs)
  $cacheMode = if ($null -ne $ParsedArgs -and [bool]$ParsedArgs.use_cache) { "cache-aware" } else { "no-cache" }
  $backendMode = "default"
  $routingMode = "manual"
  $summaryMode = "none"
  $routeEnabled = $false

  for ($i = 0; $i -lt $compileArgs.Count; $i++) {
    $token = [string]$compileArgs[$i]

    if ($token -eq "--objc3-ir-object-backend") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --objc3-ir-object-backend"
        exit 2
      }
      $i++
      $backendValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (@("clang", "llvm-direct") -notcontains $backendKey) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $backendMode = $backendKey
      continue
    }

    if ($token.StartsWith("--objc3-ir-object-backend=", [System.StringComparison]::Ordinal)) {
      $backendValue = $token.Substring("--objc3-ir-object-backend=".Length)
      if ([string]::IsNullOrWhiteSpace($backendValue)) {
        Write-Error "empty value for --objc3-ir-object-backend"
        exit 2
      }
      $backendKey = $backendValue.Trim().ToLowerInvariant().Replace("_", "-")
      if (@("clang", "llvm-direct") -notcontains $backendKey) {
        Write-Error "unsupported value '$backendValue' for --objc3-ir-object-backend"
        exit 2
      }
      $backendMode = $backendKey
      continue
    }

    if ($token -eq "--llvm-capabilities-summary") {
      if (($i + 1) -ge $compileArgs.Count) {
        Write-Error "missing value for --llvm-capabilities-summary"
        exit 2
      }
      $i++
      $summaryValue = [string]$compileArgs[$i]
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $summaryMode = "present"
      continue
    }

    if ($token.StartsWith("--llvm-capabilities-summary=", [System.StringComparison]::Ordinal)) {
      $summaryValue = $token.Substring("--llvm-capabilities-summary=".Length)
      if ([string]::IsNullOrWhiteSpace($summaryValue)) {
        Write-Error "empty value for --llvm-capabilities-summary"
        exit 2
      }
      $summaryMode = "present"
      continue
    }

    if ($token -eq "--objc3-route-backend-from-capabilities") {
      $routeEnabled = $true
      continue
    }

    if ($token.StartsWith("--objc3-route-backend-from-capabilities=", [System.StringComparison]::Ordinal)) {
      $routeBoolean = $token.Substring("--objc3-route-backend-from-capabilities=".Length).Trim().ToLowerInvariant()
      if (@("1", "true", "yes", "on") -contains $routeBoolean) {
        $routeEnabled = $true
        continue
      }
      if (@("0", "false", "no", "off") -contains $routeBoolean) {
        continue
      }
      Write-Error "invalid boolean value '$routeBoolean' for --objc3-route-backend-from-capabilities"
      exit 2
    }
  }

  if ($routeEnabled) {
    $routingMode = "capability-route"
  }
  if ($routingMode -eq "capability-route" -and $summaryMode -ne "present") {
    Write-Error "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary"
    exit 2
  }

  $invocationProfileKey = "{0}|{1}|{2}|{3}" -f $cacheMode, $backendMode, $routingMode, $summaryMode
  if (-not $acceptanceProfileSet.ContainsKey($invocationProfileKey)) {
    Write-Error "frontend conformance matrix has no acceptance row for invocation profile '$invocationProfileKey' in $conformancePath"
    exit 2
  }

  return [pscustomobject]@{
    conformance_matrix_path = $conformancePath
    profile_key = $invocationProfileKey
    case_id = [string]$acceptanceProfileSet[$invocationProfileKey]
  }
}

function Assert-FrontendConformanceCorpus {
  param(
    [string]$RepoRoot,
    [object]$BuildResult,
    [string]$InvocationProfileKey
  )

  $corpusPath = Resolve-FrontendConformanceCorpusPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $corpusPath -PathType Leaf)) {
    Write-Error "frontend conformance corpus artifact missing at $corpusPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $corpusPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend conformance corpus artifact is not valid JSON at $corpusPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-conformance-corpus/parser_build-d010-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend conformance corpus contract id mismatch in $corpusPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-conformance-matrix/parser_build-d009-v1",
    "objc3c-frontend-build-invocation-edge-compat-completion/parser_build-d005-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend conformance corpus missing dependency contract '$requiredContractId' in $corpusPath"
      exit 2
    }
  }

  $acceptanceRows = @($payload.acceptance_corpus)
  $rejectionRows = @($payload.rejection_corpus)
  if ([int]$payload.acceptance_corpus_count -ne $acceptanceRows.Count) {
    Write-Error "frontend conformance corpus acceptance_corpus_count mismatch in $corpusPath"
    exit 2
  }
  if ([int]$payload.rejection_corpus_count -ne $rejectionRows.Count) {
    Write-Error "frontend conformance corpus rejection_corpus_count mismatch in $corpusPath"
    exit 2
  }
  if ([int]$payload.corpus_case_count -ne ($acceptanceRows.Count + $rejectionRows.Count)) {
    Write-Error "frontend conformance corpus corpus_case_count mismatch in $corpusPath"
    exit 2
  }
  if ($acceptanceRows.Count -le 0 -or $rejectionRows.Count -le 0) {
    Write-Error "frontend conformance corpus requires non-empty acceptance and rejection corpus in $corpusPath"
    exit 2
  }

  $acceptanceByProfile = @{}
  foreach ($row in $acceptanceRows) {
    $caseId = [string]$row.corpus_case_id
    $profileKey = [string]$row.profile_key
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($profileKey)) {
      Write-Error "frontend conformance corpus acceptance rows must define corpus_case_id and profile_key in $corpusPath"
      exit 2
    }
    if ($expectedResult -ne "accept") {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must declare expected_result='accept' in $corpusPath"
      exit 2
    }
    if ($expectedExitCode -ne 0) {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must declare expected_exit_code=0 in $corpusPath"
      exit 2
    }
    if ($compileArgs.Count -le 0) {
      Write-Error "frontend conformance corpus acceptance row '$caseId' must provide compile_args in $corpusPath"
      exit 2
    }
    if (-not $acceptanceByProfile.ContainsKey($profileKey)) {
      $acceptanceByProfile[$profileKey] = New-Object System.Collections.Generic.List[string]
    }
    $acceptanceByProfile[$profileKey].Add($caseId)
  }

  $requiredRejectDiagnostics = @(
    "--objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
    "unsupported value '<backend>' for --objc3-ir-object-backend",
    "--objc3-ir-object-backend can be provided at most once",
    "--llvm-capabilities-summary must not contain '..' relative segments",
    "--objc3-route-backend-from-capabilities can be provided at most once"
  )
  $rejectDiagnosticSet = @{}
  foreach ($row in $rejectionRows) {
    $caseId = [string]$row.corpus_case_id
    $expectedResult = [string]$row.expected_result
    $expectedExitCode = [int]$row.expected_exit_code
    $expectedDiagnostic = [string]$row.expected_diagnostic
    $compileArgs = @($row.compile_args)
    if ([string]::IsNullOrWhiteSpace($caseId) -or [string]::IsNullOrWhiteSpace($expectedDiagnostic)) {
      Write-Error "frontend conformance corpus rejection rows must define corpus_case_id and expected_diagnostic in $corpusPath"
      exit 2
    }
    if ($expectedResult -ne "reject") {
      Write-Error "frontend conformance corpus rejection row '$caseId' must declare expected_result='reject' in $corpusPath"
      exit 2
    }
    if ($expectedExitCode -ne 2) {
      Write-Error "frontend conformance corpus rejection row '$caseId' must declare expected_exit_code=2 in $corpusPath"
      exit 2
    }
    if ($compileArgs.Count -le 0) {
      Write-Error "frontend conformance corpus rejection row '$caseId' must provide compile_args in $corpusPath"
      exit 2
    }
    $rejectDiagnosticSet[$expectedDiagnostic] = $true
  }
  foreach ($requiredDiagnostic in $requiredRejectDiagnostics) {
    if (-not $rejectDiagnosticSet.ContainsKey($requiredDiagnostic)) {
      Write-Error "frontend conformance corpus missing rejection diagnostic '$requiredDiagnostic' in $corpusPath"
      exit 2
    }
  }

  if ([string]::IsNullOrWhiteSpace($InvocationProfileKey)) {
    Write-Error "frontend conformance corpus invocation profile key is required"
    exit 2
  }
  if (-not $acceptanceByProfile.ContainsKey($InvocationProfileKey)) {
    Write-Error "frontend conformance corpus has no acceptance case for invocation profile '$InvocationProfileKey' in $corpusPath"
    exit 2
  }

  return [pscustomobject]@{
    conformance_corpus_path = $corpusPath
    profile_key = $InvocationProfileKey
    acceptance_case_count = [int]$acceptanceByProfile[$InvocationProfileKey].Count
  }
}

function Assert-FrontendIntegrationCloseout {
  param(
    [string]$RepoRoot,
    [object]$BuildResult
  )

  $closeoutPath = Resolve-FrontendIntegrationCloseoutPath -RepoRoot $RepoRoot -BuildResult $BuildResult
  if (!(Test-Path -LiteralPath $closeoutPath -PathType Leaf)) {
    Write-Error "frontend integration closeout artifact missing at $closeoutPath"
    exit 2
  }

  try {
    $payload = Get-Content -LiteralPath $closeoutPath -Raw | ConvertFrom-Json
  } catch {
    Write-Error "frontend integration closeout artifact is not valid JSON at $closeoutPath"
    exit 2
  }

  $expectedContractId = "objc3c-frontend-build-invocation-integration-closeout/parser_build-d011-v1"
  if ([string]$payload.contract_id -ne $expectedContractId) {
    Write-Error "frontend integration closeout contract id mismatch in $closeoutPath"
    exit 2
  }

  $expectedDependencies = @(
    "objc3c-frontend-build-invocation-conformance-corpus/parser_build-d010-v1",
    "objc3c-frontend-build-invocation-conformance-matrix/parser_build-d009-v1",
    "objc3c-frontend-build-invocation-recovery-determinism-hardening/parser_build-d008-v1"
  )
  $dependencySet = @{}
  foreach ($contractId in @($payload.depends_on_contract_ids)) {
    $contractIdText = [string]$contractId
    if (-not [string]::IsNullOrWhiteSpace($contractIdText)) {
      $dependencySet[$contractIdText] = $true
    }
  }
  foreach ($requiredContractId in $expectedDependencies) {
    if (-not $dependencySet.ContainsKey($requiredContractId)) {
      Write-Error "frontend integration closeout missing dependency contract '$requiredContractId' in $closeoutPath"
      exit 2
    }
  }

  $closeoutGate = $payload.closeout_gate
  if ($null -eq $closeoutGate) {
    Write-Error "frontend integration closeout closeout_gate metadata missing in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.build_integration_gate_signoff) {
    Write-Error "frontend integration closeout build_integration_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.invocation_profile_gate_signoff) {
    Write-Error "frontend integration closeout invocation_profile_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if (-not [bool]$closeoutGate.corpus_coverage_gate_signoff) {
    Write-Error "frontend integration closeout corpus_coverage_gate_signoff must be true in $closeoutPath"
    exit 2
  }
  if ([int]$closeoutGate.deterministic_fail_closed_exit_code -ne 2) {
    Write-Error "frontend integration closeout deterministic_fail_closed_exit_code must be 2 in $closeoutPath"
    exit 2
  }
  if ([int]$closeoutGate.acceptance_corpus_count -le 0 -or [int]$closeoutGate.rejection_corpus_count -le 0) {
    Write-Error "frontend integration closeout acceptance/rejection corpus counts must be positive in $closeoutPath"
    exit 2
  }

  return [pscustomobject]@{
    integration_closeout_path = $closeoutPath
  }
}

$parsed = Parse-WrapperArguments -RawArgs $args
$exe = Resolve-NativeCompilerExecutablePath -RepoRoot $repoRoot
$buildResult = $null
$buildResult = Ensure-NativeCompilerAvailable -RepoRoot $repoRoot -BuildResult $buildResult

Assert-FrontendModuleScaffold -RepoRoot $repoRoot -BuildResult $buildResult
Assert-FrontendInvocationLock -RepoRoot $repoRoot -BuildResult $buildResult
$coreFeatureGuard = Assert-FrontendCoreFeatureExpansion -RepoRoot $repoRoot -BuildResult $buildResult -ParsedArgs $parsed
$edgeCompatGuard = Assert-FrontendEdgeCompatibility `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -ParsedArgs $parsed `
  -CoreFeatureGuard $coreFeatureGuard
Assert-FrontendEdgeRobustness -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
Assert-FrontendDiagnosticsHardening -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
Assert-FrontendRecoveryDeterminismHardening -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null
$effectiveCompileArgs = @($parsed.compile_args)
if ($null -ne $edgeCompatGuard -and $null -ne $edgeCompatGuard.normalized_compile_args) {
  $effectiveCompileArgs = @($edgeCompatGuard.normalized_compile_args)
}
$matrixGuard = Assert-FrontendConformanceMatrix `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -ParsedArgs $parsed `
  -EffectiveCompileArgs $effectiveCompileArgs
Assert-FrontendConformanceCorpus `
  -RepoRoot $repoRoot `
  -BuildResult $buildResult `
  -InvocationProfileKey ([string]$matrixGuard.profile_key) | Out-Null
Assert-FrontendIntegrationCloseout -RepoRoot $repoRoot -BuildResult $buildResult | Out-Null

$argsWithoutOutDir = @(Get-ArgsWithoutOutDir -CompileArgs $effectiveCompileArgs)
$inputPath = $null
if ($argsWithoutOutDir.Count -gt 0) {
  $inputCandidate = $argsWithoutOutDir[0]
  if (-not [string]::IsNullOrWhiteSpace($inputCandidate)) {
    $inputPath = [System.IO.Path]::GetFullPath($inputCandidate)
  }
}

$cacheRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/cache"
$compilerSourcePath = Join-Path $repoRoot "native/objc3c/src/main.cpp"
$cacheEntryContractId = "objc3c-native-cache-entry/parser_build-d008-v1"
$cacheKey = Get-CacheKey `
  -InputPath $inputPath `
  -ArgsWithoutOutDir $argsWithoutOutDir `
  -CompilerSourcePath $compilerSourcePath `
  -WrapperScriptPath $PSCommandPath

if ($parsed.use_cache -and $null -ne $cacheKey) {
  $entryDir = Join-Path $cacheRoot $cacheKey
  $filesDir = Join-Path $entryDir "files"
  $exitPath = Join-Path $entryDir "exit_code.txt"
  $readyPath = Join-Path $entryDir "ready.marker"

  $cacheRestore = Try-RestoreCacheEntry `
    -EntryDir $entryDir `
    -FilesDir $filesDir `
    -ExitPath $exitPath `
    -ReadyPath $readyPath `
    -DestinationRoot $parsed.out_dir `
    -CacheKey $cacheKey `
    -ExpectedEntryContractId $cacheEntryContractId
  if ($cacheRestore.restored) {
    Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix
    Write-CompileOutputProvenance `
      -RepoRoot $repoRoot `
      -CompileDir $parsed.out_dir `
      -EmitPrefix $parsed.emit_prefix `
      -InputPath $inputPath `
      -CompilerBinaryPath $exe `
      -RuntimeLibraryPath (Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib") `
      -WrapperScriptPath $PSCommandPath
    Write-Output "cache_hit=true"
    exit ([int]$cacheRestore.exit_code)
  }
}

$compileExit = Invoke-NativeCompiler -ExePath $exe -Arguments $effectiveCompileArgs

if ($compileExit -eq 0) {
  Assert-Objc3cRuntimeLaunchContract -CompileDir $parsed.out_dir -RepoRoot $repoRoot -EmitPrefix $parsed.emit_prefix
  Write-CompileOutputProvenance `
    -RepoRoot $repoRoot `
    -CompileDir $parsed.out_dir `
    -EmitPrefix $parsed.emit_prefix `
    -InputPath $inputPath `
    -CompilerBinaryPath $exe `
    -RuntimeLibraryPath (Join-Path $repoRoot "artifacts/lib/objc3_runtime.lib") `
    -WrapperScriptPath $PSCommandPath
}

if ($parsed.use_cache -and $null -ne $cacheKey) {
  try {
    New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
    $stagingDir = Join-Path $cacheRoot ("_stage_" + [Guid]::NewGuid().ToString("N"))
    $stageFilesDir = Join-Path $stagingDir "files"
    New-Item -ItemType Directory -Force -Path $stageFilesDir | Out-Null

    Copy-DirectoryContents -SourceRoot $parsed.out_dir -DestinationRoot $stageFilesDir
    $outputDigest = Get-DirectoryDeterminismDigest -Path $stageFilesDir
    Set-Content -LiteralPath (Join-Path $stagingDir "exit_code.txt") -Value "$compileExit" -Encoding ascii
    $metadataPayload = [ordered]@{
      entry_contract_id = $cacheEntryContractId
      schema_version = 1
      cache_key = $cacheKey
      compile_exit_code = [int]$compileExit
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

Write-Output "cache_hit=false"
exit $compileExit
