$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$positiveFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/positive"
$negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/execution/negative"
$runtimeShimSource = Join-Path $repoRoot "tests/tooling/runtime/objc3_msgsend_i32_shim.c"
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/execution-smoke"
$configuredRunId = $env:OBJC3C_NATIVE_EXECUTION_RUN_ID
$runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$defaultNativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$configuredNativeExe = $env:OBJC3C_NATIVE_EXECUTABLE
$nativeExe = if ([string]::IsNullOrWhiteSpace($configuredNativeExe)) { $defaultNativeExe } else { $configuredNativeExe }
$nativeExeExplicit = -not [string]::IsNullOrWhiteSpace($configuredNativeExe)
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$configuredClangPath = $env:OBJC3C_NATIVE_EXECUTION_CLANG_PATH
$clangCommand = if ([string]::IsNullOrWhiteSpace($configuredClangPath)) { "clang" } else { $configuredClangPath }

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
  $requiresRuntimeShim = $true
  $requiresRuntimeShimExplicit = $false
  $runtimeDispatchSymbol = "objc3_msgsend_i32"
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
    if ($null -ne $metaSpec.execution -and $metaSpec.execution.PSObject.Properties.Name -contains "requires_runtime_shim") {
      $requiresRuntimeShim = [bool]$metaSpec.execution.requires_runtime_shim
      $requiresRuntimeShimExplicit = $true
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
    requires_runtime_shim = $requiresRuntimeShim
    requires_runtime_shim_explicit = $requiresRuntimeShimExplicit
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

  $requiresRuntimeShim = $false
  $requiresRuntimeShimExplicit = $false
  if ($null -ne $spec.execution -and $spec.execution.PSObject.Properties.Name -contains "requires_runtime_shim") {
    $requiresRuntimeShim = [bool]$spec.execution.requires_runtime_shim
    $requiresRuntimeShimExplicit = $true
  }

  $runtimeDispatchSymbol = "objc3_msgsend_i32"
  if ($null -ne $spec.execution -and $spec.execution.PSObject.Properties.Name -contains "runtime_dispatch_symbol") {
    $candidate = "$($spec.execution.runtime_dispatch_symbol)".Trim()
    if (-not [string]::IsNullOrWhiteSpace($candidate)) {
      $runtimeDispatchSymbol = $candidate
    }
  }

  return [pscustomobject]@{
    stage = $stage
    requires_runtime_shim = $requiresRuntimeShim
    requires_runtime_shim_explicit = $requiresRuntimeShimExplicit
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
    [Parameter(Mandatory = $true)][bool]$RequiresRuntimeShim,
    [Parameter(Mandatory = $true)][string]$RuntimeDispatchSymbol
  )

  if (!(Test-Path -LiteralPath $LlPath -PathType Leaf)) {
    throw "execution smoke FAIL: missing module.ll for runtime-dispatch parity check ($FixtureRel)"
  }
  $llText = Get-Content -LiteralPath $LlPath -Raw
  $declareToken = "declare i32 @$RuntimeDispatchSymbol("
  $callToken = "call i32 @$RuntimeDispatchSymbol("
  $hasDeclare = $llText.IndexOf($declareToken, [System.StringComparison]::Ordinal) -ge 0
  $hasCall = $llText.IndexOf($callToken, [System.StringComparison]::Ordinal) -ge 0

  if ($RequiresRuntimeShim) {
    if (-not $hasDeclare -or -not $hasCall) {
      throw "execution smoke FAIL: runtime-shim metadata requires dispatch declaration+call for $FixtureRel (symbol=$RuntimeDispatchSymbol)"
    }
    return
  }

  if ($hasDeclare -or $hasCall) {
    throw "execution smoke FAIL: runtime-shim metadata forbids dispatch declaration/call for $FixtureRel (symbol=$RuntimeDispatchSymbol)"
  }
}

