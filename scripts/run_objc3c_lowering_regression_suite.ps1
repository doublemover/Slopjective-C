$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$positiveRecoveryFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive"
$optionalDispatchFixtureDirs = @(
  (Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive/lowering_dispatch"),
  (Join-Path $repoRoot "tests/tooling/fixtures/native/dispatch/positive")
)
$negativeFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/lowering-regression"
$configuredRunId = $env:OBJC3C_NATIVE_LOWERING_RUN_ID
$runId = if ([string]::IsNullOrWhiteSpace($configuredRunId)) { Get-Date -Format "yyyyMMdd_HHmmss_fff" } else { $configuredRunId }
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$deterministicSummaryPath = Join-Path $suiteRoot "latest-summary.json"
$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$configuredClangPath = $env:OBJC3C_NATIVE_LOWERING_CLANG_PATH
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
    [Parameter(Mandatory = $true)][string[]]$Arguments,
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

function Test-ByteArrayEqual {
  param(
    [byte[]]$Left,
    [byte[]]$Right
  )

  if ($null -eq $Left -or $null -eq $Right) {
    return $false
  }
  if ($Left.Length -ne $Right.Length) {
    return $false
  }
  for ($i = 0; $i -lt $Left.Length; $i++) {
    if ($Left[$i] -ne $Right[$i]) {
      return $false
    }
  }
  return $true
}

function Get-FileBytesOrNull {
  param([Parameter(Mandatory = $true)][string]$Path)
  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    return $null
  }
  return ,([System.IO.File]::ReadAllBytes($Path))
}

function Get-FileTextOrEmpty {
  param([Parameter(Mandatory = $true)][string]$Path)

  if (!(Test-Path -LiteralPath $Path -PathType Leaf)) {
    return ""
  }
  return Get-Content -LiteralPath $Path -Raw
}

function Get-DispatchIrExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectationPath = [System.IO.Path]::ChangeExtension($FixturePath, ".dispatch-ir.expect.txt")
  if (!(Test-Path -LiteralPath $expectationPath -PathType Leaf)) {
    return [pscustomobject]@{
      enabled = $false
      path = $expectationPath
      tokens = @()
      parse_error = ""
    }
  }

  $tokens = New-Object 'System.Collections.Generic.List[string]'
  $parseErrors = New-Object 'System.Collections.Generic.List[string]'
  $tokenSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
  $lineNumber = 0
  foreach ($line in Get-Content -LiteralPath $expectationPath) {
    $lineNumber++
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed.StartsWith("#", [System.StringComparison]::Ordinal)) {
      continue
    }
    if (-not $tokenSet.Add($trimmed)) {
      $null = $parseErrors.Add(("duplicate token at line {0}: {1}" -f $lineNumber, $trimmed))
      continue
    }
    $null = $tokens.Add($trimmed)
  }

  if ($tokens.Count -eq 0) {
    $null = $parseErrors.Add("dispatch IR expectation file has no tokens")
  }

  $parseError = if ($parseErrors.Count -gt 0) { $parseErrors -join "; " } else { "" }

  return [pscustomobject]@{
    enabled = $true
    path = $expectationPath
    tokens = @($tokens.ToArray())
    parse_error = $parseError
  }
}

