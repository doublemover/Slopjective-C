$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
$suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/diagnostics-regression"
$runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$runDir = Join-Path $suiteRoot $runId
$summaryPath = Join-Path $runDir "summary.json"
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$exePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$headerPattern = '(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$'
$diagnosticCodePattern = 'O3[A-Z]\d{3}'
$fatalErrorMessage = ""
$hadFatalError = $false

function Get-RepoRelativePath {
  param(
    [string]$Path,
    [string]$Root
  )

  $fullRoot = (Resolve-Path -LiteralPath $Root).Path
  $fullPath = [System.IO.Path]::GetFullPath($Path)
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
    return [int]$LASTEXITCODE
  } finally {
    $ErrorActionPreference = $previousErrorAction
  }
}

function Get-ExpectedCodesFromFixture {
  param([string]$FixturePath)

  $content = Get-Content -LiteralPath $FixturePath -Raw
  $headerMatch = [regex]::Match($content, $headerPattern)
  if (-not $headerMatch.Success) {
    return [pscustomobject]@{
      header_found = $false
      header_text = ""
      codes = @()
    }
  }

  $uniqueCodes = New-Object 'System.Collections.Generic.List[string]'
  foreach ($codeMatch in [regex]::Matches($headerMatch.Groups[1].Value, $diagnosticCodePattern)) {
    $code = $codeMatch.Value.ToUpperInvariant()
    if (-not $uniqueCodes.Contains($code)) {
      $null = $uniqueCodes.Add($code)
    }
  }

  return [pscustomobject]@{
    header_found = $true
    header_text = $headerMatch.Groups[1].Value.Trim()
    codes = @($uniqueCodes)
  }
}

function Get-DiagnosticsData {
  param([string]$OutDir)

  $diagPath = Join-Path $OutDir "module.diagnostics.txt"
  if (-not (Test-Path -LiteralPath $diagPath -PathType Leaf)) {
    return [pscustomobject]@{
      exists = $false
      path = $diagPath
      size_bytes = 0
      text = ""
      bytes = @()
      codes = @()
      sha256 = ""
      populated = $false
    }
  }

  $bytes = [System.IO.File]::ReadAllBytes($diagPath)
  $text = [System.IO.File]::ReadAllText($diagPath)
  $uniqueCodes = New-Object 'System.Collections.Generic.List[string]'
  foreach ($codeMatch in [regex]::Matches($text, $diagnosticCodePattern)) {
    $code = $codeMatch.Value.ToUpperInvariant()
    if (-not $uniqueCodes.Contains($code)) {
      $null = $uniqueCodes.Add($code)
    }
  }
  $sha256 = Get-ByteArraySha256Hex -Bytes $bytes

  return [pscustomobject]@{
    exists = $true
    path = $diagPath
    size_bytes = $bytes.Length
    text = $text
    bytes = $bytes
    codes = @($uniqueCodes)
    sha256 = $sha256
    populated = -not [string]::IsNullOrWhiteSpace($text)
  }
}