New-Item -ItemType Directory -Force -Path $runDir | Out-Null
Push-Location $repoRoot
try {
  if (-not $nativeExeExplicit -and !(Test-Path -LiteralPath $nativeExe -PathType Leaf)) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $buildScript
    if ($LASTEXITCODE -ne 0) {
      throw "execution smoke FAIL: native compiler build failed with exit $LASTEXITCODE"
    }
  }

  if (!(Test-Path -LiteralPath $nativeExe -PathType Leaf)) {
    throw "execution smoke FAIL: native compiler executable missing at $nativeExe"
  }
  if (!(Test-Path -LiteralPath $runtimeShimSource -PathType Leaf)) {
    throw "execution smoke FAIL: runtime shim source missing at $runtimeShimSource"
  }

  $clangCheckExit = Invoke-LoggedCommand -Command $clangCommand -Arguments @("--version") -LogPath (Join-Path $runDir "clang-version.log")
  if ($clangCheckExit -ne 0) {
    throw "execution smoke FAIL: clang command is unavailable: $clangCommand"
  }

  $positiveFixtures = Get-Fixtures -Directory $positiveFixtureDir -FixtureKind "positive execution"
  $negativeFixtures = Get-Fixtures -Directory $negativeFixtureDir -FixtureKind "negative execution"

  $results = @()

  foreach ($fixture in $positiveFixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    $expectation = Get-PositiveExpectation -FixturePath $fixture.FullName
    $caseHash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $runDir "positive_${caseHash}_$($fixture.BaseName)"
    $compileDir = Join-Path $caseDir "compile"
    $exePath = Join-Path $caseDir "module.exe"
    $compileLog = Join-Path $caseDir "compile.log"
    $linkLog = Join-Path $caseDir "link.log"
    New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

    $nativeArgs = @($fixture.FullName, "--out-dir", $compileDir, "--emit-prefix", "module")
    if ($expectation.compile_args.Count -gt 0) {
      $nativeArgs += @($expectation.compile_args)
    }
    $compileExit = Invoke-LoggedCommand -Command $nativeExe -Arguments $nativeArgs -LogPath $compileLog
    if ($compileExit -ne 0) {
      throw "execution smoke FAIL: compile failed for $fixtureRel (exit=$compileExit)"
    }

    if ($expectation.requires_runtime_shim_explicit) {
      $llPath = Join-Path $compileDir "module.ll"
      Assert-RuntimeDispatchParityFromLl `
        -LlPath $llPath `
        -FixtureRel $fixtureRel `
        -RequiresRuntimeShim $expectation.requires_runtime_shim `
        -RuntimeDispatchSymbol $expectation.runtime_dispatch_symbol
    }

    $objPath = Resolve-NativeObjectPath -CompileDir $compileDir -FixtureRel $fixtureRel

    $linkArgs = @($objPath)
    if ($expectation.requires_runtime_shim) {
      $linkArgs += $runtimeShimSource
    }
    $linkArgs += @("-o", $exePath, "-fno-color-diagnostics")
    $linkExit = Invoke-LoggedCommand -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
    if ($linkExit -ne 0) {
      throw "execution smoke FAIL: link failed for $fixtureRel (exit=$linkExit)"
    }
    if (!(Test-Path -LiteralPath $exePath -PathType Leaf)) {
      throw "execution smoke FAIL: missing module.exe for $fixtureRel"
    }

    $runLog = Join-Path $caseDir "run.log"
    $runExit = Invoke-LoggedCommand -Command $exePath -Arguments @() -LogPath $runLog
    $expectedExit = [int]$expectation.expected_exit
    $passed = ($runExit -eq $expectedExit)
    if (-not $passed) {
      throw "execution smoke FAIL: unexpected run exit for $fixtureRel (expected=$expectedExit actual=$runExit)"
    }

    $results += [pscustomobject]@{
      kind = "positive"
      fixture = $fixtureRel
      expectation = Get-RepoRelativePath -Path $expectation.expected_path -Root $repoRoot
      meta = if (Test-Path -LiteralPath $expectation.meta_path -PathType Leaf) { Get-RepoRelativePath -Path $expectation.meta_path -Root $repoRoot } else { "" }
      native_compile_args = @($expectation.compile_args)
      requires_runtime_shim = $expectation.requires_runtime_shim
      requires_runtime_shim_explicit = $expectation.requires_runtime_shim_explicit
      runtime_dispatch_symbol = $expectation.runtime_dispatch_symbol
      compile_exit = $compileExit
      link_exit = $linkExit
      run_exit = $runExit
      expected_exit = $expectedExit
      passed = $true
      out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
    }
    Write-Output "[PASS] positive $fixtureRel (run_exit=$runExit)"
  }

  foreach ($fixture in $negativeFixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    $spec = Get-NegativeExpectation -FixturePath $fixture.FullName
    $caseHash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $runDir "negative_${caseHash}_$($fixture.BaseName)"
    $compileDir = Join-Path $caseDir "compile"
    $exePath = Join-Path $caseDir "module.exe"
    $compileLog = Join-Path $caseDir "compile.log"
    $linkLog = Join-Path $caseDir "link.log"
    $runLog = Join-Path $caseDir "run.log"
    New-Item -ItemType Directory -Force -Path $compileDir | Out-Null

    $compileExit = Invoke-LoggedCommand -Command $nativeExe -Arguments @($fixture.FullName, "--out-dir", $compileDir, "--emit-prefix", "module") -LogPath $compileLog
    $compileDiagPath = Join-Path $compileDir "module.diagnostics.txt"
    $compileText = if (Test-Path -LiteralPath $compileDiagPath -PathType Leaf) {
      Get-Content -LiteralPath $compileDiagPath -Raw
    } elseif (Test-Path -LiteralPath $compileLog -PathType Leaf) {
      Get-Content -LiteralPath $compileLog -Raw
    } else {
      ""
    }

    if ($spec.stage -eq "compile") {
      if ($compileExit -eq 0) {
        throw "execution smoke FAIL: expected compile failure for negative fixture $fixtureRel"
      }
      $missingTokens = @(Get-MissingTokens -Text $compileText -Tokens $spec.required_link_tokens)
      if ($missingTokens.Count -gt 0) {
        throw "execution smoke FAIL: missing expected compile diagnostics for $fixtureRel (missing=$($missingTokens -join '|'))"
      }
      $results += [pscustomobject]@{
        kind = "negative"
        fixture = $fixtureRel
        expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $repoRoot
        stage = $spec.stage
        requires_runtime_shim = $spec.requires_runtime_shim
        runtime_dispatch_symbol = $spec.runtime_dispatch_symbol
        compile_exit = $compileExit
        link_exit = -1
        run_exit = -1
        required_link_tokens = $spec.required_link_tokens
        missing_link_tokens = @()
        passed = $true
        out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
      }
      Write-Output "[PASS] negative $fixtureRel (stage=compile compile_exit=$compileExit)"
      continue
    }

    if ($compileExit -ne 0) {
      throw "execution smoke FAIL: compile failed for negative fixture $fixtureRel (exit=$compileExit)"
    }

    if ($spec.requires_runtime_shim_explicit) {
      $llPath = Join-Path $compileDir "module.ll"
      Assert-RuntimeDispatchParityFromLl `
        -LlPath $llPath `
        -FixtureRel $fixtureRel `
        -RequiresRuntimeShim $spec.requires_runtime_shim `
        -RuntimeDispatchSymbol $spec.runtime_dispatch_symbol
    }

    $objPath = Resolve-NativeObjectPath -CompileDir $compileDir -FixtureRel $fixtureRel
    $linkArgs = @($objPath, "-o", $exePath, "-fno-color-diagnostics")
    if ($spec.stage -eq "link") {
      $linkExit = Invoke-LoggedCommand -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
      if ($linkExit -eq 0) {
        throw "execution smoke FAIL: expected link failure for negative fixture $fixtureRel"
      }

      $linkText = if (Test-Path -LiteralPath $linkLog -PathType Leaf) { Get-Content -LiteralPath $linkLog -Raw } else { "" }
      $linkDiagnosticsPath = Join-Path $caseDir "link.diagnostics.txt"
      $canonicalLinkText = Get-CanonicalLinkDiagnosticsText -RawText $linkText -ObjectPath $objPath -RepoRoot $repoRoot
      Set-Content -LiteralPath $linkDiagnosticsPath -Value $canonicalLinkText -Encoding utf8
      $missingTokens = @(Get-MissingTokens -Text $canonicalLinkText -Tokens $spec.required_link_tokens)
      if ($missingTokens.Count -gt 0) {
        $linkDiagnosticsRel = Get-RepoRelativePath -Path $linkDiagnosticsPath -Root $repoRoot
        throw "execution smoke FAIL: missing expected link diagnostics for $fixtureRel (missing=$($missingTokens -join '|') diagnostics=$linkDiagnosticsRel)"
      }

      $results += [pscustomobject]@{
        kind = "negative"
        fixture = $fixtureRel
        expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $repoRoot
        stage = $spec.stage
        requires_runtime_shim = $spec.requires_runtime_shim
        runtime_dispatch_symbol = $spec.runtime_dispatch_symbol
        compile_exit = $compileExit
        link_exit = $linkExit
        run_exit = -1
        required_link_tokens = $spec.required_link_tokens
        missing_link_tokens = @()
        link_diagnostics = Get-RepoRelativePath -Path $linkDiagnosticsPath -Root $repoRoot
        passed = $true
        out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
      }
      Write-Output "[PASS] negative $fixtureRel (stage=link link_exit=$linkExit)"
      continue
    }

    if ($spec.stage -eq "run") {
      if ($spec.requires_runtime_shim) {
        $linkArgs = @($objPath, $runtimeShimSource, "-o", $exePath, "-fno-color-diagnostics")
      }
      $linkExit = Invoke-LoggedCommand -Command $clangCommand -Arguments $linkArgs -LogPath $linkLog
      if ($linkExit -ne 0) {
        throw "execution smoke FAIL: expected successful link for run-stage negative fixture $fixtureRel (exit=$linkExit)"
      }

      $runExit = Invoke-LoggedCommand -Command $exePath -Arguments @() -LogPath $runLog
      if ($runExit -eq 0) {
        throw "execution smoke FAIL: expected non-zero run exit for negative fixture $fixtureRel"
      }
      $runText = if (Test-Path -LiteralPath $runLog -PathType Leaf) { Get-Content -LiteralPath $runLog -Raw } else { "" }
      $missingTokens = @(Get-MissingTokens -Text $runText -Tokens $spec.required_link_tokens)
      if ($missingTokens.Count -gt 0) {
        throw "execution smoke FAIL: missing expected run diagnostics for $fixtureRel (missing=$($missingTokens -join '|'))"
      }

      $results += [pscustomobject]@{
        kind = "negative"
        fixture = $fixtureRel
        expectation = Get-RepoRelativePath -Path $spec.expectation_path -Root $repoRoot
        stage = $spec.stage
        requires_runtime_shim = $spec.requires_runtime_shim
        runtime_dispatch_symbol = $spec.runtime_dispatch_symbol
        compile_exit = $compileExit
        link_exit = $linkExit
        run_exit = $runExit
        required_link_tokens = $spec.required_link_tokens
        missing_link_tokens = @()
        passed = $true
        out_dir = Get-RepoRelativePath -Path $caseDir -Root $repoRoot
      }
      Write-Output "[PASS] negative $fixtureRel (stage=run run_exit=$runExit)"
      continue
    }

    throw "execution smoke FAIL: unsupported negative stage '$($spec.stage)' for $fixtureRel"
  }

  $total = $results.Count
  $passedCount = @($results | Where-Object { $_.passed }).Count
  $failedCount = $total - $passedCount
  $summary = [ordered]@{
    run_dir = Get-RepoRelativePath -Path $runDir -Root $repoRoot
    native_exe = if (Test-Path -LiteralPath $nativeExe -PathType Leaf) { Get-RepoRelativePath -Path $nativeExe -Root $repoRoot } else { $nativeExe }
    runtime_shim = Get-RepoRelativePath -Path $runtimeShimSource -Root $repoRoot
    clang = $clangCommand
    total = $total
    passed = $passedCount
    failed = $failedCount
    status = if ($failedCount -eq 0) { "PASS" } else { "FAIL" }
    results = $results
  }
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
  Write-Output "status: PASS"
}
finally {
  Pop-Location
}
