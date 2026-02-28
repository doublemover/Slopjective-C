$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/typed-abi-replay-proof"
$configuredRunId = $env:OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID
$defaultRunId = "m143-lane-c-typed-abi-default"

function Resolve-ValidatedRunId {
  param(
    [Parameter()][string]$ConfiguredRunId,
    [Parameter(Mandatory = $true)][string]$DefaultRunId
  )

  if ([string]::IsNullOrWhiteSpace($ConfiguredRunId)) {
    return $DefaultRunId
  }

  $candidate = $ConfiguredRunId.Trim()
  if ($candidate.Length -gt 80) {
    throw "typed-abi replay FAIL: configured run id exceeds 80 characters"
  }
  if ($candidate -notmatch '^[A-Za-z0-9_-]+$') {
    throw "typed-abi replay FAIL: configured run id must match ^[A-Za-z0-9_-]+$"
  }
  return $candidate
}

$runId = Resolve-ValidatedRunId -ConfiguredRunId $configuredRunId -DefaultRunId $defaultRunId
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$positiveFixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/positive"

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

function Get-ExpectationTokens {
  param([Parameter(Mandatory = $true)][string]$ExpectationPath)

  if (!(Test-Path -LiteralPath $ExpectationPath -PathType Leaf)) {
    throw "typed-abi replay FAIL: missing expectation file $ExpectationPath"
  }

  $tokens = New-Object 'System.Collections.Generic.List[string]'
  $tokenSet = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::Ordinal)
  $lineNo = 0
  foreach ($line in Get-Content -LiteralPath $ExpectationPath) {
    $lineNo++
    $trimmed = $line.Trim()
    if ([string]::IsNullOrWhiteSpace($trimmed)) {
      continue
    }
    if ($trimmed.StartsWith("#", [System.StringComparison]::Ordinal)) {
      continue
    }
    if (-not $tokenSet.Add($trimmed)) {
      throw "typed-abi replay FAIL: duplicate expectation token at line $lineNo in $ExpectationPath"
    }
    $null = $tokens.Add($trimmed)
  }
  if ($tokens.Count -eq 0) {
    throw "typed-abi replay FAIL: expectation file has no tokens: $ExpectationPath"
  }
  return @($tokens.ToArray())
}

function Get-MissingTokens {
  param(
    [Parameter(Mandatory = $true)][string]$Text,
    [Parameter(Mandatory = $true)][string[]]$Tokens
  )

  $missing = New-Object 'System.Collections.Generic.List[string]'
  foreach ($token in $Tokens) {
    if ($Text.IndexOf($token, [System.StringComparison]::Ordinal) -lt 0) {
      $null = $missing.Add($token)
    }
  }
  return @($missing.ToArray())
}

