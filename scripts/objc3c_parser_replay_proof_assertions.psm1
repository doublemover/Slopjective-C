Set-StrictMode -Version Latest

Import-Module (Join-Path $PSScriptRoot "objc3c_parser_replay_proof_invocation.psm1") -Force -DisableNameChecking

$script:ExpectedHeaderPattern = "(?mi)^\s*//\s*Expected diagnostic code\(s\):\s*(.+?)\s*$"
$script:DiagnosticCodePattern = "O3[A-Z]\d{3}"
$script:ParserCodePattern = "^O3P\d{3}$"

function Get-Objc3cParserReplaySha256HexFromBytes {
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

function Get-Objc3cParserReplaySha256HexFromFile {
  param([Parameter(Mandatory = $true)][string]$Path)

  $bytes = [System.IO.File]::ReadAllBytes($Path)
  return Get-Objc3cParserReplaySha256HexFromBytes -Bytes $bytes
}

function Get-Objc3cParserReplayNormalizedDiagnosticCodes {
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

function Test-Objc3cParserReplayCodeSetsEqual {
  param(
    [string[]]$Left,
    [string[]]$Right
  )

  $leftSet = @(Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $Left)
  $rightSet = @(Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $Right)
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

function Get-Objc3cParserReplayDiagnosticCodesFromText {
  param([Parameter(Mandatory = $true)][string]$Text)

  $codes = [regex]::Matches($Text, $script:DiagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  return Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $codes
}

function Get-Objc3cParserReplayDiagnosticCodesFromJson {
  param([Parameter(Mandatory = $true)][string]$Path)

  $payload = Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
  if ($null -eq $payload -or $null -eq $payload.diagnostics) {
    return @()
  }
  $codes = @($payload.diagnostics | ForEach-Object { "$($_.code)" })
  return Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $codes
}

function Get-Objc3cParserReplayExpectedParserCodes {
  param([Parameter(Mandatory = $true)][string]$FixturePath)

  $content = Get-Content -LiteralPath $FixturePath -Raw
  $header = [regex]::Match($content, $script:ExpectedHeaderPattern)
  if (-not $header.Success) {
    return [pscustomobject]@{
      header_found = $false
      expected_codes = @()
      non_parser_codes = @()
    }
  }

  $parsedCodes = [regex]::Matches($header.Groups[1].Value, $script:DiagnosticCodePattern) | ForEach-Object {
    $_.Value.ToUpperInvariant()
  }
  $expectedCodes = @(Get-Objc3cParserReplayNormalizedDiagnosticCodes -Codes $parsedCodes)
  $nonParserCodes = @($expectedCodes | Where-Object { $_ -notmatch $script:ParserCodePattern })
  return [pscustomobject]@{
    header_found = $true
    expected_codes = @($expectedCodes)
    non_parser_codes = @($nonParserCodes)
  }
}

function Invoke-Objc3cParserReplayFixtureCase {
  param(
    [Parameter(Mandatory = $true)][System.IO.FileInfo]$Fixture,
    [Parameter(Mandatory = $true)][string]$ProofDir,
    [Parameter(Mandatory = $true)][string]$NativeExe,
    [Parameter(Mandatory = $true)][string]$RepoRoot
  )

  $fixtureErrors = New-Object System.Collections.Generic.List[string]
  $fixtureName = $Fixture.Name
  $caseDir = Join-Path (Join-Path $ProofDir "cases") $Fixture.BaseName
  $run1OutDir = Join-Path $caseDir "run1"
  $run2OutDir = Join-Path $caseDir "run2"
  $run1LogPath = Join-Path $caseDir "run1.log"
  $run2LogPath = Join-Path $caseDir "run2.log"
  New-Item -ItemType Directory -Force -Path $caseDir | Out-Null

  $header = Get-Objc3cParserReplayExpectedParserCodes -FixturePath $Fixture.FullName
  if (-not $header.header_found) {
    $fixtureErrors.Add("missing expected diagnostic header")
  }
  if ($header.expected_codes.Count -eq 0) {
    $fixtureErrors.Add("expected diagnostic header has no parseable diagnostic code")
  }
  if ($header.non_parser_codes.Count -gt 0) {
    $fixtureErrors.Add("expected diagnostics must be parser codes only (found: $($header.non_parser_codes -join ', '))")
  }

  $run1Exit = Invoke-Objc3cParserReplayCompile -NativeExe $NativeExe -FixturePath $Fixture.FullName -OutDir $run1OutDir -LogPath $run1LogPath
  $run2Exit = Invoke-Objc3cParserReplayCompile -NativeExe $NativeExe -FixturePath $Fixture.FullName -OutDir $run2OutDir -LogPath $run2LogPath

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
      $run1TxtHash = Get-Objc3cParserReplaySha256HexFromBytes -Bytes $run1TxtBytes
      $run1TxtCodes = @(Get-Objc3cParserReplayDiagnosticCodesFromText -Text ([System.Text.Encoding]::UTF8.GetString($run1TxtBytes)))
    }
  }
  if ($run2TxtPresent) {
    $run2TxtBytes = [System.IO.File]::ReadAllBytes($run2DiagTxt)
    if ($run2TxtBytes.Length -eq 0) {
      $fixtureErrors.Add("run2 diagnostics text is empty")
    } else {
      $run2TxtHash = Get-Objc3cParserReplaySha256HexFromBytes -Bytes $run2TxtBytes
      $run2TxtCodes = @(Get-Objc3cParserReplayDiagnosticCodesFromText -Text ([System.Text.Encoding]::UTF8.GetString($run2TxtBytes)))
    }
  }

  if ($run1JsonPresent) {
    $run1JsonBytes = [System.IO.File]::ReadAllBytes($run1DiagJson)
    if ($run1JsonBytes.Length -eq 0) {
      $fixtureErrors.Add("run1 diagnostics json is empty")
    } else {
      $run1JsonHash = Get-Objc3cParserReplaySha256HexFromBytes -Bytes $run1JsonBytes
      try {
        $run1JsonCodes = @(Get-Objc3cParserReplayDiagnosticCodesFromJson -Path $run1DiagJson)
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
      $run2JsonHash = Get-Objc3cParserReplaySha256HexFromBytes -Bytes $run2JsonBytes
      try {
        $run2JsonCodes = @(Get-Objc3cParserReplayDiagnosticCodesFromJson -Path $run2DiagJson)
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
  if ($run1TxtCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $run1TxtCodes -Right $run2TxtCodes)) {
    $fixtureErrors.Add("diagnostic text code drift across replay")
  }
  if ($run1JsonCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $run1JsonCodes -Right $run2JsonCodes)) {
    $fixtureErrors.Add("diagnostic json code drift across replay")
  }
  if ($run1TxtCodes.Count -gt 0 -and $run1JsonCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $run1TxtCodes -Right $run1JsonCodes)) {
    $fixtureErrors.Add("run1 diagnostics json codes differ from text codes")
  }
  if ($run2TxtCodes.Count -gt 0 -and $run2JsonCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $run2TxtCodes -Right $run2JsonCodes)) {
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

  return [ordered]@{
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
    run1_log = Get-Objc3cParserReplayRepoRelativePath -Path $run1LogPath -RepoRoot $RepoRoot
    run2_log = Get-Objc3cParserReplayRepoRelativePath -Path $run2LogPath -RepoRoot $RepoRoot
    passed = $passed
    errors = $fixtureErrors.ToArray()
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cParserReplayFixtureCase"
)
