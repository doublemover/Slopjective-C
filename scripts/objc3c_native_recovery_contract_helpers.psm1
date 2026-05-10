Set-StrictMode -Version Latest

function Get-EntrypointLlSurface {
  param(
    [string]$LlText
  )

  $lines = $LlText -split "`r?`n"
  $capturing = $false
  $braceDepth = 0
  $captured = New-Object System.Collections.Generic.List[string]

  foreach ($line in $lines) {
    if (-not $capturing -and $line -match '^\s*define\s+i32\s+@(main|objc3c_entry)\(') {
      $capturing = $true
    }

    if (-not $capturing) {
      continue
    }

    $captured.Add($line)
    $braceDepth += ([regex]::Matches($line, '\{')).Count
    $braceDepth -= ([regex]::Matches($line, '\}')).Count

    if ($braceDepth -le 0 -and $line -match '^\s*\}') {
      $capturing = $false
      $braceDepth = 0
    }
  }

  return ($captured -join "`n")
}

function Assert-Objc3ManifestPipelineSurface {
  param(
    [string]$ManifestText,
    [string]$CaseName
  )

  try {
    if ($PSVersionTable.PSVersion.Major -ge 6) {
      $manifest = $ManifestText | ConvertFrom-Json -Depth 64
    } else {
      $manifest = $ManifestText | ConvertFrom-Json
    }
  } catch {
    throw "contract FAIL: invalid manifest JSON for $CaseName"
  }

  if ($null -eq $manifest.frontend -or $null -eq $manifest.frontend.pipeline) {
    throw "contract FAIL: missing frontend.pipeline surface for $CaseName"
  }

  $pipeline = $manifest.frontend.pipeline
  if ($null -eq $pipeline.stages) {
    throw "contract FAIL: missing frontend.pipeline.stages for $CaseName"
  }
  if ($null -eq $pipeline.semantic_surface) {
    throw "contract FAIL: missing frontend.pipeline.semantic_surface for $CaseName"
  }

  foreach ($stageName in @("lexer", "parser", "semantic")) {
    $stage = $pipeline.stages.$stageName
    if ($null -eq $stage) {
      throw "contract FAIL: missing frontend.pipeline.stages.$stageName for $CaseName"
    }
    if ($null -eq $stage.diagnostics) {
      throw "contract FAIL: missing frontend.pipeline.stages.$stageName.diagnostics for $CaseName"
    }
    if ([int]$stage.diagnostics -ne 0) {
      throw "contract FAIL: expected zero stage diagnostics for successful compile ($CaseName stage=$stageName value=$($stage.diagnostics))"
    }
  }

  if ([bool]$pipeline.semantic_skipped) {
    throw "contract FAIL: frontend.pipeline.semantic_skipped must be false for successful .objc3 compile ($CaseName)"
  }

  $declaredGlobals = @($manifest.globals).Count
  $declaredFunctions = @($manifest.functions).Count
  $surface = $pipeline.semantic_surface
  if ([int]$surface.declared_globals -ne $declaredGlobals) {
    throw "contract FAIL: semantic surface declared_globals mismatch for $CaseName"
  }
  if ([int]$surface.declared_functions -ne $declaredFunctions) {
    throw "contract FAIL: semantic surface declared_functions mismatch for $CaseName"
  }
  if ([int]$surface.resolved_global_symbols -ne $declaredGlobals) {
    throw "contract FAIL: semantic surface resolved_global_symbols mismatch for $CaseName"
  }
  if ([int]$surface.resolved_function_symbols -ne $declaredFunctions) {
    throw "contract FAIL: semantic surface resolved_function_symbols mismatch for $CaseName"
  }

  if ($null -eq $surface.function_signature_surface) {
    throw "contract FAIL: missing frontend.pipeline.semantic_surface.function_signature_surface for $CaseName"
  }
  $signatureSurface = $surface.function_signature_surface
  foreach ($field in @("scalar_return_i32", "scalar_return_bool", "scalar_return_void", "scalar_param_i32", "scalar_param_bool")) {
    if ($null -eq $signatureSurface.$field) {
      throw "contract FAIL: missing frontend.pipeline.semantic_surface.function_signature_surface.$field for $CaseName"
    }
  }

  $functions = @($manifest.functions)
  $computedReturnI32 = 0
  $computedReturnBool = 0
  $computedReturnVoid = 0
  $computedParamI32 = 0
  $computedParamBool = 0
  $hasExtendedSignatureTypes = $false
  foreach ($fn in $functions) {
    if ($null -eq $fn.param_types) {
      throw "contract FAIL: missing function.param_types in manifest for $CaseName"
    }
    $paramTypes = @($fn.param_types)
    if ($paramTypes.Count -ne [int]$fn.params) {
      throw "contract FAIL: function.param_types length mismatch for $CaseName function=$($fn.name)"
    }
    foreach ($paramType in $paramTypes) {
      if ($paramType -eq "i32") {
        $computedParamI32++
      } elseif ($paramType -eq "bool") {
        $computedParamBool++
      } elseif ([string]::IsNullOrWhiteSpace([string]$paramType)) {
        throw "contract FAIL: empty function.param_types entry for $CaseName function=$($fn.name)"
      } else {
        $hasExtendedSignatureTypes = $true
      }
    }

    if ($fn.return -eq "i32") {
      $computedReturnI32++
    } elseif ($fn.return -eq "bool") {
      $computedReturnBool++
    } elseif ($fn.return -eq "void") {
      $computedReturnVoid++
    } elseif ([string]::IsNullOrWhiteSpace([string]$fn.return)) {
      throw "contract FAIL: empty function.return for $CaseName function=$($fn.name)"
    } else {
      $hasExtendedSignatureTypes = $true
    }
  }

  if (-not $hasExtendedSignatureTypes) {
    if ([int]$signatureSurface.scalar_return_i32 -ne $computedReturnI32) {
      throw "contract FAIL: function_signature_surface.scalar_return_i32 mismatch for $CaseName"
    }
    if ([int]$signatureSurface.scalar_return_bool -ne $computedReturnBool) {
      throw "contract FAIL: function_signature_surface.scalar_return_bool mismatch for $CaseName"
    }
    if ([int]$signatureSurface.scalar_return_void -ne $computedReturnVoid) {
      throw "contract FAIL: function_signature_surface.scalar_return_void mismatch for $CaseName"
    }
    if ([int]$signatureSurface.scalar_param_i32 -ne $computedParamI32) {
      throw "contract FAIL: function_signature_surface.scalar_param_i32 mismatch for $CaseName"
    }
    if ([int]$signatureSurface.scalar_param_bool -ne $computedParamBool) {
      throw "contract FAIL: function_signature_surface.scalar_param_bool mismatch for $CaseName"
    }
  }
}

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

