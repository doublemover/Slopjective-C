$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
if ($PSVersionTable.PSVersion.Major -ge 7) {
  $PSNativeCommandUseErrorActionPreference = $false
}

Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_catalog.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_assertions.psm1") -Force -DisableNameChecking
Import-Module (Join-Path $PSScriptRoot "objc3c_diagnostics_regression_suite_reporting.psm1") -Force -DisableNameChecking

function Invoke-Objc3cDiagnosticsLoggedNativeCommand {
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

function Invoke-Objc3cDiagnosticsRegressionCase {
  param(
    [System.IO.FileInfo]$Fixture,
    [string]$RepoRoot,
    [string]$RunDir,
    [string]$ExePath
  )

  $fixtureRel = Get-Objc3cDiagnosticsRepoRelativePath -Path $Fixture.FullName -Root $RepoRoot
  $fixtureSlug = "$(Get-Objc3cDiagnosticsShortHash -Value $fixtureRel)_$($Fixture.BaseName)"
  $fixtureRunDir = Join-Path $RunDir $fixtureSlug
  $run1OutDir = Join-Path $fixtureRunDir "run1"
  $run2OutDir = Join-Path $fixtureRunDir "run2"
  $run1LogPath = Join-Path $fixtureRunDir "run1.compile.log"
  $run2LogPath = Join-Path $fixtureRunDir "run2.compile.log"
  New-Item -ItemType Directory -Force -Path $fixtureRunDir | Out-Null

  $errors = New-Object 'System.Collections.Generic.List[string]'
  $expectedSpec = Get-Objc3cDiagnosticsExpectedCodesFromFixture -FixturePath $Fixture.FullName
  if (-not $expectedSpec.header_found) {
    $null = $errors.Add("missing expected diagnostic header")
  } elseif (@($expectedSpec.codes).Count -eq 0) {
    $null = $errors.Add("expected diagnostic header has no parseable O3* codes")
  }

  $exit1 = Invoke-Objc3cDiagnosticsLoggedNativeCommand `
    -Command $ExePath `
    -Arguments @($Fixture.FullName, "--out-dir", $run1OutDir, "--emit-prefix", "module") `
    -LogPath $run1LogPath
  $exit2 = Invoke-Objc3cDiagnosticsLoggedNativeCommand `
    -Command $ExePath `
    -Arguments @($Fixture.FullName, "--out-dir", $run2OutDir, "--emit-prefix", "module") `
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

  $diag1 = Get-Objc3cDiagnosticsData -OutDir $run1OutDir
  $diag2 = Get-Objc3cDiagnosticsData -OutDir $run2OutDir
  $diagJson1 = Get-Objc3cDiagnosticsJsonData -OutDir $run1OutDir
  $diagJson2 = Get-Objc3cDiagnosticsJsonData -OutDir $run2OutDir

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
    $diagnosticsDeterministic = Test-Objc3cDiagnosticsByteArrayEqual -Left $diag1.bytes -Right $diag2.bytes
    if (-not $diagnosticsDeterministic) {
      $null = $errors.Add("diagnostics replay mismatch: run1/run2 bytes differ")
    }
  }

  $diagnosticsJsonDeterministic = $false
  if ($diagJson1.exists -and $diagJson2.exists) {
    $diagnosticsJsonDeterministic = Test-Objc3cDiagnosticsByteArrayEqual -Left $diagJson1.bytes -Right $diagJson2.bytes
    if (-not $diagnosticsJsonDeterministic) {
      $null = $errors.Add("diagnostics json replay mismatch: run1/run2 bytes differ")
    }
  }

  $run1JsonCodesMatchText = $false
  if ($diag1.exists -and $diag1.populated -and $diagJson1.exists -and $diagJson1.valid) {
    $run1JsonCodeCheck = Test-Objc3cDiagnosticsCodeSetExactMatch -Expected $diag1.codes -Actual $diagJson1.codes
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
    $run2JsonCodeCheck = Test-Objc3cDiagnosticsCodeSetExactMatch -Expected $diag2.codes -Actual $diagJson2.codes
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

  $unexpectedArtifactsRun1 = @(Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts -OutDir $run1OutDir)
  if ($unexpectedArtifactsRun1.Count -gt 0) {
    $null = $errors.Add("run1 fail-closed violation: emitted artifact(s) $($unexpectedArtifactsRun1 -join ', ')")
  }

  $unexpectedArtifactsRun2 = @(Get-Objc3cDiagnosticsUnexpectedFailClosedArtifacts -OutDir $run2OutDir)
  if ($unexpectedArtifactsRun2.Count -gt 0) {
    $null = $errors.Add("run2 fail-closed violation: emitted artifact(s) $($unexpectedArtifactsRun2 -join ', ')")
  }

  $run1CodeCheck = Test-Objc3cDiagnosticsExpectedCodesForRun -ExpectedSpec $expectedSpec -Diagnostics $diag1 -RunLabel "run1"
  $run1CodeMatch = [bool]$run1CodeCheck.codes_match
  foreach ($errorMessage in $run1CodeCheck.errors) {
    $null = $errors.Add($errorMessage)
  }
  $run2CodeCheck = Test-Objc3cDiagnosticsExpectedCodesForRun -ExpectedSpec $expectedSpec -Diagnostics $diag2 -RunLabel "run2"
  $run2CodeMatch = [bool]$run2CodeCheck.codes_match
  foreach ($errorMessage in $run2CodeCheck.errors) {
    $null = $errors.Add($errorMessage)
  }

  $passed = ($errors.Count -eq 0)

  return [pscustomobject]@{
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
    run1_diagnostics = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diag1.path -Root $RepoRoot)
    run2_diagnostics = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diag2.path -Root $RepoRoot)
    run1_diagnostics_json = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diagJson1.path -Root $RepoRoot)
    run2_diagnostics_json = (Get-Objc3cDiagnosticsRepoRelativePath -Path $diagJson2.path -Root $RepoRoot)
    run1_log = (Get-Objc3cDiagnosticsRepoRelativePath -Path $run1LogPath -Root $RepoRoot)
    run2_log = (Get-Objc3cDiagnosticsRepoRelativePath -Path $run2LogPath -Root $RepoRoot)
    out_dir = (Get-Objc3cDiagnosticsRepoRelativePath -Path $fixtureRunDir -Root $RepoRoot)
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

