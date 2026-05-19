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
  Add-Objc3cParserReplayExpectedHeaderAssertions -Header $header -FixtureErrors $fixtureErrors

  $run1Exit = Invoke-Objc3cParserReplayCompile -NativeExe $NativeExe -FixturePath $Fixture.FullName -OutDir $run1OutDir -LogPath $run1LogPath
  $run2Exit = Invoke-Objc3cParserReplayCompile -NativeExe $NativeExe -FixturePath $Fixture.FullName -OutDir $run2OutDir -LogPath $run2LogPath

  Add-Objc3cParserReplayExitCodeAssertions -Run1Exit $run1Exit -Run2Exit $run2Exit -FixtureErrors $fixtureErrors

  $run1DiagTxt = Join-Path $run1OutDir "module.diagnostics.txt"
  $run2DiagTxt = Join-Path $run2OutDir "module.diagnostics.txt"
  $run1DiagJson = Join-Path $run1OutDir "module.diagnostics.json"
  $run2DiagJson = Join-Path $run2OutDir "module.diagnostics.json"

  $run1TxtPresent = Test-Path -LiteralPath $run1DiagTxt -PathType Leaf
  $run2TxtPresent = Test-Path -LiteralPath $run2DiagTxt -PathType Leaf
  $run1JsonPresent = Test-Path -LiteralPath $run1DiagJson -PathType Leaf
  $run2JsonPresent = Test-Path -LiteralPath $run2DiagJson -PathType Leaf

  Add-Objc3cParserReplayArtifactPresenceAssertions `
    -Run1TxtPresent $run1TxtPresent `
    -Run2TxtPresent $run2TxtPresent `
    -Run1JsonPresent $run1JsonPresent `
    -Run2JsonPresent $run2JsonPresent `
    -FixtureErrors $fixtureErrors

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

  Add-Objc3cParserReplayDriftAssertions `
    -Run1TxtHash $run1TxtHash `
    -Run2TxtHash $run2TxtHash `
    -Run1JsonHash $run1JsonHash `
    -Run2JsonHash $run2JsonHash `
    -Run1TxtCodes $run1TxtCodes `
    -Run2TxtCodes $run2TxtCodes `
    -Run1JsonCodes $run1JsonCodes `
    -Run2JsonCodes $run2JsonCodes `
    -FixtureErrors $fixtureErrors

  Add-Objc3cParserReplayExpectedCodeAssertions `
    -Header $header `
    -Run1TxtCodes $run1TxtCodes `
    -Run2TxtCodes $run2TxtCodes `
    -FixtureErrors $fixtureErrors

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