function Get-FixtureCaseName {
  param(
    [string]$Prefix,
    [string]$FixturePath
  )

  $leaf = [System.IO.Path]::GetFileNameWithoutExtension($FixturePath)
  $leaf = $leaf -replace "[^A-Za-z0-9_-]", "_"
  if ($leaf.Length -gt 48) {
    $leaf = $leaf.Substring(0, 48)
  }

  $fullPath = [System.IO.Path]::GetFullPath($FixturePath)
  $bytes = [System.Text.Encoding]::UTF8.GetBytes($fullPath.Replace("\", "/"))
  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($bytes)
  } finally {
    $sha256.Dispose()
  }
  $hash = [System.BitConverter]::ToString($hashBytes).Replace("-", "").ToLowerInvariant().Substring(0, 12)

  return "$Prefix`_$leaf`_$hash"
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

function Read-JsonHashtable {
  param([string]$Path)

  return (Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json -AsHashtable)
}

function Assert-CompileOutputProvenance {
  param(
    [string]$CaseName,
    [string]$RunDir
  )

  $provenancePath = Join-Path $RunDir "module.compile-provenance.json"
  if (!(Test-Path -LiteralPath $provenancePath -PathType Leaf)) {
    throw "contract FAIL: missing compile provenance artifact for $CaseName"
  }
  $registrationManifestPath = Join-Path $RunDir "module.runtime-registration-manifest.json"
  $hasRegistrationManifest = Test-Path -LiteralPath $registrationManifestPath -PathType Leaf

  $provenanceText = Get-Content -LiteralPath $provenancePath -Raw
  if ([string]::IsNullOrWhiteSpace($provenanceText)) {
    throw "contract FAIL: empty compile provenance artifact for $CaseName"
  }
  $provenance = Read-JsonHashtable -Path $provenancePath
  $registrationManifest = if ($hasRegistrationManifest) { Read-JsonHashtable -Path $registrationManifestPath } else { $null }

  if ([string]$provenance["contract_id"] -ne "objc3c.native.compile.output.provenance.v1") {
    throw "contract FAIL: unexpected compile provenance contract id for $CaseName"
  }
  if ($hasRegistrationManifest -and [string]$registrationManifest["compile_output_provenance_artifact"] -ne "module.compile-provenance.json") {
    throw "contract FAIL: runtime registration manifest missing compile provenance binding for $CaseName"
  }

  $truthfulness = $provenance["compile_output_truthfulness"]
  if ($null -eq $truthfulness -or [string]$truthfulness["contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "contract FAIL: missing compile output truthfulness envelope for $CaseName"
  }
  if (-not [bool]$truthfulness["truthful"]) {
    throw "contract FAIL: compile output truthfulness envelope did not certify emitted artifacts for $CaseName"
  }
  if ($hasRegistrationManifest -and [string]$registrationManifest["compile_output_truthfulness_contract_id"] -ne "objc3c.native.compile.output.truthfulness.v1") {
    throw "contract FAIL: runtime registration manifest missing compile output truthfulness contract id for $CaseName"
  }
  if ($hasRegistrationManifest -and -not [bool]$registrationManifest["compile_output_truthful"]) {
    throw "contract FAIL: runtime registration manifest did not certify truthful compile output for $CaseName"
  }
  if ($hasRegistrationManifest -and [string]$truthfulness["runtime_dispatch_symbol"] -ne [string]$registrationManifest["compile_output_truthfulness_runtime_dispatch_symbol"]) {
    throw "contract FAIL: runtime registration manifest compile output truthfulness dispatch symbol mismatch for $CaseName"
  }
  if ($hasRegistrationManifest -and [int]$truthfulness["property_descriptor_definition_count"] -ne [int]$registrationManifest["compile_output_truthfulness_property_descriptor_count"]) {
    throw "contract FAIL: runtime registration manifest compile output truthfulness property descriptor count mismatch for $CaseName"
  }
  if ($hasRegistrationManifest -and [int]$truthfulness["ivar_descriptor_definition_count"] -ne [int]$registrationManifest["compile_output_truthfulness_ivar_descriptor_count"]) {
    throw "contract FAIL: runtime registration manifest compile output truthfulness ivar descriptor count mismatch for $CaseName"
  }

  $artifactEntries = @($provenance["emitted_artifacts"])
  if ($artifactEntries.Count -lt 4) {
    throw "contract FAIL: compile provenance emitted_artifacts too small for $CaseName"
  }

  $digestLines = New-Object System.Collections.Generic.List[string]
  foreach ($entry in $artifactEntries) {
    $artifactPath = Join-Path $RunDir ([string]$entry["path"])
    if (!(Test-Path -LiteralPath $artifactPath -PathType Leaf)) {
      throw "contract FAIL: compile provenance referenced missing artifact '$artifactPath' for $CaseName"
    }
    $actualHash = Get-FileSha256Hex -Path $artifactPath
    if ($actualHash -ne ([string]$entry["sha256"]).ToLowerInvariant()) {
      throw "contract FAIL: compile provenance sha256 mismatch for '$artifactPath' in $CaseName"
    }
    $actualSize = (Get-Item -LiteralPath $artifactPath).Length
    if ([long]$actualSize -ne [long]$entry["byte_count"]) {
      throw "contract FAIL: compile provenance byte_count mismatch for '$artifactPath' in $CaseName"
    }
    $digestLines.Add(("{0}|{1}|{2}" -f [string]$entry["path"], [string]$entry["byte_count"], [string]$entry["sha256"]))
  }

  $actualArtifactSetDigest = Get-Sha256HexFromBytes -Bytes ([System.Text.Encoding]::UTF8.GetBytes(($digestLines -join "`n")))
  if ($actualArtifactSetDigest -ne ([string]$provenance["artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "contract FAIL: compile provenance artifact set digest mismatch for $CaseName"
  }
  if ($hasRegistrationManifest -and $actualArtifactSetDigest -ne ([string]$registrationManifest["compile_output_artifact_set_digest_sha256"]).ToLowerInvariant()) {
    throw "contract FAIL: runtime registration manifest compile output digest mismatch for $CaseName"
  }

  return $provenanceText
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
  "Assert-CompileOutputProvenance",
  "Assert-Objc3ManifestPipelineSurface",
  "Assert-RecoveryFixtureClass",
  "Get-EntrypointLlSurface",
  "Get-FixtureCaseName",
  "Get-RecoveryFixtures",
  "Select-RecoveryFixtureEntries"
)
