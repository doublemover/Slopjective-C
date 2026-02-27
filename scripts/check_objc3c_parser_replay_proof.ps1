param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
$nativeExe = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
$fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
$fixturePatterns = @(
  "negative_loop_control_parser_*.objc3",
  "negative_assignment_parser_*.objc3",
  "negative_bitwise_parser_*.objc3",
  "negative_modulo_parser_*.objc3",
  "negative_do_while_parser_*.objc3",
  "negative_for_parser_*.objc3",
  "negative_for_step_parser_*.objc3",
  "negative_foundation_alias_parser_*.objc3",
  "negative_id_parser_*.objc3",
  "negative_prototype_parser_*.objc3",
  "negative_message_missing_keyword_colon.objc3",
  "negative_message_unterminated.objc3",
  "negative_message_parser_*.objc3",
  "negative_return_parser_*.objc3",
  "negative_nested_block_parser_*.objc3",
  "negative_switch_parser_*.objc3",
  "negative_conditional_parser_*.objc3",
  "negative_nil_literal_parser_*.objc3",
  "negative_hex_literal_parser_*.objc3",
  "negative_binary_literal_parser_*.objc3",
  "negative_octal_literal_parser_*.objc3",
  "negative_separator_literal_parser_*.objc3",
  "negative_unary_plus_parser_*.objc3",
  "negative_objc_literal_aliases_parser_*.objc3"
)
$proofRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/parser-replay-proof"
$proofRunId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
$proofDir = Join-Path $proofRoot $proofRunId
$summaryPath = Join-Path $proofDir "summary.json"
$buildLogPath = Join-Path $proofDir "build.log"
$expectedHeaderPattern = "(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$"
$diagnosticCodePattern = "O3[A-Z]\d{3}"
$parserCodePattern = "^O3P\d{3}$"

