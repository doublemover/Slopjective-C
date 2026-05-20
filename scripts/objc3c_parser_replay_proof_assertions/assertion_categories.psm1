function Add-Objc3cParserReplayExpectedHeaderAssertions {
  param(
    [Parameter(Mandatory = $true)]$Header,
    [System.Collections.Generic.List[string]]$FixtureErrors
  )

  if (-not $Header.header_found) {
    $FixtureErrors.Add("missing expected diagnostic header")
  }
  if ($Header.expected_codes.Count -eq 0) {
    $FixtureErrors.Add("expected diagnostic header has no parseable diagnostic code")
  }
  if ($Header.non_parser_codes.Count -gt 0) {
    $FixtureErrors.Add("expected diagnostics must be parser codes only (found: $($Header.non_parser_codes -join ', '))")
  }
}

function Add-Objc3cParserReplayExitCodeAssertions {
  param(
    [Parameter(Mandatory = $true)][int]$Run1Exit,
    [Parameter(Mandatory = $true)][int]$Run2Exit,
    [System.Collections.Generic.List[string]]$FixtureErrors
  )

  if ($Run1Exit -eq 0) {
    $FixtureErrors.Add("run1 unexpectedly succeeded")
  }
  if ($Run2Exit -eq 0) {
    $FixtureErrors.Add("run2 unexpectedly succeeded")
  }
  if ($Run1Exit -ne $Run2Exit) {
    $FixtureErrors.Add("exit code drift across replay ($Run1Exit vs $Run2Exit)")
  }
}

function Add-Objc3cParserReplayArtifactPresenceAssertions {
  param(
    [Parameter(Mandatory = $true)][bool]$Run1TxtPresent,
    [Parameter(Mandatory = $true)][bool]$Run2TxtPresent,
    [Parameter(Mandatory = $true)][bool]$Run1JsonPresent,
    [Parameter(Mandatory = $true)][bool]$Run2JsonPresent,
    [System.Collections.Generic.List[string]]$FixtureErrors
  )

  if (-not $Run1TxtPresent) {
    $FixtureErrors.Add("run1 missing module.diagnostics.txt")
  }
  if (-not $Run2TxtPresent) {
    $FixtureErrors.Add("run2 missing module.diagnostics.txt")
  }
  if (-not $Run1JsonPresent) {
    $FixtureErrors.Add("run1 missing module.diagnostics.json")
  }
  if (-not $Run2JsonPresent) {
    $FixtureErrors.Add("run2 missing module.diagnostics.json")
  }
}

function Add-Objc3cParserReplayDriftAssertions {
  param(
    [Parameter(Mandatory = $true)][AllowEmptyString()][string]$Run1TxtHash,
    [Parameter(Mandatory = $true)][AllowEmptyString()][string]$Run2TxtHash,
    [Parameter(Mandatory = $true)][AllowEmptyString()][string]$Run1JsonHash,
    [Parameter(Mandatory = $true)][AllowEmptyString()][string]$Run2JsonHash,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Run1TxtCodes,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Run2TxtCodes,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Run1JsonCodes,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Run2JsonCodes,
    [System.Collections.Generic.List[string]]$FixtureErrors
  )

  if ($Run1TxtHash -ne "" -and $Run2TxtHash -ne "" -and $Run1TxtHash -ne $Run2TxtHash) {
    $FixtureErrors.Add("diagnostics text hash drift across replay")
  }
  if ($Run1JsonHash -ne "" -and $Run2JsonHash -ne "" -and $Run1JsonHash -ne $Run2JsonHash) {
    $FixtureErrors.Add("diagnostics json hash drift across replay")
  }
  if ($Run1TxtCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $Run1TxtCodes -Right $Run2TxtCodes)) {
    $FixtureErrors.Add("diagnostic text code drift across replay")
  }
  if ($Run1JsonCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $Run1JsonCodes -Right $Run2JsonCodes)) {
    $FixtureErrors.Add("diagnostic json code drift across replay")
  }
  if ($Run1TxtCodes.Count -gt 0 -and $Run1JsonCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $Run1TxtCodes -Right $Run1JsonCodes)) {
    $FixtureErrors.Add("run1 diagnostics json codes differ from text codes")
  }
  if ($Run2TxtCodes.Count -gt 0 -and $Run2JsonCodes.Count -gt 0 -and -not (Test-Objc3cParserReplayCodeSetsEqual -Left $Run2TxtCodes -Right $Run2JsonCodes)) {
    $FixtureErrors.Add("run2 diagnostics json codes differ from text codes")
  }
}

function Add-Objc3cParserReplayExpectedCodeAssertions {
  param(
    [Parameter(Mandatory = $true)]$Header,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Run1TxtCodes,
    [Parameter(Mandatory = $true)][AllowEmptyCollection()][string[]]$Run2TxtCodes,
    [System.Collections.Generic.List[string]]$FixtureErrors
  )

  if ($Header.expected_codes.Count -gt 0 -and $Run1TxtCodes.Count -gt 0) {
    $missing1 = @($Header.expected_codes | Where-Object { $_ -notin $Run1TxtCodes })
    if ($missing1.Count -gt 0) {
      $FixtureErrors.Add("run1 missing expected diagnostic code(s): $($missing1 -join ', ')")
    }
  }
  if ($Header.expected_codes.Count -gt 0 -and $Run2TxtCodes.Count -gt 0) {
    $missing2 = @($Header.expected_codes | Where-Object { $_ -notin $Run2TxtCodes })
    if ($missing2.Count -gt 0) {
      $FixtureErrors.Add("run2 missing expected diagnostic code(s): $($missing2 -join ', ')")
    }
  }
}