function Invoke-Objc3cDiagnosticsRegressionSuite {
  param([string]$ScriptRoot)

  $repoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
  $fixtureDir = Join-Path $repoRoot "tests/tooling/fixtures/native/recovery/negative"
  $suiteRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/diagnostics-regression"
  $runId = Get-Date -Format "yyyyMMdd_HHmmss_fff"
  $runDir = Join-Path $suiteRoot $runId
  $summaryPath = Join-Path $runDir "summary.json"
  $buildScript = Join-Path $repoRoot "scripts/build_objc3c_native.ps1"
  $exePath = Join-Path $repoRoot "artifacts/bin/objc3c-native.exe"
  $fatalErrorMessage = ""
  $hadFatalError = $false

  New-Item -ItemType Directory -Force -Path $runDir | Out-Null

  $results = @()

  Push-Location $repoRoot
  try {
    if (-not (Test-Path -LiteralPath $buildScript -PathType Leaf)) {
      throw "suite FAIL: missing build script at $buildScript"
    }

    if (-not (Test-Path -LiteralPath $exePath -PathType Leaf)) {
      $buildLogPath = Join-Path $runDir "build.log"
      $buildExit = Invoke-Objc3cDiagnosticsLoggedNativeCommand `
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

    $fixtures = @(Get-Objc3cDiagnosticsFixtures -FixtureDir $fixtureDir)

    foreach ($fixture in $fixtures) {
      $caseResult = Invoke-Objc3cDiagnosticsRegressionCase `
        -Fixture $fixture `
        -RepoRoot $repoRoot `
        -RunDir $runDir `
        -ExePath $exePath
      Write-Objc3cDiagnosticsCaseResult `
        -Passed $caseResult.passed `
        -FixtureRelativePath $caseResult.fixture `
        -Errors @($caseResult.errors)
      $results += $caseResult
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

  $summary = New-Objc3cDiagnosticsRegressionSummary `
    -RunId $runId `
    -RunDir $runDir `
    -RepoRoot $repoRoot `
    -Total $total `
    -PassedCount $passedCount `
    -FailedCount $failedCount `
    -Status $status `
    -FatalErrorMessage $fatalErrorMessage `
    -Results $results
  Write-Objc3cDiagnosticsRegressionSummary -Summary $summary -SummaryPath $summaryPath -RepoRoot $repoRoot

  if ($status -ne "PASS") {
    exit 1
  }
}

Export-ModuleMember -Function @(
  "Invoke-Objc3cDiagnosticsRegressionSuite"
)