function Get-Objc3IrExpectation {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $expectationPath = [System.IO.Path]::ChangeExtension($FixturePath, ".objc3-ir.expect.txt")
  if (!(Test-Path -LiteralPath $expectationPath -PathType Leaf)) {
    return [pscustomobject]@{
      enabled = $false
      path = $expectationPath
      tokens = @()
      parse_error = ""
    }
  }

  $tokens = New-Object 'System.Collections.Generic.List[string]'
  $parseErrors = New-Object 'System.Collections.Generic.List[string]'
  $tokenSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
  $lineNumber = 0
  foreach ($line in Get-Content -LiteralPath $expectationPath) {
    $lineNumber++
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed.StartsWith("#", [System.StringComparison]::Ordinal)) {
      continue
    }
    if (-not $tokenSet.Add($trimmed)) {
      $null = $parseErrors.Add(("duplicate token at line {0}: {1}" -f $lineNumber, $trimmed))
      continue
    }
    $null = $tokens.Add($trimmed)
  }

  if ($tokens.Count -eq 0) {
    $null = $parseErrors.Add("objc3 IR expectation file has no tokens")
  }

  $parseError = if ($parseErrors.Count -gt 0) { $parseErrors -join "; " } else { "" }

  return [pscustomobject]@{
    enabled = $true
    path = $expectationPath
    tokens = @($tokens.ToArray())
    parse_error = $parseError
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

function Get-Fixtures {
  param(
    [Parameter(Mandatory = $true)][string]$Directory,
    [Parameter(Mandatory = $true)][string]$FixtureKind,
    [Parameter(Mandatory = $true)][string[]]$Extensions
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "lowering regression FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File |
      Where-Object { $_.Extension -in $Extensions } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "lowering regression FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return $fixtures
}

function Invoke-LoweringCase {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][ValidateSet("positive", "negative")][string]$FixtureKind
  )

  $fixtureRel = Get-RepoRelativePath -Path $Fixture.FullName -Root $repoRoot
  $fixtureHash = Get-ShortHash -Value $fixtureRel
  $caseDir = Join-Path $runDir ("{0}_{1}_{2}" -f $FixtureKind, $fixtureHash, $Fixture.BaseName)
  $run1Dir = Join-Path $caseDir "run1"
  $run2Dir = Join-Path $caseDir "run2"
  $run1Log = Join-Path $caseDir "run1.log"
  $run2Log = Join-Path $caseDir "run2.log"

  New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
  New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null

  $run1Exit = Invoke-LoggedCommand `
    -Command $exe `
    -Arguments @($Fixture.FullName, "--out-dir", $run1Dir, "--emit-prefix", "module") `
    -LogPath $run1Log
  $run2Exit = Invoke-LoggedCommand `
    -Command $exe `
    -Arguments @($Fixture.FullName, "--out-dir", $run2Dir, "--emit-prefix", "module") `
    -LogPath $run2Log

  $run1ObjPath = Join-Path $run1Dir "module.obj"
  $run2ObjPath = Join-Path $run2Dir "module.obj"
  $run1ObjExists = Test-Path -LiteralPath $run1ObjPath -PathType Leaf
  $run2ObjExists = Test-Path -LiteralPath $run2ObjPath -PathType Leaf
  $run1ObjBytes = if ($run1ObjExists) { (Get-Item -LiteralPath $run1ObjPath).Length } else { 0 }
  $run2ObjBytes = if ($run2ObjExists) { (Get-Item -LiteralPath $run2ObjPath).Length } else { 0 }

  $manifest1Bytes = Get-FileBytesOrNull -Path (Join-Path $run1Dir "module.manifest.json")
  $manifest2Bytes = Get-FileBytesOrNull -Path (Join-Path $run2Dir "module.manifest.json")
  $diag1Bytes = Get-FileBytesOrNull -Path (Join-Path $run1Dir "module.diagnostics.txt")
  $diag2Bytes = Get-FileBytesOrNull -Path (Join-Path $run2Dir "module.diagnostics.txt")
  $ll1Bytes = Get-FileBytesOrNull -Path (Join-Path $run1Dir "module.ll")
  $ll2Bytes = Get-FileBytesOrNull -Path (Join-Path $run2Dir "module.ll")
  $llPresentRun1 = $null -ne $ll1Bytes
  $llPresentRun2 = $null -ne $ll2Bytes

  $dispatchSpec = Get-DispatchIrExpectation -FixturePath $Fixture.FullName
  $dispatchExpectationEnabled = $dispatchSpec.enabled
  $dispatchExpectationPath = $dispatchSpec.path
  $dispatchExpectationTokens = @($dispatchSpec.tokens)
  $dispatchExpectationParseError = $dispatchSpec.parse_error
  if ($dispatchExpectationEnabled -and ($Fixture.Extension -ine ".m")) {
    $dispatchExpectationParseError = "dispatch IR expectations require Objective-C .m fixtures"
  }

  $dispatchExpectationPathRel = ""
  if ($dispatchExpectationEnabled) {
    $dispatchExpectationPathRel = Get-RepoRelativePath -Path $dispatchExpectationPath -Root $repoRoot
  }

  $objc3Spec = Get-Objc3IrExpectation -FixturePath $Fixture.FullName
  $objc3ExpectationEnabled = $objc3Spec.enabled
  $objc3ExpectationPath = $objc3Spec.path
  $objc3ExpectationTokens = @($objc3Spec.tokens)
  $objc3ExpectationParseError = $objc3Spec.parse_error
  if ($objc3ExpectationEnabled -and ($Fixture.Extension -ine ".objc3")) {
    $objc3ExpectationParseError = "objc3 IR expectations require .objc3 fixtures"
  }
  $objc3ExpectationPathRel = ""
  if ($objc3ExpectationEnabled) {
    $objc3ExpectationPathRel = Get-RepoRelativePath -Path $objc3ExpectationPath -Root $repoRoot
  }
  $objc3IrPresent = $true
  $objc3IrDeterministic = $true
  $objc3IrExpectationsMatch = $true
  $objc3Run1Missing = @()
  $objc3Run2Missing = @()
  if ($objc3ExpectationEnabled -and [string]::IsNullOrWhiteSpace($objc3ExpectationParseError)) {
    $objc3IrPresent = $llPresentRun1 -and $llPresentRun2
    if ($objc3IrPresent) {
      $objc3IrDeterministic = Test-ByteArrayEqual -Left $ll1Bytes -Right $ll2Bytes
      $objc3Run1Text = Get-FileTextOrEmpty -Path (Join-Path $run1Dir "module.ll")
      $objc3Run2Text = Get-FileTextOrEmpty -Path (Join-Path $run2Dir "module.ll")
      $objc3Run1Missing = @(Get-MissingTokens -Text $objc3Run1Text -Tokens $objc3ExpectationTokens)
      $objc3Run2Missing = @(Get-MissingTokens -Text $objc3Run2Text -Tokens $objc3ExpectationTokens)
      $objc3IrExpectationsMatch = ($objc3Run1Missing.Count -eq 0) -and ($objc3Run2Missing.Count -eq 0)
    }
    else {
      $objc3IrDeterministic = $false
      $objc3IrExpectationsMatch = $false
    }
  }

  $dispatchRun1Path = Join-Path $run1Dir "module.dispatch.ll"
  $dispatchRun2Path = Join-Path $run2Dir "module.dispatch.ll"
  $dispatchRun1Log = Join-Path $caseDir "run1.dispatch-ir.log"
  $dispatchRun2Log = Join-Path $caseDir "run2.dispatch-ir.log"
  $dispatchRun1PathRel = ""
  $dispatchRun2PathRel = ""
  $dispatchRun1Exit = 0
  $dispatchRun2Exit = 0
  $dispatchIrCompileSuccess = $true
  $dispatchIrPresent = $true
  $dispatchIrDeterministic = $true
  $dispatchIrExpectationsMatch = $true
  $dispatchRun1Missing = @()
  $dispatchRun2Missing = @()

  if ($dispatchExpectationEnabled -and [string]::IsNullOrWhiteSpace($dispatchExpectationParseError)) {
    $dispatchRun1Exit = Invoke-LoggedCommand `
      -Command $clangCommand `
      -Arguments @("-x", "objective-c", "-std=gnu11", "-S", "-emit-llvm", $Fixture.FullName, "-o", $dispatchRun1Path) `
      -LogPath $dispatchRun1Log
    $dispatchRun2Exit = Invoke-LoggedCommand `
      -Command $clangCommand `
      -Arguments @("-x", "objective-c", "-std=gnu11", "-S", "-emit-llvm", $Fixture.FullName, "-o", $dispatchRun2Path) `
      -LogPath $dispatchRun2Log

    $dispatchIrCompileSuccess = ($dispatchRun1Exit -eq 0) -and ($dispatchRun2Exit -eq 0)

    if ($dispatchIrCompileSuccess) {
      $dispatchRun1Bytes = Get-FileBytesOrNull -Path $dispatchRun1Path
      $dispatchRun2Bytes = Get-FileBytesOrNull -Path $dispatchRun2Path
      $dispatchRun1Present = $null -ne $dispatchRun1Bytes
      $dispatchRun2Present = $null -ne $dispatchRun2Bytes
      $dispatchIrPresent = $dispatchRun1Present -and $dispatchRun2Present
      if ($dispatchIrPresent) {
        $dispatchIrDeterministic = Test-ByteArrayEqual -Left $dispatchRun1Bytes -Right $dispatchRun2Bytes
        $dispatchRun1Text = Get-FileTextOrEmpty -Path $dispatchRun1Path
        $dispatchRun2Text = Get-FileTextOrEmpty -Path $dispatchRun2Path
        $dispatchRun1Missing = @(Get-MissingTokens -Text $dispatchRun1Text -Tokens $dispatchExpectationTokens)
        $dispatchRun2Missing = @(Get-MissingTokens -Text $dispatchRun2Text -Tokens $dispatchExpectationTokens)
        $dispatchIrExpectationsMatch = ($dispatchRun1Missing.Count -eq 0) -and ($dispatchRun2Missing.Count -eq 0)
      }
      else {
        $dispatchIrDeterministic = $false
        $dispatchIrExpectationsMatch = $false
      }
    }
    else {
      $dispatchIrPresent = $false
      $dispatchIrDeterministic = $false
      $dispatchIrExpectationsMatch = $false
    }
  }

  if (Test-Path -LiteralPath $dispatchRun1Path -PathType Leaf) {
    $dispatchRun1PathRel = Get-RepoRelativePath -Path $dispatchRun1Path -Root $repoRoot
  }
  if (Test-Path -LiteralPath $dispatchRun2Path -PathType Leaf) {
    $dispatchRun2PathRel = Get-RepoRelativePath -Path $dispatchRun2Path -Root $repoRoot
  }

  $exitCodeDeterministic = $run1Exit -eq $run2Exit
  $diagnosticsDeterministic = ($null -ne $diag1Bytes) -and ($null -ne $diag2Bytes) -and (Test-ByteArrayEqual -Left $diag1Bytes -Right $diag2Bytes)

  $llPresent = $llPresentRun1 -or $llPresentRun2
  if ($llPresent) {
    $llDeterministic = $llPresentRun1 -and $llPresentRun2 -and (Test-ByteArrayEqual -Left $ll1Bytes -Right $ll2Bytes)
  }
  else {
    $llDeterministic = $true
  }

  $failedChecks = New-Object 'System.Collections.Generic.List[string]'
  $checks = [ordered]@{
    exit_code_deterministic = $exitCodeDeterministic
    diagnostics_deterministic = $diagnosticsDeterministic
    dispatch_ir_expectations_enabled = $dispatchExpectationEnabled
    dispatch_ir_expectation_valid = [string]::IsNullOrWhiteSpace($dispatchExpectationParseError)
  }

  if ($dispatchExpectationEnabled) {
    $checks.dispatch_ir_expectation_token_count = $dispatchExpectationTokens.Count
    $checks.dispatch_ir_compile_success = $dispatchIrCompileSuccess
    $checks.dispatch_ir_present = $dispatchIrPresent
    $checks.dispatch_ir_deterministic = $dispatchIrDeterministic
    $checks.dispatch_ir_expectations_match = $dispatchIrExpectationsMatch
  }
  $checks.objc3_ir_expectations_enabled = $objc3ExpectationEnabled
  $checks.objc3_ir_expectation_valid = [string]::IsNullOrWhiteSpace($objc3ExpectationParseError)
  if ($objc3ExpectationEnabled) {
    $checks.objc3_ir_expectation_token_count = $objc3ExpectationTokens.Count
    $checks.objc3_ir_present = $objc3IrPresent
    $checks.objc3_ir_deterministic = $objc3IrDeterministic
    $checks.objc3_ir_expectations_match = $objc3IrExpectationsMatch
  }

  if ($FixtureKind -eq "positive") {
    $compileSuccess = ($run1Exit -eq 0) -and ($run2Exit -eq 0)
    $objNonEmpty = $run1ObjExists -and $run2ObjExists -and ($run1ObjBytes -gt 0) -and ($run2ObjBytes -gt 0)
    $manifestDeterministic = ($null -ne $manifest1Bytes) -and ($null -ne $manifest2Bytes) -and (Test-ByteArrayEqual -Left $manifest1Bytes -Right $manifest2Bytes)

    $checks.compile_success = $compileSuccess
    $checks.obj_non_empty = $objNonEmpty
    $checks.manifest_deterministic = $manifestDeterministic
    $checks.ll_present = $llPresent
    $checks.ll_deterministic = $llDeterministic

    if (-not $compileSuccess) {
      $failedChecks.Add("compile_success")
    }
    if (-not $objNonEmpty) {
      $failedChecks.Add("obj_non_empty")
    }
    if (-not $manifestDeterministic) {
      $failedChecks.Add("manifest_deterministic")
    }
    if ($llPresent -and -not $llDeterministic) {
      $failedChecks.Add("ll_deterministic")
    }

    if ($dispatchExpectationEnabled -and -not $dispatchIrCompileSuccess) {
      $failedChecks.Add("dispatch_ir_compile_success")
    }
    if ($dispatchExpectationEnabled -and -not $dispatchIrPresent) {
      $failedChecks.Add("dispatch_ir_present")
    }
    if ($dispatchExpectationEnabled -and -not $dispatchIrDeterministic) {
      $failedChecks.Add("dispatch_ir_deterministic")
    }
    if ($dispatchExpectationEnabled -and -not $dispatchIrExpectationsMatch) {
      $failedChecks.Add("dispatch_ir_expectations_match")
    }
    if ($objc3ExpectationEnabled -and -not $objc3IrPresent) {
      $failedChecks.Add("objc3_ir_present")
    }
    if ($objc3ExpectationEnabled -and -not $objc3IrDeterministic) {
      $failedChecks.Add("objc3_ir_deterministic")
    }
    if ($objc3ExpectationEnabled -and -not $objc3IrExpectationsMatch) {
      $failedChecks.Add("objc3_ir_expectations_match")
    }
  }
  else {
    $compileFails = ($run1Exit -ne 0) -and ($run2Exit -ne 0)
    $diag1Text = Get-FileTextOrEmpty -Path (Join-Path $run1Dir "module.diagnostics.txt")
    $diag2Text = Get-FileTextOrEmpty -Path (Join-Path $run2Dir "module.diagnostics.txt")
    $diagnosticsPopulated =
      (-not [string]::IsNullOrWhiteSpace($diag1Text)) -and
      (-not [string]::IsNullOrWhiteSpace($diag2Text))
    $manifestAbsent = ($null -eq $manifest1Bytes) -and ($null -eq $manifest2Bytes)
    $objAbsent = (-not $run1ObjExists) -and (-not $run2ObjExists)
    $llAbsent = ($null -eq $ll1Bytes) -and ($null -eq $ll2Bytes)

    $checks.compile_fails = $compileFails
    $checks.diagnostics_populated = $diagnosticsPopulated
    $checks.manifest_absent = $manifestAbsent
    $checks.obj_absent = $objAbsent
    $checks.ll_absent = $llAbsent

    if (-not $compileFails) {
      $failedChecks.Add("compile_fails")
    }
    if (-not $diagnosticsPopulated) {
      $failedChecks.Add("diagnostics_populated")
    }
    if (-not $manifestAbsent) {
      $failedChecks.Add("manifest_absent")
    }
    if (-not $objAbsent) {
      $failedChecks.Add("obj_absent")
    }
    if (-not $llAbsent) {
      $failedChecks.Add("ll_absent")
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($dispatchExpectationParseError)) {
    $failedChecks.Add("dispatch_ir_expectation_valid")
  }
  if (-not [string]::IsNullOrWhiteSpace($objc3ExpectationParseError)) {
    $failedChecks.Add("objc3_ir_expectation_valid")
  }

  if (-not $exitCodeDeterministic) {
    $failedChecks.Add("exit_code_deterministic")
  }
  if (-not $diagnosticsDeterministic) {
    $failedChecks.Add("diagnostics_deterministic")
  }

  $passed = $failedChecks.Count -eq 0
  if ($passed) {
    $detail = "all checks passed"
  }
  else {
    $detailSegments = New-Object 'System.Collections.Generic.List[string]'
    $null = $detailSegments.Add(($failedChecks -join ","))
    if ($dispatchExpectationEnabled -and -not $dispatchIrExpectationsMatch) {
      if ($dispatchRun1Missing.Count -gt 0) {
        $null = $detailSegments.Add(("dispatch_run1_missing={0}" -f ($dispatchRun1Missing -join "|")))
      }
      if ($dispatchRun2Missing.Count -gt 0) {
        $null = $detailSegments.Add(("dispatch_run2_missing={0}" -f ($dispatchRun2Missing -join "|")))
      }
    }
    if ($objc3ExpectationEnabled -and -not $objc3IrExpectationsMatch) {
      if ($objc3Run1Missing.Count -gt 0) {
        $null = $detailSegments.Add(("objc3_run1_missing={0}" -f ($objc3Run1Missing -join "|")))
      }
      if ($objc3Run2Missing.Count -gt 0) {
        $null = $detailSegments.Add(("objc3_run2_missing={0}" -f ($objc3Run2Missing -join "|")))
      }
    }
    if (-not [string]::IsNullOrWhiteSpace($dispatchExpectationParseError)) {
      $null = $detailSegments.Add(("dispatch_expectation_error={0}" -f $dispatchExpectationParseError))
    }
    if (-not [string]::IsNullOrWhiteSpace($objc3ExpectationParseError)) {
      $null = $detailSegments.Add(("objc3_expectation_error={0}" -f $objc3ExpectationParseError))
    }
    $detail = $detailSegments -join "; "
  }

  return [pscustomobject]@{
    kind = $FixtureKind
    fixture = $fixtureRel
    passed = $passed
    detail = $detail
    case_dir = (Get-RepoRelativePath -Path $caseDir -Root $repoRoot)
    run1_exit_code = $run1Exit
    run2_exit_code = $run2Exit
    run1_obj_bytes = $run1ObjBytes
    run2_obj_bytes = $run2ObjBytes
    dispatch_ir_expectation = $dispatchExpectationPathRel
    dispatch_ir_run1 = $dispatchRun1PathRel
    dispatch_ir_run2 = $dispatchRun2PathRel
    dispatch_ir_expected_tokens = $dispatchExpectationTokens
    dispatch_ir_run1_missing_tokens = $dispatchRun1Missing
    dispatch_ir_run2_missing_tokens = $dispatchRun2Missing
    objc3_ir_expectation = $objc3ExpectationPathRel
    objc3_ir_expected_tokens = $objc3ExpectationTokens
    objc3_ir_run1_missing_tokens = $objc3Run1Missing
    objc3_ir_run2_missing_tokens = $objc3Run2Missing
    checks = $checks
  }
}

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

$startedAtUtc = (Get-Date).ToUniversalTime().ToString("o")
$results = @()
$fatalError = ""
$fixtureRoots = @(
  (Get-RepoRelativePath -Path $positiveRecoveryFixtureDir -Root $repoRoot),
  (Get-RepoRelativePath -Path $negativeFixtureDir -Root $repoRoot)
)

Push-Location $repoRoot
try {
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    $buildLog = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
      -LogPath $buildLog
    if ($buildExit -ne 0) {
      throw "lowering regression FAIL: build failed with exit code $buildExit"
    }
  }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "lowering regression FAIL: native compiler executable missing at $exe"
  }
  if ([System.IO.Path]::IsPathRooted($clangCommand) -and !(Test-Path -LiteralPath $clangCommand -PathType Leaf)) {
    throw "lowering regression FAIL: clang executable missing at $clangCommand"
  }

  $positiveFixtures = @(
    Get-Fixtures -Directory $positiveRecoveryFixtureDir -FixtureKind "positive recovery" -Extensions @(".objc3")
  )
  $dispatchFixtures = @()
  foreach ($dispatchFixtureDir in $optionalDispatchFixtureDirs) {
    if (!(Test-Path -LiteralPath $dispatchFixtureDir -PathType Container)) {
      Write-Output ("[INFO] optional dispatch fixture directory not found: {0}" -f $dispatchFixtureDir)
      continue
    }

    $fixtureRoots += (Get-RepoRelativePath -Path $dispatchFixtureDir -Root $repoRoot)
    $discoveredFixtures = @(
      Get-ChildItem -LiteralPath $dispatchFixtureDir -Recurse -File |
        Where-Object { $_.Extension -ieq ".m" } |
        Sort-Object -Property FullName
    )
    if ($discoveredFixtures.Count -eq 0) {
      Write-Output ("[INFO] optional dispatch fixture directory has no .m fixtures: {0}" -f $dispatchFixtureDir)
      continue
    }

    $dispatchFixtures += $discoveredFixtures
  }

  if ($dispatchFixtures.Count -gt 0) {
    $positiveFixtures += $dispatchFixtures
  }

  $positiveFixtures = @($positiveFixtures | Sort-Object -Property FullName -Unique)
  $negativeFixtures = Get-Fixtures -Directory $negativeFixtureDir -FixtureKind "negative recovery" -Extensions @(".objc3")
  $fixtureRoots = @($fixtureRoots | Select-Object -Unique)

  foreach ($fixture in $positiveFixtures) {
    $caseResult = Invoke-LoweringCase -Fixture $fixture -FixtureKind "positive"
    $results += $caseResult
    $statusToken = if ($caseResult.passed) { "PASS" } else { "FAIL" }
    if ($caseResult.passed) {
      Write-Output ("[{0}] positive {1}" -f $statusToken, $caseResult.fixture)
    }
    else {
      Write-Output ("[{0}] positive {1} ({2})" -f $statusToken, $caseResult.fixture, $caseResult.detail)
    }
  }

  foreach ($fixture in $negativeFixtures) {
    $caseResult = Invoke-LoweringCase -Fixture $fixture -FixtureKind "negative"
    $results += $caseResult
    $statusToken = if ($caseResult.passed) { "PASS" } else { "FAIL" }
    if ($caseResult.passed) {
      Write-Output ("[{0}] negative {1}" -f $statusToken, $caseResult.fixture)
    }
    else {
      Write-Output ("[{0}] negative {1} ({2})" -f $statusToken, $caseResult.fixture, $caseResult.detail)
    }
  }
}
catch {
  $fatalError = $_.Exception.Message
  Write-Output ("error: {0}" -f $fatalError)
}
finally {
  Pop-Location
}