function Get-RepoRelativePath {
  param([Parameter(Mandatory = $true)][string]$Path)

  $fullPath = (Resolve-Path -LiteralPath $Path).Path
  if ($fullPath.StartsWith($repoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    return $fullPath.Substring($repoRoot.Length).TrimStart('\', '/').Replace('\', '/')
  }
  return $fullPath.Replace('\', '/')
}

function Get-Sha256HexFromBytes {
  param([Parameter(Mandatory = $true)][byte[]]$Bytes)

  $sha256 = [System.Security.Cryptography.SHA256]::Create()
  try {
    $hashBytes = $sha256.ComputeHash($Bytes)
    return ([System.BitConverter]::ToString($hashBytes)).Replace("-", "").ToLowerInvariant()
  }
  finally {
    $sha256.Dispose()
  }
}

function Get-Sha256HexFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  $bytes = [System.IO.File]::ReadAllBytes($Path)
  return Get-Sha256HexFromBytes -Bytes $bytes
}

function Get-NormalizedDiagnosticCodes {
  param([string[]]$Codes)

  if ($null -eq $Codes) {
    return @()
  }

  return @(
    $Codes |
      Where-Object { -not [string]::IsNullOrWhiteSpace($_) } |
      ForEach-Object { $_.ToUpperInvariant() } |
      Sort-Object -Unique
  )
}

function Test-CodeSetsEqual {
  param(
    [string[]]$Left,
    [string[]]$Right
  )

  $leftSet = @(Get-NormalizedDiagnosticCodes -Codes $Left)
  $rightSet = @(Get-NormalizedDiagnosticCodes -Codes $Right)
  if ($leftSet.Count -ne $rightSet.Count) {
    return $false
  }

  for ($i = 0; $i -lt $leftSet.Count; $i++) {
    if ($leftSet[$i] -ne $rightSet[$i]) {
      return $false
    }
  }
  return $true
}

function Get-DiagnosticCodesFromText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $codes = [regex]::Matches($Text, $diagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  return Get-NormalizedDiagnosticCodes -Codes $codes
}

function Get-DiagnosticCodesFromJson {
  param([Parameter(Mandatory = $true)][string]$Path)

  $payload = Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  if ($null -eq $payload -or $null -eq $payload.diagnostics) {
    return @()
  }
  $codes = @($payload.diagnostics | ForEach-Object { "$($_.code)" })
  return Get-NormalizedDiagnosticCodes -Codes $codes
}

function Get-ExpectedParserCodes {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $content = Get-Content -LiteralPath $FixturePath -Raw
  $header = [regex]::Match($content, $expectedHeaderPattern)
  if (-not $header.Success) {
    return [pscustomobject]@{
      header_found = $false
      expected_codes = @()
      non_parser_codes = @()
    }
  }

  $parsedCodes = [regex]::Matches($header.Groups[1].Value, $diagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  $expectedCodes = @(Get-NormalizedDiagnosticCodes -Codes $parsedCodes)
  $nonParserCodes = @($expectedCodes | Where-Object { $_ -notmatch $parserCodePattern })
  return [pscustomobject]@{
    header_found = $true
    expected_codes = @($expectedCodes)
    non_parser_codes = @($nonParserCodes)
  }
}

function Invoke-ParserCompile {
  param(
    [Parameter(Mandatory = $true)][string]$FixturePath,
    [Parameter(Mandatory = $true)][string]$OutDir,
    [Parameter(Mandatory = $true)][string]$LogPath
  )

  New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
  & $nativeExe $FixturePath --out-dir $OutDir --emit-prefix module *> $LogPath
  return [int]$LASTEXITCODE
}

New-Item -ItemType Directory -Force -Path $proofDir | Out-Null

if (!(Test-Path -LiteralPath $buildScript -PathType Leaf)) {
  Write-Output "error: parser replay proof FAIL: missing build script at $buildScript"
  exit 1
}
if (!(Test-Path -LiteralPath $fixtureDir -PathType Container)) {
  Write-Output "error: parser replay proof FAIL: missing fixture directory at $fixtureDir"
  exit 1
}

if (!(Test-Path -LiteralPath $nativeExe -PathType Leaf)) {
  & powershell -NoProfile -ExecutionPolicy Bypass -File $buildScript *> $buildLogPath
  $buildExitCode = [int]$LASTEXITCODE
  if ($buildExitCode -ne 0) {
    Write-Output ("error: parser replay proof FAIL: native build failed with exit {0}" -f $buildExitCode)
    Write-Output ("build_log: {0}" -f (Get-RepoRelativePath -Path $buildLogPath))
    exit 1
  }
}

if (!(Test-Path -LiteralPath $nativeExe -PathType Leaf)) {
  Write-Output "error: parser replay proof FAIL: missing native compiler executable after build"
  exit 1
}

$fixturesByPath = [System.Collections.Generic.Dictionary[string, System.IO.FileInfo]]::new([System.StringComparer]::OrdinalIgnoreCase)
foreach ($pattern in $fixturePatterns) {
  $matched = @(Get-ChildItem -LiteralPath $fixtureDir -File -Filter $pattern | Sort-Object -Property Name)
  if ($matched.Count -eq 0) {
    Write-Output ("error: parser replay proof FAIL: expected parser fixtures matching {0}; found 0" -f $pattern)
    exit 1
  }
  foreach ($fixture in $matched) {
    if (-not $fixturesByPath.ContainsKey($fixture.FullName)) {
      $fixturesByPath.Add($fixture.FullName, $fixture)
    }
  }
}
$fixtures = @($fixturesByPath.Values | Sort-Object -Property Name)
if ($fixtures.Count -lt 36) {
  Write-Output ("error: parser replay proof FAIL: expected at least 36 parser fixtures across patterns; found {0}" -f $fixtures.Count)
  exit 1
}

$results = New-Object System.Collections.Generic.List[object]
$failedCount = 0

foreach ($fixture in $fixtures) {
  $fixtureErrors = New-Object System.Collections.Generic.List[string]
  $fixtureName = $fixture.Name
  $caseDir = Join-Path (Join-Path $proofDir "cases") $fixture.BaseName
  $run1OutDir = Join-Path $caseDir "run1"
  $run2OutDir = Join-Path $caseDir "run2"
  $run1LogPath = Join-Path $caseDir "run1.log"
  $run2LogPath = Join-Path $caseDir "run2.log"
  New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

  $header = Get-ExpectedParserCodes -FixturePath $fixture.FullName
  if (-not $header.header_found) {
    $fixtureErrors.Add("missing expected diagnostic header")
  }
  if ($header.expected_codes.Count -eq 0) {
    $fixtureErrors.Add("expected diagnostic header has no parseable diagnostic code")
  }
  if ($header.non_parser_codes.Count -gt 0) {
    $fixtureErrors.Add("expected diagnostics must be parser codes only (found: $($header.non_parser_codes -join ', '))")
  }

  $run1Exit = Invoke-ParserCompile -FixturePath $fixture.FullName -OutDir $run1OutDir -LogPath $run1LogPath
  $run2Exit = Invoke-ParserCompile -FixturePath $fixture.FullName -OutDir $run2OutDir -LogPath $run2LogPath

  if ($run1Exit -eq 0) {
    $fixtureErrors.Add("run1 unexpectedly succeeded")
  }
  if ($run2Exit -eq 0) {
    $fixtureErrors.Add("run2 unexpectedly succeeded")
  }
  if ($run1Exit -ne $run2Exit) {
    $fixtureErrors.Add("exit code drift across replay ($run1Exit vs $run2Exit)")
  }

  $run1DiagTxt = Join-Path $run1OutDir "module.diagnostics.txt"
  $run2DiagTxt = Join-Path $run2OutDir "module.diagnostics.txt"
  $run1DiagJson = Join-Path $run1OutDir "module.diagnostics.json"
  $run2DiagJson = Join-Path $run2OutDir "module.diagnostics.json"

  $run1TxtPresent = Test-Path -LiteralPath $run1DiagTxt -PathType Leaf
  $run2TxtPresent = Test-Path -LiteralPath $run2DiagTxt -PathType Leaf
  $run1JsonPresent = Test-Path -LiteralPath $run1DiagJson -PathType Leaf
  $run2JsonPresent = Test-Path -LiteralPath $run2DiagJson -PathType Leaf

  if (-not $run1TxtPresent) {
    $fixtureErrors.Add("run1 missing module.diagnostics.txt")
  }
  if (-not $run2TxtPresent) {
    $fixtureErrors.Add("run2 missing module.diagnostics.txt")
  }
  if (-not $run1JsonPresent) {
    $fixtureErrors.Add("run1 missing module.diagnostics.json")
  }
  if (-not $run2JsonPresent) {
    $fixtureErrors.Add("run2 missing module.diagnostics.json")
  }

  $run1TxtHash = ""
  $run2TxtHash = ""
  $run1JsonHash = ""
  $run2JsonHash = ""
  $run1TxtCodes = @()
  $run2TxtCodes = @()
  $run1JsonCodes = @()
  $run2JsonCodes = @()

  if ($run1TxtPresent) {
    $run1TxtBytes = [System.IO.File]::ReadAllBytes($run1DiagTxt)
    if ($run1TxtBytes.Length -eq 0) {
      $fixtureErrors.Add("run1 diagnostics text is empty")
    } else {
      $run1TxtHash = Get-Sha256HexFromBytes -Bytes $run1TxtBytes
      $run1TxtCodes = @(Get-DiagnosticCodesFromText -Text ([System.Text.Encoding]::UTF8.GetString($run1TxtBytes)))
    }
  }
  if ($run2TxtPresent) {
    $run2TxtBytes = [System.IO.File]::ReadAllBytes($run2DiagTxt)
    if ($run2TxtBytes.Length -eq 0) {
      $fixtureErrors.Add("run2 diagnostics text is empty")
    } else {
      $run2TxtHash = Get-Sha256HexFromBytes -Bytes $run2TxtBytes
      $run2TxtCodes = @(Get-DiagnosticCodesFromText -Text ([System.Text.Encoding]::UTF8.GetString($run2TxtBytes)))
    }
  }

  if ($run1JsonPresent) {
    $run1JsonBytes = [System.IO.File]::ReadAllBytes($run1DiagJson)
    if ($run1JsonBytes.Length -eq 0) {
      $fixtureErrors.Add("run1 diagnostics json is empty")
    } else {
      $run1JsonHash = Get-Sha256HexFromBytes -Bytes $run1JsonBytes
      try {
        $run1JsonCodes = @(Get-DiagnosticCodesFromJson -Path $run1DiagJson)
      } catch {
        $fixtureErrors.Add("run1 diagnostics json is invalid")
      }
    }
  }
  if ($run2JsonPresent) {
    $run2JsonBytes = [System.IO.File]::ReadAllBytes($run2DiagJson)
    if ($run2JsonBytes.Length -eq 0) {
      $fixtureErrors.Add("run2 diagnostics json is empty")
    } else {
      $run2JsonHash = Get-Sha256HexFromBytes -Bytes $run2JsonBytes
      try {
        $run2JsonCodes = @(Get-DiagnosticCodesFromJson -Path $run2DiagJson)
      } catch {
        $fixtureErrors.Add("run2 diagnostics json is invalid")
      }
    }
  }

  if ($run1TxtHash -ne "" -and $run2TxtHash -ne "" -and $run1TxtHash -ne $run2TxtHash) {
    $fixtureErrors.Add("diagnostics text hash drift across replay")
  }
  if ($run1JsonHash -ne "" -and $run2JsonHash -ne "" -and $run1JsonHash -ne $run2JsonHash) {
    $fixtureErrors.Add("diagnostics json hash drift across replay")
  }
  if ($run1TxtCodes.Count -gt 0 -and -not (Test-CodeSetsEqual -Left $run1TxtCodes -Right $run2TxtCodes)) {
    $fixtureErrors.Add("diagnostic text code drift across replay")
  }
  if ($run1JsonCodes.Count -gt 0 -and -not (Test-CodeSetsEqual -Left $run1JsonCodes -Right $run2JsonCodes)) {
    $fixtureErrors.Add("diagnostic json code drift across replay")
  }
  if ($run1TxtCodes.Count -gt 0 -and $run1JsonCodes.Count -gt 0 -and -not (Test-CodeSetsEqual -Left $run1TxtCodes -Right $run1JsonCodes)) {
    $fixtureErrors.Add("run1 diagnostics json codes differ from text codes")
  }
  if ($run2TxtCodes.Count -gt 0 -and $run2JsonCodes.Count -gt 0 -and -not (Test-CodeSetsEqual -Left $run2TxtCodes -Right $run2JsonCodes)) {
    $fixtureErrors.Add("run2 diagnostics json codes differ from text codes")
  }

  if ($header.expected_codes.Count -gt 0 -and $run1TxtCodes.Count -gt 0) {
    $missing1 = @($header.expected_codes | Where-Object { $_ -notin $run1TxtCodes })
    if ($missing1.Count -gt 0) {
      $fixtureErrors.Add("run1 missing expected diagnostic code(s): $($missing1 -join ', ')")
    }
  }
  if ($header.expected_codes.Count -gt 0 -and $run2TxtCodes.Count -gt 0) {
    $missing2 = @($header.expected_codes | Where-Object { $_ -notin $run2TxtCodes })
    if ($missing2.Count -gt 0) {
      $fixtureErrors.Add("run2 missing expected diagnostic code(s): $($missing2 -join ', ')")
    }
  }

  $passed = $fixtureErrors.Count -eq 0
  if ($passed) {
    Write-Output "PASS: $fixtureName"
  } else {
    $failedCount++
    Write-Output "FAIL: $fixtureName"
    foreach ($message in $fixtureErrors) {
      Write-Output ("  - {0}" -f $message)
    }
  }

  $results.Add([ordered]@{
      fixture = $fixtureName
      expected_codes = @($header.expected_codes)
      run1_exit_code = $run1Exit
      run2_exit_code = $run2Exit
      run1_text_codes = @($run1TxtCodes)
      run2_text_codes = @($run2TxtCodes)
      run1_json_codes = @($run1JsonCodes)
      run2_json_codes = @($run2JsonCodes)
      run1_diagnostics_txt_sha256 = $run1TxtHash
      run2_diagnostics_txt_sha256 = $run2TxtHash
      run1_diagnostics_json_sha256 = $run1JsonHash
      run2_diagnostics_json_sha256 = $run2JsonHash
      run1_log = Get-RepoRelativePath -Path $run1LogPath
      run2_log = Get-RepoRelativePath -Path $run2LogPath
      passed = $passed
      errors = $fixtureErrors.ToArray()
    })
}

$total = $fixtures.Count
$passedCount = $total - $failedCount
$status = if ($failedCount -eq 0) { "PASS" } else { "FAIL" }

$summary = [ordered]@{
  generated_at_utc = (Get-Date).ToUniversalTime().ToString("o")
  proof_run_id = $proofRunId
  fixture_patterns = @($fixturePatterns)
  total = $total
  passed = $passedCount
  failed = $failedCount
  status = $status
  results = $results.ToArray()
}
$summary | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $summaryPath -Encoding utf8

if ($failedCount -eq 0) {
  Write-Output ("PASS SUMMARY: {0}/{1} fixtures passed parser replay proof." -f $passedCount, $total)
} else {
  Write-Output ("FAIL SUMMARY: {0}/{1} fixtures passed parser replay proof; {2} failed." -f $passedCount, $total, $failedCount)
}
Write-Output ("summary_path: {0}" -f (Get-RepoRelativePath -Path $summaryPath))
Write-Output ("status: {0}" -f $status)

if ($failedCount -ne 0) {
  exit 1
}
