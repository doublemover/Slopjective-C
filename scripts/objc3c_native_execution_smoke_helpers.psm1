Set-StrictMode -Version Latest

function Ensure-NativeCompilerExecutable {
  param(
    [Parameter(Mandatory = $true)][string]$NativeExePath,
    [Parameter(Mandatory = $true)][bool]$NativeExeExplicit,
    [Parameter(Mandatory = $true)][string]$BuildScriptPath
  )

  if (Test-Path -LiteralPath $NativeExePath -PathType Leaf) {
    return
  }
  if ($NativeExeExplicit) {
    throw "execution smoke FAIL: configured native compiler missing at $NativeExePath"
  }
  if (!(Test-Path -LiteralPath $BuildScriptPath -PathType Leaf)) {
    throw "execution smoke FAIL: native build script missing at $BuildScriptPath"
  }

  & $BuildScriptPath -ExecutionMode binaries-only | Out-Null
  if ($LASTEXITCODE -ne 0) {
    throw "execution smoke FAIL: native compiler build failed with exit code $LASTEXITCODE"
  }
  if (!(Test-Path -LiteralPath $NativeExePath -PathType Leaf)) {
    throw "execution smoke FAIL: native compiler executable missing at $NativeExePath"
  }
}

function Get-RepoRelativePath {
  param(
    [Parameter(Mandatory = $true)][string]$Path,
    [Parameter(Mandatory = $true)][string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-ShortHash {
  param([Parameter(Mandatory = $true)][string]$Value)

  $sha1 = [System.Security.Cryptography.SHA1]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
    $hashBytes = $sha1.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant().Substring(0, 10)
  }
  finally {
    $sha1.Dispose()
  }
}

function Get-CaseDirectoryName {
  param(
    [Parameter(Mandatory = $true)][string]$RunDir,
    [Parameter(Mandatory = $true)][string]$Kind,
    [Parameter(Mandatory = $true)][string]$FixtureRelativePath,
    [Parameter(Mandatory = $true)][string]$FixtureBaseName
  )

  $prefix = "${Kind}_$(Get-ShortHash -Value $FixtureRelativePath)"
  $sanitizedBase = [regex]::Replace($FixtureBaseName.ToLowerInvariant(), '[^a-z0-9]+', '_').Trim('_')
  if ([string]::IsNullOrWhiteSpace($sanitizedBase)) {
    return $prefix
  }

  $reservedLeaf = "\compile\module.object-backend.txt"
  $maxPathLength = 220
  $available = $maxPathLength - ((Join-Path $RunDir $prefix).Length + $reservedLeaf.Length + 1)
  if ($available -le 0) {
    return $prefix
  }

  if ($sanitizedBase.Length -gt $available) {
    $sanitizedBase = $sanitizedBase.Substring(0, $available).TrimEnd('_')
  }
  if ([string]::IsNullOrWhiteSpace($sanitizedBase)) {
    return $prefix
  }

  return "${prefix}_$sanitizedBase"
}

function Invoke-LoggedCommand {
  param(
    [Parameter(Mandatory = $true)][string]$Command,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Arguments,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return [int]$LASTEXITCODE
  }
  finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Get-Fixtures {
  param(
    [Parameter(Mandatory = $true)][string]$Directory,
    [Parameter(Mandatory = $true)][string]$FixtureKind
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "execution smoke FAIL: missing $FixtureKind fixture directory $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File -Filter "*.objc3" |
      Sort-Object -Property FullName
  )
  if ($fixtures.Count -eq 0) {
    throw "execution smoke FAIL: no $FixtureKind fixtures found in $Directory"
  }
  return $fixtures
}

function Get-RequestedRelativePaths {
  param(
    [Parameter(Mandatory = $true)][string]$FixtureListPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $resolvedFixtureList = if ([System.IO.Path]::IsPathRooted($FixtureListPath)) {
    $FixtureListPath
  } else {
    Join-Path $RepoRoot $FixtureListPath
  }
  if (!(Test-Path -LiteralPath $resolvedFixtureList -PathType Leaf)) {
    throw "execution smoke FAIL: missing fixture list at $resolvedFixtureList"
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

function Select-ExecutionFixtureEntries {
  param(
    [Parameter(Mandatory = $true)][object[]]$Entries,
    [string]$FixtureListPath,
    [string]$FixtureGlobPattern,
    [Parameter(Mandatory = $true)][int]$ShardIndexValue,
    [Parameter(Mandatory = $true)][int]$ShardCountValue,
    [Parameter(Mandatory = $true)][int]$LimitValue,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  if ($LimitValue -lt 0) {
    throw "execution smoke FAIL: limit must be non-negative"
  }
  if ($ShardCountValue -lt 0) {
    throw "execution smoke FAIL: shard-count must be non-negative"
  }
  if (($ShardIndexValue -ge 0) -and ($ShardCountValue -le 0)) {
    throw "execution smoke FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCountValue -gt 0) -and (($ShardIndexValue -lt 0) -or ($ShardIndexValue -ge $ShardCountValue))) {
    throw "execution smoke FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
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
      throw "execution smoke FAIL: fixture-list entries did not match execution fixtures ($($missing -join ', '))"
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
    throw "execution smoke FAIL: no execution fixtures matched the requested selection"
  }

  return $selected
}

function Get-PositiveExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectedPath = [System.IO.Path]::ChangeExtension($FixturePath, ".exitcode.txt")
  if (!(Test-Path -LiteralPath $expectedPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing positive expectation file $expectedPath"
  }

  $raw = (Get-Content -LiteralPath $expectedPath -Raw).Trim()
  $parsed = 0
  if (![int]::TryParse($raw, [ref]$parsed)) {
    throw "execution smoke FAIL: invalid exit code '$raw' in $expectedPath"
  }
  $compileArgs = @()
  $requiresLiveRuntimeDispatch = $false
  $requiresLiveRuntimeDispatchExplicit = $false
  $runtimeDispatchSymbol = "objc3_runtime_dispatch_i32"
  $metaPath = [System.IO.Path]::ChangeExtension($FixturePath, ".meta.json")
  if (Test-Path -LiteralPath $metaPath -PathType Leaf) {
    $metaRaw = Get-Content -LiteralPath $metaPath -Raw
    $metaSpec = $null
    try {
      $metaSpec = $metaRaw | ConvertFrom-Json
    }
    catch {
      throw "execution smoke FAIL: invalid json in ${metaPath}: $($_.Exception.Message)"
    }

    $fixtureName = "$($metaSpec.fixture)".Trim()
    if ([string]::IsNullOrWhiteSpace($fixtureName) -or $fixtureName -ne [System.IO.Path]::GetFileName($FixturePath)) {
      throw "execution smoke FAIL: positive expectation fixture field does not match fixture name in $metaPath"
    }

    if ($null -ne $metaSpec.execution -and $metaSpec.execution.PSObject.Properties.Name -contains "native_compile_args") {
      foreach ($arg in @($metaSpec.execution.native_compile_args)) {
        $text = "$arg".Trim()
        if (![string]::IsNullOrWhiteSpace($text)) {
          $compileArgs += $text
        }
      }
    }
    if ($null -ne $metaSpec.execution -and $metaSpec.execution.PSObject.Properties.Name -contains "requires_runtime_link") {
      $requiresLiveRuntimeDispatch = [bool]$metaSpec.execution.requires_runtime_link
      $requiresLiveRuntimeDispatchExplicit = $true
    }
    if ($null -ne $metaSpec.execution -and $metaSpec.execution.PSObject.Properties.Name -contains "requires_live_runtime_dispatch") {
      $requiresLiveRuntimeDispatch = [bool]$metaSpec.execution.requires_live_runtime_dispatch
      $requiresLiveRuntimeDispatchExplicit = $true
    }
    if ($null -ne $metaSpec.execution -and $metaSpec.execution.PSObject.Properties.Name -contains "runtime_dispatch_symbol") {
      $candidate = "$($metaSpec.execution.runtime_dispatch_symbol)".Trim()
      if (-not [string]::IsNullOrWhiteSpace($candidate)) {
        $runtimeDispatchSymbol = $candidate
      }
    }
  }

  return [pscustomobject]@{
    expected_path = $expectedPath
    expected_exit = [int]$parsed
    compile_args = @($compileArgs)
    requires_live_runtime_dispatch = $requiresLiveRuntimeDispatch
    requires_live_runtime_dispatch_explicit = $requiresLiveRuntimeDispatchExplicit
    runtime_dispatch_symbol = $runtimeDispatchSymbol
    meta_path = $metaPath
  }
}

function Get-NegativeExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectPath = [System.IO.Path]::ChangeExtension($FixturePath, ".meta.json")
  if (!(Test-Path -LiteralPath $expectPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing negative expectation file $expectPath"
  }

  $raw = Get-Content -LiteralPath $expectPath -Raw
  $spec = $null
  try {
    $spec = $raw | ConvertFrom-Json
  }
  catch {
    throw "execution smoke FAIL: invalid json in ${expectPath}: $($_.Exception.Message)"
  }

  $stage = "$($spec.expect_failure.stage)".Trim().ToLowerInvariant()
  if ([string]::IsNullOrWhiteSpace($stage) -or ($stage -ne "compile" -and $stage -ne "link" -and $stage -ne "run")) {
    throw "execution smoke FAIL: negative expectation requires expect_failure.stage=compile|link|run in $expectPath"
  }

  $requiredTokens = @()
  if ($null -ne $spec.expect_failure.required_diagnostic_tokens) {
    foreach ($token in @($spec.expect_failure.required_diagnostic_tokens)) {
      $text = "$token".Trim()
      if (![string]::IsNullOrWhiteSpace($text)) {
        $requiredTokens += $text
      }
    }
  }
  if ($requiredTokens.Count -eq 0) {
    throw "execution smoke FAIL: negative expectation requires expect_failure.required_diagnostic_tokens in $expectPath"
  }

  $fixtureName = "$($spec.fixture)".Trim()
  if ([string]::IsNullOrWhiteSpace($fixtureName) -or $fixtureName -ne [System.IO.Path]::GetFileName($FixturePath)) {
    throw "execution smoke FAIL: negative expectation fixture field does not match fixture name in $expectPath"
  }

  $compileArgs = @()
  if ($null -ne $spec.execution -and $spec.execution.PSObject.Properties.Name -contains "native_compile_args") {
    foreach ($arg in @($spec.execution.native_compile_args)) {
      $text = "$arg".Trim()
      if (![string]::IsNullOrWhiteSpace($text)) {
        $compileArgs += $text
      }
    }
  }

  $requiresLiveRuntimeDispatch = $false
  $requiresLiveRuntimeDispatchExplicit = $false
  if ($null -ne $spec.execution -and $spec.execution.PSObject.Properties.Name -contains "requires_runtime_link") {
    $requiresLiveRuntimeDispatch = [bool]$spec.execution.requires_runtime_link
    $requiresLiveRuntimeDispatchExplicit = $true
  }

  if ($null -ne $spec.execution -and $spec.execution.PSObject.Properties.Name -contains "requires_live_runtime_dispatch") {
    $requiresLiveRuntimeDispatch = [bool]$spec.execution.requires_live_runtime_dispatch
    $requiresLiveRuntimeDispatchExplicit = $true
  }

  $runtimeDispatchSymbol = "objc3_runtime_dispatch_i32"
  if ($null -ne $spec.execution -and $spec.execution.PSObject.Properties.Name -contains "runtime_dispatch_symbol") {
    $candidate = "$($spec.execution.runtime_dispatch_symbol)".Trim()
    if (-not [string]::IsNullOrWhiteSpace($candidate)) {
      $runtimeDispatchSymbol = $candidate
    }
  }

  return [pscustomobject]@{
    stage = $stage
    compile_args = @($compileArgs)
    requires_live_runtime_dispatch = $requiresLiveRuntimeDispatch
    requires_live_runtime_dispatch_explicit = $requiresLiveRuntimeDispatchExplicit
    runtime_dispatch_symbol = $runtimeDispatchSymbol
    required_link_tokens = @($requiredTokens)
    expectation_path = $expectPath
  }
}

function Get-MissingTokens {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$Tokens
  )

  $missing = New-Object 'System.Collections.Generic.List[string]'
  foreach ($token in $Tokens) {
    if ([string]::IsNullOrWhiteSpace($token)) {
      continue
    }
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      $null = $missing.Add($token)
    }
  }
  return @($missing.ToArray())
}

function Resolve-NativeObjectPath {
  param(
    [Parameter(Mandatory = $true)][string]$CompileDir,
    [Parameter(Mandatory = $true)][string]$FixtureRel
  )

  $preferredNames = @("module.obj", "module.o")
  foreach ($name in $preferredNames) {
    $candidate = Join-Path $CompileDir $name
    if (Test-Path -LiteralPath $candidate -PathType Leaf) {
      return $candidate
    }
  }

  $moduleNamedCandidates = @(
    Get-ChildItem -LiteralPath $CompileDir -File -ErrorAction SilentlyContinue |
      Where-Object { $_.BaseName -eq "module" -and ($_.Extension -eq ".obj" -or $_.Extension -eq ".o") } |
      Sort-Object -Property Name
  )
  if ($moduleNamedCandidates.Count -eq 1) {
    return $moduleNamedCandidates[0].FullName
  }
  if ($moduleNamedCandidates.Count -gt 1) {
    throw "execution smoke FAIL: ambiguous native object artifacts for $fixtureRel in $CompileDir"
  }

  $allObjectCandidates = @(
    Get-ChildItem -LiteralPath $CompileDir -File -ErrorAction SilentlyContinue |
      Where-Object { $_.Extension -eq ".obj" -or $_.Extension -eq ".o" } |
      Sort-Object -Property Name
  )
  if ($allObjectCandidates.Count -eq 1) {
    return $allObjectCandidates[0].FullName
  }

  throw "execution smoke FAIL: missing native object artifact (.obj/.o) for $fixtureRel in $CompileDir"
}

function Get-CanonicalLinkDiagnosticsText {
  param(
    [Parameter(Mandatory = $true)][string]$RawText,
    [Parameter(Mandatory = $true)][string]$ObjectPath,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $normalizedRaw = "$RawText"
  $normalizedRaw = $normalizedRaw -replace "`r`n", "`n"
  $normalizedRaw = $normalizedRaw -replace "`r", "`n"

  $lines = New-Object 'System.Collections.Generic.List[string]'
  $null = $lines.Add("link.input_object:$(Get-RepoRelativePath -Path $ObjectPath -Root $RepoRoot)")
  $null = $lines.Add("link.input_object_basename:$([System.IO.Path]::GetFileName($ObjectPath))")

  $unresolvedSymbols = New-Object 'System.Collections.Generic.HashSet[string]' ([System.StringComparer]::Ordinal)
  foreach ($match in [regex]::Matches($normalizedRaw, '(?im)unresolved external symbol\s+([A-Za-z_.$?@][A-Za-z0-9_.$?@]*)')) {
    $null = $unresolvedSymbols.Add($match.Groups[1].Value)
  }
  foreach ($match in [regex]::Matches($normalizedRaw, '(?im)undefined (?:reference|symbol)(?:\s+to)?\s+[^A-Za-z_.$?@]*([A-Za-z_.$?@][A-Za-z0-9_.$?@]*)')) {
    $null = $unresolvedSymbols.Add($match.Groups[1].Value)
  }
  foreach ($symbol in @($unresolvedSymbols) | Sort-Object) {
    $null = $lines.Add("link.unresolved_symbol:$symbol")
  }

  $entryPointMissing = $false
  if ($normalizedRaw -match '(?im)\bentry point\b') {
    $entryPointMissing = $true
  } elseif ($normalizedRaw -match '(?im)undefined (?:reference|symbol)(?:\s+to)?\s+[^A-Za-z_.$?@]*main\b') {
    $entryPointMissing = $true
  }
  if ($entryPointMissing) {
    $null = $lines.Add("link.entrypoint_missing")
  }

  if (-not [string]::IsNullOrWhiteSpace($normalizedRaw)) {
    $null = $lines.Add("link.raw.begin")
    $null = $lines.Add($normalizedRaw.Trim())
    $null = $lines.Add("link.raw.end")
  }

  return ($lines -join "`n")
}

function Assert-RuntimeDispatchParityFromLl {
  param(
    [Parameter(Mandatory = $true)][string]$LlPath,
    [Parameter(Mandatory = $true)][string]$FixtureRel,
    [Parameter(Mandatory = $true)][bool]$RequiresLiveRuntimeDispatch,
    [Parameter(Mandatory = $true)][string]$RuntimeDispatchSymbol
  )

  if (!(Test-Path -LiteralPath $LlPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing module.ll for runtime-dispatch parity check ($FixtureRel)"
  }
  $llText = Get-Content -LiteralPath $LlPath -Raw
  $llCodeText = (($llText -split "`r?`n") | Where-Object { $_ -notmatch '^\s*;' }) -join "`n"
  $declareToken = "declare i32 @$RuntimeDispatchSymbol("
  $callToken = "call i32 @$RuntimeDispatchSymbol("
  $hasDeclare = $llCodeText.IndexOf($declareToken, [System.StringComparison]::Ordinal) -ge 0
  $hasCall = $llCodeText.IndexOf($callToken, [System.StringComparison]::Ordinal) -ge 0

  if ($RequiresLiveRuntimeDispatch) {
    if (-not $hasDeclare -or -not $hasCall) {
      throw "execution smoke FAIL: live-runtime-dispatch metadata requires dispatch declaration+call for $FixtureRel (symbol=$RuntimeDispatchSymbol)"
    }
    return
  }

  if ($hasDeclare -or $hasCall) {
    throw "execution smoke FAIL: live-runtime-dispatch metadata forbids dispatch declaration/call for $FixtureRel (symbol=$RuntimeDispatchSymbol)"
  }
}

Export-ModuleMember -Function @(
  "Assert-RuntimeDispatchParityFromLl",
  "Ensure-NativeCompilerExecutable",
  "Get-CanonicalLinkDiagnosticsText",
  "Get-CaseDirectoryName",
  "Get-Fixtures",
  "Get-MissingTokens",
  "Get-NegativeExpectation",
  "Get-PositiveExpectation",
  "Get-RepoRelativePath",
  "Invoke-LoggedCommand",
  "Resolve-NativeObjectPath",
  "Select-ExecutionFixtureEntries"
)
