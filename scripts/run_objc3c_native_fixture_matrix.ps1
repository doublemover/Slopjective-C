param(
  [string]$FixtureList = "",
  [string]$FixtureGlob = "",
  [int]$ShardIndex = -1,
  [int]$ShardCount = 0,
  [int]$Limit = 0
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$recoveryRoot = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery"
$positiveDir = Join-Path $recoveryRoot "positive"
$runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$matrixRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/fixture-matrix"
$runDir = Join-Path $matrixRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$hadFatalError = $false
$fatalErrorMessage = ""
$selectedPositiveCount = 0

function Get-RepoRelativePath {
  param(
    [string]$Path,
    [string]$Root
  )

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  if ($fullPath.StartsWith($fullRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($fullRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-ShortHash {
  param([string]$Value)

  $sha1 = [System.Security.Cryptography.SHA1]::Create()
  try {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Value)
    $hashBytes = $sha1.ComputeHash($bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant().Substring(0, 10)
  } finally {
    $sha1.Dispose()
  }
}

function Get-Fixtures {
  param(
    [string]$Directory,
    [string]$FixtureKind
  )

  if (!(Test-Path -LiteralPath $Directory -PathType Container)) {
    throw "matrix FAIL: missing $FixtureKind fixture directory at $Directory"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $Directory -Recurse -File |
      Where-Object { $_.Extension -in @(".objc3", ".m") } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "matrix FAIL: no $FixtureKind fixtures found in $Directory"
  }

  return $fixtures
}

function Get-RequestedRelativePaths {
  param([string]$FixtureListPath)

  $resolvedFixtureList = if ([System.IO.Path]::IsPathRooted($FixtureListPath)) { $FixtureListPath } else { Join-Path $repoRoot $FixtureListPath }
  if (!(Test-Path -LiteralPath $resolvedFixtureList -PathType Leaf)) {
    throw "matrix FAIL: missing fixture list at $resolvedFixtureList"
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

function Select-Fixtures {
  param(
    [object[]]$Fixtures,
    [string]$FixtureListPath,
    [string]$FixtureGlobPattern,
    [int]$ShardIndexValue,
    [int]$ShardCountValue,
    [int]$LimitValue
  )

  if ($LimitValue -lt 0) {
    throw "matrix FAIL: limit must be non-negative"
  }
  if ($ShardCountValue -lt 0) {
    throw "matrix FAIL: shard-count must be non-negative"
  }
  if (($ShardIndexValue -ge 0) -and ($ShardCountValue -le 0)) {
    throw "matrix FAIL: shard-index requires shard-count > 0"
  }
  if (($ShardCountValue -gt 0) -and (($ShardIndexValue -lt 0) -or ($ShardIndexValue -ge $ShardCountValue))) {
    throw "matrix FAIL: shard-index must satisfy 0 <= shard-index < shard-count"
  }

  $selected = @($Fixtures)

  if (-not [string]::IsNullOrWhiteSpace($FixtureListPath)) {
    $requested = Get-RequestedRelativePaths -FixtureListPath $FixtureListPath
    $selected = @($selected | Where-Object { $requested.Contains((Get-RepoRelativePath -Path $_.FullName -Root $repoRoot)) })
    $matched = [System.Collections.Generic.HashSet[string]]::new([System.StringComparer]::OrdinalIgnoreCase)
    foreach ($fixture in $selected) {
      $null = $matched.Add((Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot))
    }
    $missing = @()
    foreach ($requestedPath in $requested) {
      if (-not $matched.Contains($requestedPath)) {
        $missing += $requestedPath
      }
    }
    if ($missing.Count -gt 0) {
      throw "matrix FAIL: fixture-list entries did not match matrix fixtures ($($missing -join ', '))"
    }
  }

  if (-not [string]::IsNullOrWhiteSpace($FixtureGlobPattern)) {
    $pattern = [System.Management.Automation.WildcardPattern]::new(
      $FixtureGlobPattern.Replace('\', '/'),
      [System.Management.Automation.WildcardOptions]::IgnoreCase
    )
    $selected = @($selected | Where-Object { $pattern.IsMatch((Get-RepoRelativePath -Path $_.FullName -Root $repoRoot)) })
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
    throw "matrix FAIL: no positive recovery fixtures matched the requested selection"
  }

  return $selected
}

function Invoke-LoggedNativeCommand {
  param(
    [string]$Command,
    [string[]]$Arguments,
    [string]$LogPath
  )

  $previousErrorAction = $ErrorActionPreference
  try {
    $ErrorActionPreference = "Continue"
    & $Command @Arguments *> $LogPath
    return $LASTEXITCODE
  } finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

$results = @()

Push-Location $repoRoot
try {
  $exe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
    $buildLog = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedNativeCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript, "-ExecutionMode", "binaries-only") `
      -LogPath $buildLog
    if ($buildExit -ne 0) {
      throw "matrix FAIL: native compiler build failed with exit code $buildExit"
    }
  }

  if (!(Test-Path -LiteralPath $exe -PathType Leaf)) {
    throw "matrix FAIL: native compiler executable missing at $exe"
  }

  $positiveFixtures = Get-Fixtures -Directory $positiveDir -FixtureKind "positive recovery"
  $positiveFixtures = Select-Fixtures `
    -Fixtures $positiveFixtures `
    -FixtureListPath $FixtureList `
    -FixtureGlobPattern $FixtureGlob `
    -ShardIndexValue $ShardIndex `
    -ShardCountValue $ShardCount `
    -LimitValue $Limit
  $selectedPositiveCount = $positiveFixtures.Count
  Write-Output ("selection: positive={0}" -f $positiveFixtures.Count)

  foreach ($fixture in $positiveFixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    $hash = Get-ShortHash -Value $fixtureRel
    $caseDir = Join-Path $runDir ("positive_{0}_{1}" -f $hash, $fixture.BaseName)
    $compileLog = Join-Path $caseDir "compile.log"
    New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

    $exitCode = Invoke-LoggedNativeCommand `
      -Command $exe `
      -Arguments @($fixture.FullName, "--out-dir", $caseDir, "--emit-prefix", "module") `
      -LogPath $compileLog

    $objPath = Join-Path $caseDir "module.obj"
    $objExists = Test-Path -LiteralPath $objPath -PathType Leaf
    $objSize = if ($objExists) { (Get-Item -LiteralPath $objPath).Length } else { 0 }

    $passed = ($exitCode -eq 0) -and $objExists -and ($objSize -gt 0)
    $detail = if ($passed) {
      "exit=0 obj_bytes=$objSize"
    } elseif ($exitCode -ne 0) {
      "expected exit=0 got exit=$exitCode"
    } elseif (!$objExists) {
      "missing module.obj"
    } else {
      "empty module.obj"
    }

    $results += [pscustomobject]@{
      kind = "positive"
      fixture = $fixtureRel
      passed = $passed
      detail = $detail
      exit_code = $exitCode
      out_dir = (Get-RepoRelativePath -Path $caseDir -Root $repoRoot)
    }

    $statusToken = if ($passed) { "PASS" } else { "FAIL" }
    Write-Output ("[{0}] positive {1} ({2})" -f $statusToken, $fixtureRel, $detail)
  }
} catch {
  $hadFatalError = $true
  $fatalErrorMessage = $_.Exception.Message
  Write-Output ("error: {0}" -f $fatalErrorMessage)
} finally {
  Pop-Location
}

$total = $results.Count
$passedCount = @($results | Where-Object { $_.passed }).Count
$failedCount = $total - $passedCount
$status = if (!$hadFatalError -and $total -gt 0 -and $failedCount -eq 0) { "PASS" } else { "FAIL" }

$summary = [ordered]@{
  run_dir = (Get-RepoRelativePath -Path $runDir -Root $repoRoot)
  selection = [ordered]@{
    fixture_list = $FixtureList
    fixture_glob = $FixtureGlob
    shard_index = $ShardIndex
    shard_count = $ShardCount
    limit = $Limit
    selected_positive = $selectedPositiveCount
  }
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  fatal_error = $fatalErrorMessage
  results = $results
}
$summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output ("summary: total={0} passed={1} failed={2}" -f $total, $passedCount, $failedCount)
Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $summaryPath -Root $repoRoot))
Write-Output ("status: {0}" -f $status)

if ($status -ne "PASS") {
  exit 1
}