$total = $results.Count
$passedCount = @($results | Where-Object { $_.passed }).Count
$failedCount = $total - $passedCount
$status = if ([string]::IsNullOrWhiteSpace($fatalError) -and $total -gt 0 -and $failedCount -eq 0) { "PASS" } else { "FAIL" }

$summary = [ordered]@{
  run_id = $runId
  started_at_utc = $startedAtUtc
  completed_at_utc = (Get-Date).ToUniversalTime().ToString("o")
  run_dir = (Get-RepoRelativePath -Path $runDir -Root $repoRoot)
  fixture_roots = $fixtureRoots
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  fatal_error = $fatalError
  results = $results
}
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

$deterministicResults = @(
  $results |
    Sort-Object -Property kind, fixture |
    ForEach-Object {
      [ordered]@{
        kind = $_.kind
        fixture = $_.fixture
        passed = $_.passed
        detail = $_.detail
        run1_exit_code = $_.run1_exit_code
        run2_exit_code = $_.run2_exit_code
        checks = $_.checks
      }
    }
)

$deterministicSummary = [ordered]@{
  fixture_roots = $fixtureRoots
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  fatal_error = $fatalError
  results = $deterministicResults
}
$deterministicSummary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $deterministicSummaryPath -Encoding utf8

Write-Output ("summary: total={0} passed={1} failed={2}" -f $total, $passedCount, $failedCount)
Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $summaryPath -Root $repoRoot))
Write-Output ("deterministic_summary_path: {0}" -f (Get-RepoRelativePath -Path $deterministicSummaryPath -Root $repoRoot))
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