function Get-FixturePathFromExpectation {
  param([Parameter(Mandatory = $true)][string]$ExpectationPath)

  $suffix = ".objc3-ir.expect.txt"
  if (-not $ExpectationPath.EndsWith($suffix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "typed-abi replay FAIL: unsupported expectation filename $ExpectationPath (expected suffix $suffix)"
  }
  $root = $ExpectationPath.Substring(0, $ExpectationPath.Length - $suffix.Length)
  return "$root.objc3"
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

New-Item -ItemType Directory -Force -Path $runDir | Out-Null
Push-Location $repoRoot
try {
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $buildScript
    if ($LASTEXITCODE -ne 0) {
      throw "typed-abi replay FAIL: build failed with exit $LASTEXITCODE"
    }
  }

  if (!(Test-Path -LiteralPath $positiveFixtureDir -PathType Container)) {
    throw "typed-abi replay FAIL: missing positive fixture directory $positiveFixtureDir"
  }

  $expectationFiles = @(
    Get-ChildItem -LiteralPath $positiveFixtureDir -Recurse -File -Filter "*.objc3-ir.expect.txt" |
      Sort-Object -Property FullName
  )
  if ($expectationFiles.Count -eq 0) {
    throw "typed-abi replay FAIL: no .objc3-ir.expect.txt files found under $(Get-RepoRelativePath -Path $positiveFixtureDir -Root $repoRoot)"
  }

  $fixtureRoots = @()
  foreach ($expectationFile in $expectationFiles) {
    $fixturePath = Get-FixturePathFromExpectation -ExpectationPath $expectationFile.FullName
    if (!(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
      throw "typed-abi replay FAIL: missing fixture for expectation $(Get-RepoRelativePath -Path $expectationFile.FullName -Root $repoRoot)"
    }
    $fixtureRoots += (Get-RepoRelativePath -Path $fixturePath -Root $repoRoot)
  }
  $fixtureRoots = @($fixtureRoots | Sort-Object -Unique)

  $results = @()
  foreach ($fixtureRel in $fixtureRoots) {
    $fixturePath = Join-Path $repoRoot $fixtureRel
    if (!(Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
      throw "typed-abi replay FAIL: missing fixture $fixtureRel"
    }

    $expectPath = [System.IO.Path]::ChangeExtension($fixturePath, ".objc3-ir.expect.txt")
    $tokens = Get-ExpectationTokens -ExpectationPath $expectPath
    $slug = "$(Get-ShortHash -Value $fixtureRel)_$([System.IO.Path]::GetFileNameWithoutExtension($fixturePath))"
    $caseDir = Join-Path $runDir $slug
    $run1Dir = Join-Path $caseDir "run1"
    $run2Dir = Join-Path $caseDir "run2"
    New-Item -ItemType Directory -Force -Path $run1Dir | Out-Null
    New-Item -ItemType Directory -Force -Path $run2Dir | Out-Null

    $run1Exit = Invoke-LoggedCommand `
      -Command $exe `
      -Arguments @($fixturePath, "--out-dir", $run1Dir, "--emit-prefix", "module") `
      -LogPath (Join-Path $caseDir "run1.log")
    $run2Exit = Invoke-LoggedCommand `
      -Command $exe `
      -Arguments @($fixturePath, "--out-dir", $run2Dir, "--emit-prefix", "module") `
      -LogPath (Join-Path $caseDir "run2.log")
    if ($run1Exit -ne 0 -or $run2Exit -ne 0) {
      throw "typed-abi replay FAIL: compile failed for $fixtureRel (run1=$run1Exit run2=$run2Exit)"
    }

    $run1LlPath = Join-Path $run1Dir "module.ll"
    $run2LlPath = Join-Path $run2Dir "module.ll"
    if (!(Test-Path -LiteralPath $run1LlPath -PathType Leaf) -or !(Test-Path -LiteralPath $run2LlPath -PathType Leaf)) {
      throw "typed-abi replay FAIL: missing module.ll for $fixtureRel"
    }
    $run1Ll = Get-Content -LiteralPath $run1LlPath -Raw
    $run2Ll = Get-Content -LiteralPath $run2LlPath -Raw
    if ($run1Ll -ne $run2Ll) {
      throw "typed-abi replay FAIL: LLVM IR drift across replay for $fixtureRel"
    }

    $missingRun1 = @(Get-MissingTokens -Text $run1Ll -Tokens $tokens)
    $missingRun2 = @(Get-MissingTokens -Text $run2Ll -Tokens $tokens)
    if ($missingRun1.Count -gt 0 -or $missingRun2.Count -gt 0) {
      throw "typed-abi replay FAIL: expectation token mismatch for $fixtureRel (run1_missing=$($missingRun1 -join '|') run2_missing=$($missingRun2 -join '|'))"
    }

    $results += [pscustomobject]@{
      fixture = $fixtureRel
      expectation = Get-RepoRelativePath -Path $expectPath -Root $repoRoot
      token_count = $tokens.Count
      replay_ir_deterministic = $true
      expectations_match = $true
    }
    Write-Output "[PASS] $fixtureRel"
  }

  $summary = [ordered]@{
    schema_version = "1.0.0"
    suite = "objc3c-native-typed-abi-replay-proof"
    generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
    status = "PASS"
    total = $results.Count
    passed = $results.Count
    failed = 0
    results = $results
  }
  $summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8
  Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
  Write-Output "status: PASS"
}
finally {
  Pop-Location
}