function Get-DiagnosticsJsonData {
  param([string]$OutDir)

  $diagPath = Join-Path $OutDir "module.diagnostics.json"
  if (-not (Test-Path -LiteralPath $diagPath -PathType Leaf)) {
    return [pscustomobject]@{
      exists = $false
      path = $diagPath
      size_bytes = 0
      text = ""
      bytes = @()
      codes = @()
      sha256 = ""
      populated = $false
      valid = $false
      parse_error = ""
    }
  }

  $bytes = [System.IO.File]::ReadAllBytes($diagPath)
  $text = [System.IO.File]::ReadAllText($diagPath)
  $sha256 = Get-ByteArraySha256Hex -Bytes $bytes
  $populated = -not [string]::IsNullOrWhiteSpace($text)
  if (-not $populated) {
    return [pscustomobject]@{
      exists = $true
      path = $diagPath
      size_bytes = $bytes.Length
      text = $text
      bytes = $bytes
      codes = @()
      sha256 = $sha256
      populated = $false
      valid = $false
      parse_error = "empty diagnostics json"
    }
  }

  try {
    $payload = $text | ConvertFrom-Json
  } catch {
    return [pscustomobject]@{
      exists = $true
      path = $diagPath
      size_bytes = $bytes.Length
      text = $text
      bytes = $bytes
      codes = @()
      sha256 = $sha256
      populated = $true
      valid = $false
      parse_error = $_.Exception.Message
    }
  }

  $diagRows = @()
  if ($null -ne $payload -and $payload.PSObject.Properties.Name -contains "diagnostics") {
    $diagRows = @($payload.diagnostics)
  }
  if ($diagRows.Count -eq 0) {
    return [pscustomobject]@{
      exists = $true
      path = $diagPath
      size_bytes = $bytes.Length
      text = $text
      bytes = $bytes
      codes = @()
      sha256 = $sha256
      populated = $true
      valid = $false
      parse_error = "missing or empty diagnostics array"
    }
  }

  $uniqueCodes = New-Object 'System.Collections.Generic.List[string]'
  foreach ($row in $diagRows) {
    if ($null -eq $row) {
      continue
    }
    $code = ""
    if ($row.PSObject.Properties.Name -contains "code" -and $null -ne $row.code) {
      $code = [string]$row.code
    }
    if ([string]::IsNullOrWhiteSpace($code)) {
      continue
    }
    $normalized = $code.Trim().ToUpperInvariant()
    if (-not $uniqueCodes.Contains($normalized)) {
      $null = $uniqueCodes.Add($normalized)
    }
  }

  return [pscustomobject]@{
    exists = $true
    path = $diagPath
    size_bytes = $bytes.Length
    text = $text
    bytes = $bytes
    codes = @($uniqueCodes)
    sha256 = $sha256
    populated = $true
    valid = $true
    parse_error = ""
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

function Get-ByteArraySha256Hex {
  param([byte[]]$Bytes)

  if ($null -eq $Bytes) {
    return ""
  }

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  } finally {
    $sha256.Dispose()
  }
}

function Test-CodeSetExactMatch {
  param(
    [string[]]$Expected,
    [string[]]$Actual
  )

  $missing = @($Expected | Where-Object { $_ -notin $Actual })
  $unexpected = @($Actual | Where-Object { $_ -notin $Expected })
  return [pscustomobject]@{
    match = ($missing.Count -eq 0 -and $unexpected.Count -eq 0)
    missing = $missing
    unexpected = $unexpected
  }
}

function Get-UnexpectedFailClosedArtifacts {
  param(
    [string]$OutDir,
    [string[]]$ArtifactNames = @("module.manifest.json", "module.ll", "module.obj")
  )

  $unexpected = New-Object 'System.Collections.Generic.List[string]'
  foreach ($artifact in $ArtifactNames) {
    if (Test-Path -LiteralPath (Join-Path $OutDir $artifact) -PathType Leaf) {
      $null = $unexpected.Add($artifact)
    }
  }

  return @($unexpected.ToArray())
}

function Test-ExpectedCodesForRun {
  param(
    [pscustomobject]$ExpectedSpec,
    [pscustomobject]$Diagnostics,
    [string]$RunLabel
  )

  $errors = New-Object 'System.Collections.Generic.List[string]'
  $codesMatch = $true

  if (@($ExpectedSpec.codes).Count -gt 0) {
    if ($Diagnostics.exists -and $Diagnostics.populated) {
      $codeCheck = Test-CodeSetExactMatch -Expected $ExpectedSpec.codes -Actual $Diagnostics.codes
      $codesMatch = $codeCheck.match
      if (-not $codeCheck.match) {
        if (@($codeCheck.missing).Count -gt 0) {
          $null = $errors.Add("$RunLabel missing expected code(s): $($codeCheck.missing -join ', ')")
        }
        if (@($codeCheck.unexpected).Count -gt 0) {
          $null = $errors.Add("$RunLabel unexpected code(s): $($codeCheck.unexpected -join ', ')")
        }
      }
    } else {
      $codesMatch = $false
      $null = $errors.Add("$RunLabel cannot enforce expected diagnostic codes because diagnostics are unavailable")
    }
  }

  return [pscustomobject]@{
    codes_match = $codesMatch
    errors = @($errors)
  }
}

New-Item -ItemType Directory -Force -Path $runDir | Out-Null

$results = @()

Push-Location $repoRoot
try {
  if (-not (Test-Path -LiteralPath $buildScript -PathType Leaf)) {
    throw "suite FAIL: missing build script at $buildScript"
  }

  if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
    $buildLogPath = Join-Path $runDir "build.log"
    $buildExit = Invoke-LoggedNativeCommand `
      -Command "powershell" `
      -Arguments @("-NoProfile", "-ExecutionPolicy", "Bypass", "-File", $buildScript) `
      -LogPath $buildLogPath
    if ($buildExit -ne 0) {
      throw "suite FAIL: native compiler build failed with exit code $buildExit"
    }
  }

  if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
    throw "suite FAIL: native compiler executable missing at $exePath"
  }

  if (-not (Test-Path -LiteralPath $fixtureDir -PathType Container)) {
    throw "suite FAIL: missing negative fixture directory at $fixtureDir"
  }

  $fixtures = @(
    Get-ChildItem -LiteralPath $fixtureDir -Recurse -File |
      Where-Object { $_.Extension -in @(".objc3", ".m") } |
      Sort-Object -Property FullName
  )

  if ($fixtures.Count -eq 0) {
    throw "suite FAIL: no negative fixtures found in $fixtureDir"
  }

  foreach ($fixture in $fixtures) {
    $fixtureRel = Get-RepoRelativePath -Path $fixture.FullName -Root $repoRoot
    $fixtureSlug = "$(Get-ShortHash -Value $fixtureRel)_$($fixture.BaseName)"
    $fixtureRunDir = Join-Path $runDir $fixtureSlug
    $run1OutDir = Join-Path $fixtureRunDir "run1"
    $run2OutDir = Join-Path $fixtureRunDir "run2"
    $run1LogPath = Join-Path $fixtureRunDir "run1.compile.log"
    $run2LogPath = Join-Path $fixtureRunDir "run2.compile.log"
    New-Item -ItemType Directory -Force -Path $fixtureRunDir | Out-Null

    $errors = New-Object 'System.Collections.Generic.List[string]'
    $expectedSpec = Get-ExpectedCodesFromFixture -FixturePath $fixture.FullName
    if (-not $expectedSpec.header_found) {
      $null = $errors.Add("missing expected diagnostic header")
    } elseif (@($expectedSpec.codes).Count -eq 0) {
      $null = $errors.Add("expected diagnostic header has no parseable O3* codes")
    }

    $exit1 = Invoke-LoggedNativeCommand `
      -Command $exePath `
      -Arguments @($fixture.FullName, "--out-dir", $run1OutDir, "--emit-prefix", "module") `
      -LogPath $run1LogPath
    $exit2 = Invoke-LoggedNativeCommand `
      -Command $exePath `
      -Arguments @($fixture.FullName, "--out-dir", $run2OutDir, "--emit-prefix", "module") `
      -LogPath $run2LogPath

    if ($exit1 -eq 0) {
      $null = $errors.Add("run1 expected nonzero exit but got 0")
    }
    if ($exit2 -eq 0) {
      $null = $errors.Add("run2 expected nonzero exit but got 0")
    }
    if ($exit1 -ne $exit2) {
      $null = $errors.Add("exit code drift across replay ($exit1 vs $exit2)")
    }

    $diag1 = Get-DiagnosticsData -OutDir $run1OutDir
    $diag2 = Get-DiagnosticsData -OutDir $run2OutDir
    $diagJson1 = Get-DiagnosticsJsonData -OutDir $run1OutDir
    $diagJson2 = Get-DiagnosticsJsonData -OutDir $run2OutDir

    if (-not $diag1.exists) {
      $null = $errors.Add("run1 missing module.diagnostics.txt")
    } elseif (-not $diag1.populated) {
      $null = $errors.Add("run1 diagnostics file is empty")
    }

    if (-not $diag2.exists) {
      $null = $errors.Add("run2 missing module.diagnostics.txt")
    } elseif (-not $diag2.populated) {
      $null = $errors.Add("run2 diagnostics file is empty")
    }

    if (-not $diagJson1.exists) {
      $null = $errors.Add("run1 missing module.diagnostics.json")
    } elseif (-not $diagJson1.populated) {
      $null = $errors.Add("run1 diagnostics json file is empty")
    } elseif (-not $diagJson1.valid) {
      $null = $errors.Add("run1 diagnostics json invalid: $($diagJson1.parse_error)")
    }

    if (-not $diagJson2.exists) {
      $null = $errors.Add("run2 missing module.diagnostics.json")
    } elseif (-not $diagJson2.populated) {
      $null = $errors.Add("run2 diagnostics json file is empty")
    } elseif (-not $diagJson2.valid) {
      $null = $errors.Add("run2 diagnostics json invalid: $($diagJson2.parse_error)")
    }

    $diagnosticsDeterministic = $false
    if ($diag1.exists -and $diag2.exists) {
      $diagnosticsDeterministic = Test-ByteArrayEqual -Left $diag1.bytes -Right $diag2.bytes
      if (-not $diagnosticsDeterministic) {
        $null = $errors.Add("diagnostics replay mismatch: run1/run2 bytes differ")
      }
    }

    $diagnosticsJsonDeterministic = $false
    if ($diagJson1.exists -and $diagJson2.exists) {
      $diagnosticsJsonDeterministic = Test-ByteArrayEqual -Left $diagJson1.bytes -Right $diagJson2.bytes
      if (-not $diagnosticsJsonDeterministic) {
        $null = $errors.Add("diagnostics json replay mismatch: run1/run2 bytes differ")
      }
    }

    $run1JsonCodesMatchText = $false
    if ($diag1.exists -and $diag1.populated -and $diagJson1.exists -and $diagJson1.valid) {
      $run1JsonCodeCheck = Test-CodeSetExactMatch -Expected $diag1.codes -Actual $diagJson1.codes
      $run1JsonCodesMatchText = $run1JsonCodeCheck.match
      if (-not $run1JsonCodeCheck.match) {
        if (@($run1JsonCodeCheck.missing).Count -gt 0) {
          $null = $errors.Add("run1 diagnostics json missing code(s) present in text: $($run1JsonCodeCheck.missing -join ', ')")
        }
        if (@($run1JsonCodeCheck.unexpected).Count -gt 0) {
          $null = $errors.Add("run1 diagnostics json has unexpected code(s) vs text: $($run1JsonCodeCheck.unexpected -join ', ')")
        }
      }
    }

    $run2JsonCodesMatchText = $false
    if ($diag2.exists -and $diag2.populated -and $diagJson2.exists -and $diagJson2.valid) {
      $run2JsonCodeCheck = Test-CodeSetExactMatch -Expected $diag2.codes -Actual $diagJson2.codes
      $run2JsonCodesMatchText = $run2JsonCodeCheck.match
      if (-not $run2JsonCodeCheck.match) {
        if (@($run2JsonCodeCheck.missing).Count -gt 0) {
          $null = $errors.Add("run2 diagnostics json missing code(s) present in text: $($run2JsonCodeCheck.missing -join ', ')")
        }
        if (@($run2JsonCodeCheck.unexpected).Count -gt 0) {
          $null = $errors.Add("run2 diagnostics json has unexpected code(s) vs text: $($run2JsonCodeCheck.unexpected -join ', ')")
        }
      }
    }

    $unexpectedArtifactsRun1 = @(Get-UnexpectedFailClosedArtifacts -OutDir $run1OutDir)
    if ($unexpectedArtifactsRun1.Count -gt 0) {
      $null = $errors.Add("run1 fail-closed violation: emitted artifact(s) $($unexpectedArtifactsRun1 -join ', ')")
    }

    $unexpectedArtifactsRun2 = @(Get-UnexpectedFailClosedArtifacts -OutDir $run2OutDir)
    if ($unexpectedArtifactsRun2.Count -gt 0) {
      $null = $errors.Add("run2 fail-closed violation: emitted artifact(s) $($unexpectedArtifactsRun2 -join ', ')")
    }

    $run1CodeCheck = Test-ExpectedCodesForRun -ExpectedSpec $expectedSpec -Diagnostics $diag1 -RunLabel "run1"
    $run1CodeMatch = [bool]$run1CodeCheck.codes_match
    foreach ($errorMessage in $run1CodeCheck.errors) {
      $null = $errors.Add($errorMessage)
    }
    $run2CodeCheck = Test-ExpectedCodesForRun -ExpectedSpec $expectedSpec -Diagnostics $diag2 -RunLabel "run2"
    $run2CodeMatch = [bool]$run2CodeCheck.codes_match
    foreach ($errorMessage in $run2CodeCheck.errors) {
      $null = $errors.Add($errorMessage)
    }

    $passed = ($errors.Count -eq 0)
    $statusToken = if ($passed) { "PASS" } else { "FAIL" }
    Write-Output "[$statusToken] $fixtureRel"

    if (-not $passed) {
      foreach ($errorMessage in $errors) {
        Write-Output "  - $errorMessage"
      }
    }

    $results += [pscustomobject]@{
      fixture = $fixtureRel
      expected_codes = @($expectedSpec.codes)
      passed = $passed
      run1_exit_code = $exit1
      run2_exit_code = $exit2
      run1_diagnostic_codes = @($diag1.codes)
      run2_diagnostic_codes = @($diag2.codes)
      run1_diagnostics_sha256 = $diag1.sha256
      run2_diagnostics_sha256 = $diag2.sha256
      run1_diagnostics_json_codes = @($diagJson1.codes)
      run2_diagnostics_json_codes = @($diagJson2.codes)
      run1_diagnostics_json_sha256 = $diagJson1.sha256
      run2_diagnostics_json_sha256 = $diagJson2.sha256
      run1_diagnostics = (Get-RepoRelativePath -Path $diag1.path -Root $repoRoot)
      run2_diagnostics = (Get-RepoRelativePath -Path $diag2.path -Root $repoRoot)
      run1_diagnostics_json = (Get-RepoRelativePath -Path $diagJson1.path -Root $repoRoot)
      run2_diagnostics_json = (Get-RepoRelativePath -Path $diagJson2.path -Root $repoRoot)
      run1_log = (Get-RepoRelativePath -Path $run1LogPath -Root $repoRoot)
      run2_log = (Get-RepoRelativePath -Path $run2LogPath -Root $repoRoot)
      out_dir = (Get-RepoRelativePath -Path $fixtureRunDir -Root $repoRoot)
      checks = [ordered]@{
        exit_nonzero = (($exit1 -ne 0) -and ($exit2 -ne 0))
        exit_deterministic = ($exit1 -eq $exit2)
        diagnostics_present = ($diag1.exists -and $diag2.exists)
        diagnostics_nonempty = ($diag1.populated -and $diag2.populated)
        diagnostics_deterministic = $diagnosticsDeterministic
        diagnostics_json_present = ($diagJson1.exists -and $diagJson2.exists)
        diagnostics_json_nonempty = ($diagJson1.populated -and $diagJson2.populated)
        diagnostics_json_valid = ($diagJson1.valid -and $diagJson2.valid)
        diagnostics_json_deterministic = $diagnosticsJsonDeterministic
        diagnostics_json_codes_match_text = ($run1JsonCodesMatchText -and $run2JsonCodesMatchText)
        codes_match_expected = ($run1CodeMatch -and $run2CodeMatch)
        fail_closed_artifacts = ($unexpectedArtifactsRun1.Count -eq 0 -and $unexpectedArtifactsRun2.Count -eq 0)
      }
      errors = @($errors)
    }
  }
} catch {
  $hadFatalError = $true
  $fatalErrorMessage = $_.Exception.Message
  Write-Output "error: $fatalErrorMessage"
} finally {
  Pop-Location
}

$total = $results.Count
$passedCount = @($results | Where-Object { $_.passed }).Count
$failedCount = $total - $passedCount
$status = if (-not $hadFatalError -and $total -gt 0 -and $failedCount -eq 0) { "PASS" } else { "FAIL" }

$summary = [ordered]@{
  run_id = $runId
  run_dir = (Get-RepoRelativePath -Path $runDir -Root $repoRoot)
  generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  fatal_error = $fatalErrorMessage
  results = $results
}
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

Write-Output "summary_path: $(Get-RepoRelativePath -Path $summaryPath -Root $repoRoot)"
Write-Output "overall: $status ($passedCount/$total passed)"

if ($status -ne "PASS") {
  exit 1
}
